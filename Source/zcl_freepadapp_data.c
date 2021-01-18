#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"

#include "zcl_freepadapp.h"

#include "battery.h"
#include "version.h"
/*********************************************************************
 * CONSTANTS
 */
#define FREEPADAPP_DEVICE_VERSION 2

#define FREEPADAPP_FLAGS 0

#define FREEPADAPP_HWVERSION 1
#define FREEPADAPP_ZCLVERSION 1

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Global attributes
const uint16 zclFreePadApp_clusterRevision_all = 0x0001;

// Basic Cluster
const uint8 zclFreePadApp_HWRevision = FREEPADAPP_HWVERSION;
const uint8 zclFreePadApp_ZCLVersion = FREEPADAPP_ZCLVERSION;

#if BDB_REPORTING
const uint8 zclFreePadApp_ApplicationVersion = 2;
#else
const uint8 zclFreePadApp_ApplicationVersion = 3;
#endif
const uint8 zclFreePadApp_StackVersion = 4;

//{lenght, 'd', 'a', 't', 'a'}
const uint8 zclFreePadApp_ManufacturerName[] = {9, 'm', 'o', 'd', 'k', 'a', 'm', '.', 'r', 'u'};
#if defined(HAL_BOARD_LETV)
const uint8 zclFreePadApp_ModelId[] = {14, 'F', 'r', 'e', 'e', 'P', 'a', 'd', '_', 'L', 'e', 'T', 'V', '_', '8'};
#else
const uint8 zclFreePadApp_ModelId[] = {14, 'D', 'I', 'Y', 'R', 'u', 'Z', '_', 'F', 'r', 'e', 'e', 'P', 'a', 'd'};
#endif

const uint8 zclFreePadApp_PowerSource = POWER_SOURCE_BATTERY;

uint8 zclFreePadApp_SwitchActions[FREEPAD_BUTTONS_COUNT];
uint8 zclFreePadApp_SwitchTypes[FREEPAD_BUTTONS_COUNT];

/*********************************************************************
 * ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
 */
#define R ACCESS_CONTROL_READ
#define RR R | ACCESS_REPORTABLE
#define RW (R | ACCESS_CONTROL_WRITE | ACCESS_CONTROL_AUTH_WRITE)
#define BASIC ZCL_CLUSTER_ID_GEN_BASIC
#define POWER_CFG ZCL_CLUSTER_ID_GEN_POWER_CFG
#define SWITCH_CONFIG ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG
#define ON_OFF ZCL_CLUSTER_ID_GEN_ON_OFF
#define MS_INP_BASIC ZCL_CLUSTER_ID_GEN_MULTISTATE_INPUT_BASIC
#define ZCL_UINT8 ZCL_DATATYPE_UINT8


CONST zclAttrRec_t zclFreePadApp_AttrsFirstEP[] = {
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclFreePadApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclFreePadApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclFreePadApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclFreePadApp_HWRevision}},

    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclFreePadApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclFreePadApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclFreePadApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclFreePadApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_UINT8, R, (void *)&zclFreePadApp_ApplicationVersion}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclFreePadApp_clusterRevision_all}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_VOLTAGE, ZCL_UINT8, RR, (void *)&zclBattery_Voltage}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING, ZCL_UINT8, RR, (void *)&zclBattery_PercentageRemainig}},

    {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[0]}},
    {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[0]}}};

CONST zclAttrRec_t zclFreePadApp_Attrs[][FREEPAD_ATTRS_COUNT] = {

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[1]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[1]}}},
    
    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[2]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[2]}}},
    
    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[3]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[3]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[4]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[4]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[5]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[5]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[6]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[6]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[7]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[7]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[8]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[8]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[9]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[9]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[10]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[10]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[11]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[11]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[12]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[12]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[13]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[13]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[14]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[14]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[15]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[15]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[16]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[16]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[17]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[17]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[18]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[18]}}},

    {{SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_TYPE, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchTypes[19]}},
     {SWITCH_CONFIG, {ATTRID_ON_OFF_SWITCH_ACTIONS, ZCL_DATATYPE_ENUM8, RW, (void *)&zclFreePadApp_SwitchActions[19]}}}

};
uint8 CONST zclFreePadApp_AttrsFirstEPCount = (sizeof(zclFreePadApp_AttrsFirstEP) / sizeof(zclFreePadApp_AttrsFirstEP[0]));

const cId_t zclFreePadApp_InClusterList[] = {ZCL_CLUSTER_ID_GEN_BASIC};

#define FREEPAD_MAX_INCLUSTERS (sizeof(zclFreePadApp_InClusterList) / sizeof(zclFreePadApp_InClusterList[0]))

const cId_t zclFreePadApp_OutClusterListOdd[] = {ON_OFF, MS_INP_BASIC};
const cId_t zclFreePadApp_OutClusterListNth14[] = {ON_OFF, MS_INP_BASIC, ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
                                                 ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL};
const cId_t zclFreePadApp_OutClusterListEven[] = {ON_OFF, MS_INP_BASIC};

