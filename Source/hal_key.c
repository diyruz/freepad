/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "hal_key.h"
#include "Debug.h"
#include "hal_adc.h"
#include "hal_defs.h"
#include "hal_drivers.h"
#include "hal_led.h"
#include "hal_mcu.h"
#include "hal_types.h"
#include "osal.h"
#include <stdio.h>

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/

#define HAL_KEY_BIT0 0x01
#define HAL_KEY_BIT1 0x02
#define HAL_KEY_BIT2 0x04
#define HAL_KEY_BIT3 0x08
#define HAL_KEY_BIT4 0x10
#define HAL_KEY_BIT5 0x20
#define HAL_KEY_BIT6 0x40
#define HAL_KEY_BIT7 0x80

#define HAL_KEY_RISING_EDGE 0
#define HAL_KEY_FALLING_EDGE 1

#define HAL_KEY_DEBOUNCE_VALUE 25 // TODO: adjust this value

#if defined(HAL_BOARD_FREEPAD_20)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4 | HAL_KEY_BIT5 | HAL_KEY_BIT6)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4 | HAL_KEY_BIT5 | HAL_KEY_BIT6)
#elif defined(HAL_BOARD_FREEPAD_12)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4)
#elif defined(HAL_BOARD_FREEPAD_8)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3)
#elif defined(HAL_BOARD_FREEPAD_3)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2)
#elif defined(HAL_BOARD_FREEPAD_2)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2)
#elif defined(HAL_BOARD_FREEPAD_1)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT2)
#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT2)
#elif defined(HAL_BOARD_CHDTECH_DEV)
#define HAL_KEY_P0_GPIO_PINS (HAL_KEY_BIT1)
#define HAL_KEY_P1_GPIO_PINS 0x00
#define HAL_KEY_P2_GPIO_PINS (HAL_KEY_BIT0)

#define HAL_KEY_P0_INPUT_PINS (HAL_KEY_BIT1)
#define HAL_KEY_P1_INPUT_PINS 0x00
#define HAL_KEY_P2_INPUT_PINS (HAL_KEY_BIT0)

#define HAL_KEY_P0_INTERRUPT_PINS (HAL_KEY_BIT1)
#define HAL_KEY_P1_INTERRUPT_PINS 0x00
#define HAL_KEY_P2_INTERRUPT_PINS (HAL_KEY_BIT0)
#endif

#if defined(HAL_BOARD_FREEPAD_20) || defined(HAL_BOARD_FREEPAD_12) || defined(HAL_BOARD_FREEPAD_8) || defined(HAL_BOARD_FREEPAD_3) || defined(HAL_BOARD_FREEPAD_2) || defined(HAL_BOARD_FREEPAD_1)

#define HAL_KEY_P1_GPIO_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4 | HAL_KEY_BIT5)
#define HAL_KEY_P2_GPIO_PINS 0x00

#define HAL_KEY_P1_INPUT_PINS (HAL_KEY_BIT2 | HAL_KEY_BIT3 | HAL_KEY_BIT4 | HAL_KEY_BIT5)

#define HAL_KEY_P0_INTERRUPT_PINS HAL_KEY_P0_INPUT_PINS
#define HAL_KEY_P1_INTERRUPT_PINS 0x00

#define HAL_KEY_P2_INTERRUPT_PINS 0x00
#endif

/**************************************************************************************************
 *                                            TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/
static halKeyCBack_t pHalKeyProcessFunction;
bool Hal_KeyIntEnable;

uint8 halSaveIntKey; /* used by ISR to save state of interrupt-driven keys */

static uint8 halKeyTimerRunning; // Set to true while polling timer is running in interrupt
                                 // enabled mode

/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
void halProcessKeyInterrupt(void);

