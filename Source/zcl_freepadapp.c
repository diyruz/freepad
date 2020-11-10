
#include "AF.h"
#include "OSAL.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "ZComDef.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "math.h"

#include "nwk_util.h"
#include "zcl.h"
#include "zcl_diagnostic.h"
#include "zcl_freepadapp.h"
#include "zcl_general.h"
#include "zcl_lighting.h"

#include "bdb.h"
#include "bdb_interface.h"

#include "Debug.h"
#include "commissioning.h"
#include "factory_reset.h"
#include "onboard.h"
#include "tl_resetter.h"

/* HAL */
#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_led.h"

#include "battery.h"
#include "version.h"
/*********************************************************************
 * MACROS
 */
#define HAL_KEY_CODE_RELEASE_KEY HAL_KEY_CODE_NOKEY

#define HOLD_CODE 0
#define RELEASE_CODE 255

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern bool requestNewTrustCenterLinkKey;
byte zclFreePadApp_TaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

bool holdSend = false;
byte currentKeyCode = 0;
byte clicksCount = 0;

afAddrType_t inderect_DstAddr = {.addrMode = (afAddrMode_t)AddrNotPresent, .endPoint = 0, .addr.shortAddr = 0};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclFreePadApp_HandleKeys(byte shift, byte keys);
static void zclFreePadApp_SendButtonPress(uint8 endPoint, byte clicksCount);
static void zclFreePadApp_SendKeys(byte keyCode, byte pressCount, byte pressTime);
static void zclFreepadApp_SendKeysToBinds(byte keyCode, byte pressCount, byte pressTime);
static void zclFreePadApp_BasicResetCB(void);
static ZStatus_t zclFreePadApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper);
static void zclFreePadApp_SaveAttributesToNV(void);
static void zclFreePadApp_RestoreAttributesFromNV(void);
/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclFreePadApp_CmdCallbacks = {
    zclFreePadApp_BasicResetCB, // Basic Cluster Reset command
    NULL,                       // Identify Trigger Effect command
    NULL,                       // On/Off cluster commands
    NULL,                       // On/Off cluster enhanced command Off with Effect
    NULL,                       // On/Off cluster enhanced command On with Recall Global Scene
    NULL,                       // On/Off cluster enhanced command On with Timed Off
    NULL,                       // RSSI Location command
    NULL                        // RSSI Location Response command
};
// static CONST zclAttrRec_t attrs[FREEPAD_BUTTONS_COUNT];

static void zclFreePadApp_BasicResetCB(void) {
    LREPMaster("zclFreePadApp_BasicResetCB\r\n");
    zclFreePadApp_ResetAttributesToDefaultValues();
    zclFreePadApp_SaveAttributesToNV();
}

/*
this is workaround, since we don't have CB after attribute write, we will hack into write auth CB
and save attributes few secons later
*/
static ZStatus_t zclFreePadApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper) {
    LREPMaster("AUTH CB called\r\n");

    osal_start_timerEx(zclFreePadApp_TaskID, FREEPADAPP_SAVE_ATTRS_EVT, 2000);
    return ZSuccess;
}
void zclFreePadApp_Init(byte task_id) {
    // this is important to allow connects throught routers
    // to make this work, coordinator should be compiled with this flag #define TP2_LEGACY_ZC
    requestNewTrustCenterLinkKey = FALSE;

    zclFreePadApp_TaskID = task_id;
    zclFreePadApp_ResetAttributesToDefaultValues();
    zclFreePadApp_RestoreAttributesFromNV();
    zclFreePadApp_InitClusters();

    zclGeneral_RegisterCmdCallbacks(1, &zclFreePadApp_CmdCallbacks);

    zcl_registerAttrList(zclFreePadApp_SimpleDescs[0].EndPoint, zclFreePadApp_AttrsFirstEPCount, zclFreePadApp_AttrsFirstEP);
    bdb_RegisterSimpleDescriptor(&zclFreePadApp_SimpleDescs[0]);
    zcl_registerReadWriteCB(zclFreePadApp_SimpleDescs[0].EndPoint, NULL, zclFreePadApp_ReadWriteAuthCB);

    for (uint8 i = 1; i < FREEPAD_BUTTONS_COUNT; i++) {
        zcl_registerAttrList(zclFreePadApp_SimpleDescs[i].EndPoint, FREEPAD_ATTRS_COUNT, zclFreePadApp_Attrs[i - 1]);
        bdb_RegisterSimpleDescriptor(&zclFreePadApp_SimpleDescs[i]);
        zcl_registerReadWriteCB(zclFreePadApp_SimpleDescs[i].EndPoint, NULL, zclFreePadApp_ReadWriteAuthCB);
    }
    zcl_registerForMsg(zclFreePadApp_TaskID);

    // Register for all key events - This app will handle all key events
    RegisterForKeys(zclFreePadApp_TaskID);

    LREP("Started build %s \r\n", zclFreePadApp_DateCodeNT);
    ZMacSetTransmitPower(TX_PWR_PLUS_4); // set 4dBm
}

