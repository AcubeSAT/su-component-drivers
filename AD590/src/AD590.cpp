#include "AD590.hpp"

void AD590::setADCResult(const uint16_t ADCResult) {
    adcResult = ADCResult;
}

float AD590::getTemperature(){
    const float voltageConversion = static_cast<float>(adcResult)/numOfBits*voltageValue;
    const float currentConversion = voltageConversion/resistorValue;
    return currentConversion - offsetCurrent + referenceTemperature ;
}