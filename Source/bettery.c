#include "battery.h"
#include "hal_adc.h"
#define MIN_VOLTAGE (uint16) 2000
#define MAX_VOLTAGE (uint16) 3300
/* (( 3 * 1.15 ) / (( Math.pow(2,14) / 2 ) - 1 )) * 1000  */
#define MULTI ((float)0.4211939934073983)

uint8 getBatteryVoltageZCL(void) { return (uint8)(getBatteryVoltage() / 100); }

uint16 getBatteryVoltage(void) {
    HalAdcSetReference(HAL_ADC_REF_125V);
    return (uint16)(HalAdcRead(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14) * MULTI);
}

/***
Specifies the remaining battery life as a half integer percentage of the full battery capacity (e.g., 34.5%, 45%, 68.5%, 90%)
with a range between zero and 100%, with 0x00 = 0%, 0x64 = 50%, and 0xC8 = 100%.
***/
uint8 getBatteryRemainingPercentageZCL(void) { 
    return 200 * (uint8) ((getBatteryVoltage() - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE));
}