static void zclFreepadApp_SendKeysToBinds(byte keyCode, byte pressCount, bool isRelease) {
    byte button = zclFreePadApp_KeyCodeToButton(keyCode);
    uint8 endPoint = zclFreePadApp_SimpleDescs[button - 1].EndPoint;
    uint8 switchAction = zclFreePadApp_SwitchActions[button - 1];
    LREP("button %d clicks %d isRelease %d action %d\r\n", button, pressCount, isRelease, switchAction);

    switch (switchAction) {
    case ON_OFF_SWITCH_ACTIONS_OFF:
        zclGeneral_SendOnOff_CmdOff(endPoint, &inderect_DstAddr, TRUE, bdb_getZCLFrameCounter());
        break;
    case ON_OFF_SWITCH_ACTIONS_ON:
        zclGeneral_SendOnOff_CmdOn(endPoint, &inderect_DstAddr, TRUE, bdb_getZCLFrameCounter());
        break;
    case ON_OFF_SWITCH_ACTIONS_TOGGLE:
    default:
        zclGeneral_SendOnOff_CmdToggle(endPoint, &inderect_DstAddr, TRUE, bdb_getZCLFrameCounter());
        break;
    }

    if (button % 2 == 0) {
        // even numbers (2 4, send up to lower odd number)
        zclGeneral_SendLevelControlStepWithOnOff(endPoint - 1, &inderect_DstAddr, LEVEL_STEP_UP, FREEPADAPP_LEVEL_STEP_SIZE,
                                                 FREEPADAPP_LEVEL_TRANSITION_TIME, TRUE, bdb_getZCLFrameCounter());

        zclLighting_ColorControl_Send_StepColorCmd(endPoint - 1, &inderect_DstAddr, 1000, 0, 0, true, bdb_getZCLFrameCounter());
        // works zclLighting_ColorControl_Send_MoveToColorCmd( endPoint - 1, &inderect_DstAddr, 45914, 19615, 0, TRUE,
        // bdb_getZCLFrameCounter());

        // zclLighting_ColorControl_Send_StepColorCmd( endPoint - 1, &inderect_DstAddr, 1000, 1000, 0, TRUE, bdb_getZCLFrameCounter());
        // zclLighting_ColorControl_Send_StepHueCmd(endPoint - 1, &inderect_DstAddr, LIGHTING_STEP_HUE_UP, 100, 0, TRUE,
        // bdb_getZCLFrameCounter());

    } else {
        // odd number (1 3, send LEVEL_MOVE_DOWN to self)
        zclGeneral_SendLevelControlStepWithOnOff(endPoint, &inderect_DstAddr, LEVEL_STEP_DOWN, FREEPADAPP_LEVEL_STEP_SIZE,
                                                 FREEPADAPP_LEVEL_TRANSITION_TIME, TRUE, bdb_getZCLFrameCounter());

        zclLighting_ColorControl_Send_StepColorCmd(endPoint - 1, &inderect_DstAddr, -1000, 0, 0, true, bdb_getZCLFrameCounter());
        // zclLighting_ColorControl_Send_StepSaturationCmd(endPoint, &inderect_DstAddr, LIGHTING_STEP_SATURATION_UP, 100, 0, TRUE,
        // bdb_getZCLFrameCounter());
        // works zclLighting_ColorControl_Send_MoveToColorCmd( endPoint, &inderect_DstAddr, 11298, 48942, 0, TRUE,
        // bdb_getZCLFrameCounter());
    }

    if (button % 4 == 1) {
        zclLighting_ColorControl_Send_StepColorCmd(endPoint, &inderect_DstAddr, FREEPADAPP_COLOR_LEVEL_STEP_X_SIZE,
                                                   FREEPADAPP_COLOR_LEVEL_STEP_Y_SIZE, FREEPADAPP_COLOR_LEVEL_TRANSITION_TIME, true,
                                                   bdb_getZCLFrameCounter());
    } else if (button % 4 == 2) {
        zclLighting_ColorControl_Send_StepColorCmd(endPoint - 1, &inderect_DstAddr, -FREEPADAPP_COLOR_LEVEL_STEP_X_SIZE,
                                                   FREEPADAPP_COLOR_LEVEL_STEP_Y_SIZE, FREEPADAPP_COLOR_LEVEL_TRANSITION_TIME, true,
                                                   bdb_getZCLFrameCounter());
    } else if (button % 4 == 3) {
        zclLighting_ColorControl_Send_StepColorCmd(endPoint - 2, &inderect_DstAddr, FREEPADAPP_COLOR_LEVEL_STEP_X_SIZE,
                                                   FREEPADAPP_COLOR_LEVEL_STEP_Y_SIZE, FREEPADAPP_COLOR_LEVEL_TRANSITION_TIME, true,
                                                   bdb_getZCLFrameCounter());
    } else if (button % 4 == 0) {
        zclLighting_ColorControl_Send_StepColorCmd(endPoint - 3, &inderect_DstAddr, -FREEPADAPP_COLOR_LEVEL_STEP_X_SIZE,
                                                   FREEPADAPP_COLOR_LEVEL_STEP_Y_SIZE, FREEPADAPP_COLOR_LEVEL_TRANSITION_TIME, true,
                                                   bdb_getZCLFrameCounter());
    }
}