void HalKeyInit(void) {
    P0SEL &= ~HAL_KEY_P0_GPIO_PINS;
    P1SEL &= ~HAL_KEY_P1_GPIO_PINS;
    P2SEL &= ~HAL_KEY_P2_GPIO_PINS;

#if defined(HAL_BOARD_FREEPAD_20) || defined(HAL_BOARD_FREEPAD_12) || defined(HAL_BOARD_FREEPAD_8) || defined(HAL_BOARD_FREEPAD_3) || defined(HAL_BOARD_FREEPAD_2) || defined(HAL_BOARD_FREEPAD_1)
    /**
     * columns (p1)
     **/

    P1DIR |= HAL_KEY_P1_INPUT_PINS; // set as output
    P1 |= HAL_KEY_P1_INPUT_PINS;    // set high
    P1INP |= HAL_KEY_P1_INPUT_PINS; // set tri state
    ////////////////////////////////////////////
    /**
     * row pins
     **/

    P0DIR &= ~HAL_KEY_P0_INPUT_PINS; // set input
    P0INP &= ~HAL_KEY_P0_INPUT_PINS; // pull pins
    P2INP |= HAL_KEY_BIT5;           // pull down port0
    HAL_BOARD_DELAY_USEC(50);
#elif defined(HAL_BOARD_CHDTECH_DEV)
    P0DIR &= ~(HAL_KEY_P0_INPUT_PINS);
    P1DIR &= ~(HAL_KEY_P1_INPUT_PINS);
    P2DIR &= ~(HAL_KEY_P2_INPUT_PINS);

#endif
    pHalKeyProcessFunction = NULL;

    halKeyTimerRunning = FALSE;
}

void HalKeyConfig(bool interruptEnable, halKeyCBack_t cback) {
    Hal_KeyIntEnable = true;
    pHalKeyProcessFunction = cback;
    P0IEN |= HAL_KEY_P0_INTERRUPT_PINS;
    P1IEN |= HAL_KEY_P1_INTERRUPT_PINS;
    P2IEN |= HAL_KEY_P2_INTERRUPT_PINS;
#if defined(HAL_BOARD_FREEPAD_20) || defined(HAL_BOARD_FREEPAD_12) || defined(HAL_BOARD_FREEPAD_8) || defined(HAL_BOARD_FREEPAD_3) || defined(HAL_BOARD_FREEPAD_2) || defined(HAL_BOARD_FREEPAD_1)
    PICTL &= ~HAL_KEY_BIT0; // set rising edge on port 0
                            // enable intrupt on row pins
    IEN1 |= HAL_KEY_BIT5;   // enable port0 int
#elif defined(HAL_BOARD_CHDTECH_DEV)
    PICTL |= HAL_KEY_BIT0 | HAL_KEY_BIT3; // set falling edge on port 0 and 2
    IEN1 |= HAL_KEY_BIT5;                 // enable port0 int
    IEN2 |= HAL_KEY_BIT1;                 // enable port2 int

#endif
}
uint8 HalKeyRead(void) {

    uint8 key = HAL_KEY_CODE_NOKEY;
#if defined(HAL_BOARD_FREEPAD_20) || defined(HAL_BOARD_FREEPAD_12) || defined(HAL_BOARD_FREEPAD_8) || defined(HAL_BOARD_FREEPAD_3) || defined(HAL_BOARD_FREEPAD_2) || defined(HAL_BOARD_FREEPAD_1)
    uint8 row, col;
    row = P0 & HAL_KEY_P0_INPUT_PINS;

    if (row) {
        // row is 0x8, 0x10, 0x20, 0x40 or , 0x80 depending on row that is pressed
        // Now find out which col it was by changing row IO's to input and driving
        // the col IO's as O/P.
        col = 0;
        // set pull downs on col pins

        // P1 &= ~HAL_KEY_P1_INPUT_PINS;
        P1DIR &= ~HAL_KEY_P1_INPUT_PINS; // set p1 as input
        P1INP &= ~HAL_KEY_P1_INPUT_PINS; // set as pull pins
        P2INP |= HAL_KEY_BIT6;           // pull down port1

        // P1 &= ~HAL_KEY_P1_INPUT_PINS; //set low on p1 pins

        P0INP |= HAL_KEY_P0_INPUT_PINS; // set as tri state
        P0DIR |= HAL_KEY_P0_INPUT_PINS; // set p0 as output
        P0 |= HAL_KEY_P0_INPUT_PINS;    // set high on p0

        HAL_BOARD_DELAY_USEC(50);

        col = P1 & HAL_KEY_P1_INPUT_PINS;

        // set P0:0-2 as output - high ready for next press
        P1DIR |= HAL_KEY_P1_INPUT_PINS; // set as output
        P1 |= HAL_KEY_P1_INPUT_PINS;    // set high
        // all pulls tri-states
        P1INP |= HAL_KEY_P1_INPUT_PINS; // set tri state

        P0DIR &= ~HAL_KEY_P0_INPUT_PINS; // set input
        P0INP &= ~HAL_KEY_P0_INPUT_PINS; // pull pins
        P2INP |= HAL_KEY_BIT5;           // pull down port0

        key = (((row << 2) | col >> 1)) >> 1;
    }
    // LREP("row %d col %d key 0x%X %d \r\n", row, col, key, key);
#elif defined(HAL_BOARD_CHDTECH_DEV)

    if (ACTIVE_LOW(P0 & HAL_KEY_P0_INPUT_PINS)) {
        key = 0x01;
    }

    if (ACTIVE_LOW(P2 & HAL_KEY_P2_INPUT_PINS)) {
        key = 0x02;
    }
#endif

    return key;
}
void HalKeyPoll(void) {
    uint8 keys = 0;

    keys = HalKeyRead();

    if (pHalKeyProcessFunction) {
        (pHalKeyProcessFunction)(keys, HAL_KEY_STATE_NORMAL);
    }

    if (keys != HAL_KEY_CODE_NOKEY) {
        osal_start_timerEx(Hal_TaskID, HAL_KEY_EVENT, 200);
    } else {
        halKeyTimerRunning = FALSE;
    }
}
void halProcessKeyInterrupt(void) {
    if (!halKeyTimerRunning) {
        halKeyTimerRunning = TRUE;
        osal_start_timerEx(Hal_TaskID, HAL_KEY_EVENT, HAL_KEY_DEBOUNCE_VALUE);
    }
}