#define FREEPAD_MAX_OUTCLUSTERS_NTH14 (sizeof(zclFreePadApp_OutClusterListNth14) / sizeof(zclFreePadApp_OutClusterListNth14[0]))
#define FREEPAD_MAX_OUTCLUSTERS_EVEN (sizeof(zclFreePadApp_OutClusterListEven) / sizeof(zclFreePadApp_OutClusterListEven[0]))
#define FREEPAD_MAX_OUTCLUSTERS_ODD (sizeof(zclFreePadApp_OutClusterListOdd) / sizeof(zclFreePadApp_OutClusterListOdd[0]))

SimpleDescriptionFormat_t zclFreePadApp_SimpleDescs[FREEPAD_BUTTONS_COUNT];
void zclFreePadApp_InitClusters(void) {
    for (uint8 i = 0; i < FREEPAD_BUTTONS_COUNT; i++) {
        uint8 endPoint = i + 1;
        zclFreePadApp_SimpleDescs[i].EndPoint = endPoint;
        zclFreePadApp_SimpleDescs[i].AppProfId = ZCL_HA_PROFILE_ID;
        zclFreePadApp_SimpleDescs[i].AppDeviceId = ZCL_HA_DEVICEID_REMOTE_CONTROL;
        zclFreePadApp_SimpleDescs[i].AppDevVer = FREEPADAPP_DEVICE_VERSION;
        zclFreePadApp_SimpleDescs[i].Reserved = FREEPADAPP_FLAGS; // AF_V1_SUPPORT uses for AppFlags:4.

        if (endPoint == 1) {
            zclFreePadApp_SimpleDescs[i].AppNumInClusters = FREEPAD_MAX_INCLUSTERS;
            zclFreePadApp_SimpleDescs[i].pAppInClusterList = (cId_t *)zclFreePadApp_InClusterList;
        } else {
            zclFreePadApp_SimpleDescs[i].AppNumInClusters = 0;
            zclFreePadApp_SimpleDescs[i].pAppInClusterList = (cId_t *)NULL;
        }

        if (endPoint % 4 == 1) { // every 1 in 4
            zclFreePadApp_SimpleDescs[i].AppNumOutClusters = FREEPAD_MAX_OUTCLUSTERS_NTH14;
            zclFreePadApp_SimpleDescs[i].pAppOutClusterList = (cId_t *)zclFreePadApp_OutClusterListNth14;
        } else if (endPoint % 2 == 0) {
            zclFreePadApp_SimpleDescs[i].AppNumOutClusters = FREEPAD_MAX_OUTCLUSTERS_EVEN;
            zclFreePadApp_SimpleDescs[i].pAppOutClusterList = (cId_t *)zclFreePadApp_OutClusterListEven;
        } else {
            zclFreePadApp_SimpleDescs[i].AppNumOutClusters = FREEPAD_MAX_OUTCLUSTERS_ODD;
            zclFreePadApp_SimpleDescs[i].pAppOutClusterList = (cId_t *)zclFreePadApp_OutClusterListOdd;
        }
    }
}

byte zclFreePadApp_KeyCodeToButton(byte key) {
    switch (key) {
#if defined(HAL_BOARD_FREEPAD)
    case 0x9: // row=4 col=4
        return 1;
    case 0xa: // row=4 col=8
        return 2;
    case 0xc: // row=4 col=16
        return 3;
    case 0x8: // row=4 col=32
        return 4;
    case 0x11: // row=8 col=4
        return 5;
    case 0x12: // row=8 col=8
        return 6;
    case 0x14: // row=8 col=16
        return 7;
    case 0x18: // row=8 col=32
        return 8;
    case 0x21: // row=16 col=4
        return 9;
    case 0x22: // row=16 col=8
        return 10;
    case 0x24: // row=16 col=16
        return 11;
    case 0x28: // row=16 col=32
        return 12;
    case 0x41: // row=32 col=4
        return 13;
    case 0x42: // row=32 col=8
        return 14;
    case 0x44: // row=32 col=16
        return 15;
    case 0x48: // row=32 col=32
        return 16;
    case 0x81: // row=64 col=4
        return 17;
    case 0x82: // row=64 col=8
        return 18;
    case 0x84: // row=64 col=16
        return 19;
    case 0x88: // row=64 col=32
        return 20;
#elif defined(HAL_BOARD_LETV)
    case 0x1: // 1
        return 1;
    case 0x2: // 2
        return 2;
    case 0x3: // 3
        return 3;
    case 0x4: // 4
        return 4;
    case 0x5: // 5
        return 5;
    case 0x6: // 6
        return 6;
    case 0x7: // 7
        return 7;
    case 0x8: // 8
        return 8;
#elif defined(HAL_BOARD_CHDTECH_DEV)
    case 0x1: // row=4 col=4
        return 1;
    case 0x2: // row=4 col=8
        return 2;
#endif

    default:
        return HAL_UNKNOWN_BUTTON;
        break;
    }
}

void zclFreePadApp_ResetAttributesToDefaultValues(void) {
    for (uint8 i = 0; i < FREEPAD_BUTTONS_COUNT; i++) {
        zclFreePadApp_SwitchActions[i] = ON_OFF_SWITCH_ACTIONS_TOGGLE;
        zclFreePadApp_SwitchTypes[i] = ON_OFF_SWITCH_TYPE_MULTIFUNCTION;
    }
}