static void zclFreePadApp_SendKeys(byte keyCode, byte pressCount, bool isRelease) {
    byte button = zclFreePadApp_KeyCodeToButton(keyCode);
    uint8 endPoint = zclFreePadApp_SimpleDescs[button - 1].EndPoint;
    if (isRelease) {
        zclFreePadApp_SendButtonPress(endPoint, RELEASE_CODE);
    } else {
        zclFreePadApp_SendButtonPress(endPoint, pressCount);
    }
}

uint16 zclFreePadApp_event_loop(uint8 task_id, uint16 events) {
    afIncomingMSGPacket_t *MSGpkt;

    (void)task_id; // Intentionally unreferenced parameter
    if (events & SYS_EVENT_MSG) {
        while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclFreePadApp_TaskID))) {

            switch (MSGpkt->hdr.event) {

            case KEY_CHANGE:
                zclFreePadApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
                break;

            case ZCL_INCOMING_MSG:
                if (((zclIncomingMsg_t *)MSGpkt)->attrCmd) {
                    osal_mem_free(((zclIncomingMsg_t *)MSGpkt)->attrCmd);
                }
                break;

            default:
                break;
            }

            // Release the memory
            osal_msg_deallocate((uint8 *)MSGpkt);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & FREEPADAPP_SEND_KEYS_EVT) {
        LREPMaster("FREEPADAPP_SEND_KEYS_EVT\r\n");
        zclFreePadApp_SendKeys(currentKeyCode, clicksCount, holdSend);
        clicksCount = 0;
        currentKeyCode = 0;
        holdSend = false;
        return (events ^ FREEPADAPP_SEND_KEYS_EVT);
    }

    if (events & FREEPADAPP_HOLD_START_EVT) {
        LREPMaster("FREEPADAPP_HOLD_START_EVT\r\n");
        byte button = zclFreePadApp_KeyCodeToButton(currentKeyCode);

        uint8 endPoint = zclFreePadApp_SimpleDescs[button - 1].EndPoint;
        zclFreePadApp_SendButtonPress(endPoint, HOLD_CODE);
        holdSend = true;

        return (events ^ FREEPADAPP_HOLD_START_EVT);
    }

    if (events & FREEPADAPP_SAVE_ATTRS_EVT) {
        LREPMaster("FREEPADAPP_SAVE_ATTRS_EVT\r\n");
        zclFreePadApp_SaveAttributesToNV();
        return (events ^ FREEPADAPP_SAVE_ATTRS_EVT);
    }
    // Discard unknown events
    return 0;
}

static void zclFreePadApp_SendButtonPress(uint8 endPoint, uint8 clicksCount) {

    const uint8 NUM_ATTRIBUTES = 1;
    zclReportCmd_t *pReportCmd;
    pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
    if (pReportCmd != NULL) {
        pReportCmd->numAttr = NUM_ATTRIBUTES;
        pReportCmd->attrList[0].attrID = ATTRID_IOV_BASIC_PRESENT_VALUE;
        pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
        pReportCmd->attrList[0].attrData = (void *)(&clicksCount);

        zcl_SendReportCmd(endPoint, &inderect_DstAddr, ZCL_CLUSTER_ID_GEN_MULTISTATE_INPUT_BASIC, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR,
                          TRUE, bdb_getZCLFrameCounter());
    }
    osal_mem_free(pReportCmd);
}