void HalKeyEnterSleep(void) {
    uint8 clkcmd = CLKCONCMD;
    uint8 clksta = CLKCONSTA;
    // Switch to 16MHz before setting the DC/DC to bypass to reduce risk of flash corruption
    CLKCONCMD = (CLKCONCMD_16MHZ | OSC_32KHZ);
    // wait till clock speed stablizes
    while (CLKCONSTA != (CLKCONCMD_16MHZ | OSC_32KHZ))
        ;

    CLKCONCMD = clkcmd;
    while (CLKCONSTA != (clksta))
        ;
}

uint8 HalKeyExitSleep(void) {
    uint8 clkcmd = CLKCONCMD;
    // Switch to 16MHz before setting the DC/DC to on to reduce risk of flash corruption
    CLKCONCMD = (CLKCONCMD_16MHZ | OSC_32KHZ);
    // wait till clock speed stablizes
    while (CLKCONSTA != (CLKCONCMD_16MHZ | OSC_32KHZ))
        ;

    CLKCONCMD = clkcmd;

    /* Wake up and read keys */
    return (HalKeyRead());
}

HAL_ISR_FUNCTION(halKeyPort0Isr, P0INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P0IFG & HAL_KEY_P0_INTERRUPT_PINS) {
        halProcessKeyInterrupt();
    }

    P0IFG &= ~HAL_KEY_P0_INTERRUPT_PINS;
    P0IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}

HAL_ISR_FUNCTION(halKeyPort1Isr, P1INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P1IFG & HAL_KEY_P1_INTERRUPT_PINS) {
        halProcessKeyInterrupt();
    }

    P1IFG &= ~HAL_KEY_P1_INTERRUPT_PINS;
    P1IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}

HAL_ISR_FUNCTION(halKeyPort2Isr, P2INT_VECTOR) {
    HAL_ENTER_ISR();

    if (P2IFG & HAL_KEY_P2_INTERRUPT_PINS) {
        halProcessKeyInterrupt();
    }

    P2IFG &= ~HAL_KEY_P2_INTERRUPT_PINS;
    P2IF = 0;

    CLEAR_SLEEP_MODE();
    HAL_EXIT_ISR();
}