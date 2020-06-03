#include "battery.h"
#include "hal_adc.h"
#define MIN_VOLTAGE (uint16) 2000
#define MAX_VOLTAGE (uint16) 3300

uint8 getBatteryVoltageZCL(void) { 
  uint16 volt16 = getBatteryVoltage();
  uint8  volt8  = (uint8)(volt16 / 100);
  if ((volt16-(volt8*100)) > 50){
    return volt8+1;
  }else{
    return volt8; 
  }
}

uint16 getBatteryVoltage(void) {
  HalAdcSetReference(HAL_ADC_REF_125V);
  // for 1.9V ADC 4428, for 3.3V ADC 7608 // ((7608-4428)/(3300-1900))=2.271
  float result = 0;
  result = 1900+((HalAdcRead (HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14)-4428)/2.271);
  return (uint16)result;
}

/***
Specifies the remaining battery life as a half integer percentage of the full battery capacity (e.g., 34.5%, 45%, 68.5%, 90%)
with a range between zero and 100%, with 0x00 = 0%, 0x64 = 50%, and 0xC8 = 100%.
***/
uint8 getBatteryRemainingPercentageZCL(void) { 
  uint16 volt16 = getBatteryVoltage();
  if ( volt16 >= MIN_VOLTAGE) {
    return (uint8) ((((float)(volt16 - MIN_VOLTAGE)) * 200.0) / ((float) (MAX_VOLTAGE - MIN_VOLTAGE)));
  } else {
    return 0;
  }
}