static void zclFreePadApp_HandleKeys(byte shift, byte keyCode) {
    static uint32 pressTime = 0;
    static byte prevKeyCode = 0;
    if (keyCode == prevKeyCode) {
        return;
    }
    LREP("devState %d bdbNodeIsOnANetwork %d \r\n", devState, bdbAttributes.bdbNodeIsOnANetwork);

    prevKeyCode = keyCode;

    if (keyCode == HAL_KEY_CODE_RELEASE_KEY) {
        zclFactoryResetter_HandleKeys(HAL_KEY_RELEASE, keyCode);
        zclBattery_HandleKeys(HAL_KEY_RELEASE, keyCode);
        zclTouchLinkRestter_HandleKeys(HAL_KEY_RELEASE, keyCode);
        byte prevButton = zclFreePadApp_KeyCodeToButton(currentKeyCode);
        uint8 prevSwitchType = zclFreePadApp_SwitchTypes[prevButton - 1];

        switch (prevSwitchType) {
        case ON_OFF_SWITCH_TYPE_TOGGLE:
        case ON_OFF_SWITCH_TYPE_MOMENTARY:
            LREPMaster("Rlease \r\n");
            break;

        case ON_OFF_SWITCH_TYPE_MULTIFUNCTION:
        default:
            if (pressTime > 0) {
                pressTime = 0;
                osal_start_timerEx(zclFreePadApp_TaskID, FREEPADAPP_SEND_KEYS_EVT, FREEPADAPP_SEND_KEYS_DELAY);
                osal_stop_timerEx(zclFreePadApp_TaskID, FREEPADAPP_HOLD_START_EVT);
            }
            break;
        }

    } else {
        byte button = zclFreePadApp_KeyCodeToButton(keyCode);
        // TODO: implement proper keys task
        zclCommissioning_HandleKeys(HAL_KEY_PRESS, keyCode);
        if (button == 1) {
            zclFactoryResetter_HandleKeys(HAL_KEY_PRESS, keyCode);
        }

        zclTouchLinkRestter_HandleKeys(HAL_KEY_PRESS, button);

        uint8 switchType = zclFreePadApp_SwitchTypes[button - 1];

        HalLedSet(HAL_LED_2, HAL_LED_MODE_BLINK);
        pressTime = osal_getClock();

        currentKeyCode = keyCode;
        zclFreepadApp_SendKeysToBinds(keyCode, 1, false);
        switch (switchType) {
        case ON_OFF_SWITCH_TYPE_TOGGLE:
        case ON_OFF_SWITCH_TYPE_MOMENTARY:
            clicksCount = 0;
            zclFreePadApp_SendKeys(keyCode, 1, false);
            LREPMaster("ON_OFF_SWITCH_TYPE_TOGGLE or ON_OFF_SWITCH_TYPE_MOMENTARY\r\n");
            break;

        case ON_OFF_SWITCH_TYPE_MULTIFUNCTION:
        default:
            clicksCount++;
            LREPMaster("ON_OFF_SWITCH_TYPE_MULTIFUNCTION or default\r\n");
            osal_stop_timerEx(zclFreePadApp_TaskID, FREEPADAPP_SEND_KEYS_EVT);
            osal_start_timerEx(zclFreePadApp_TaskID, FREEPADAPP_HOLD_START_EVT, FREEPADAPP_HOLD_START_DELAY);
            break;
        }
    }
}

static void zclFreePadApp_RestoreAttributesFromNV(void) {
    LREPMaster("Restoring attributes to NV\r\n");

    if (osal_nv_item_init(FREEPAD_NW_SWITCH_ACTIONS, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchActions) == ZSuccess) {
        if (osal_nv_read(FREEPAD_NW_SWITCH_ACTIONS, 0, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchActions) != ZSuccess) {
            LREPMaster("fail FREEPAD_NW_SWITCH_ACTIONS\r\n");
        }
    }
    if (osal_nv_item_init(FREEPAD_NW_SWITCH_TYPES, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchTypes) == ZSuccess) {
        if (osal_nv_read(FREEPAD_NW_SWITCH_TYPES, 0, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchTypes) != ZSuccess) {
            LREPMaster("fail FREEPAD_NW_SWITCH_TYPES\r\n");
        }
    }
}
static void zclFreePadApp_SaveAttributesToNV(void) {
    LREPMaster("Saving attributes to NV\r\n");
    if (osal_nv_item_init(FREEPAD_NW_SWITCH_ACTIONS, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchActions) == ZSuccess) {
        osal_nv_write(FREEPAD_NW_SWITCH_ACTIONS, 0, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchActions);
    }
    if (osal_nv_item_init(FREEPAD_NW_SWITCH_TYPES, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchTypes) == ZSuccess) {
        osal_nv_write(FREEPAD_NW_SWITCH_TYPES, 0, FREEPAD_BUTTONS_COUNT, &zclFreePadApp_SwitchTypes);
    }
}

/****************************************************************************
****************************************************************************/
