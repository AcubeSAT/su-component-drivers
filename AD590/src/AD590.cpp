#include "AD590.hpp"

void AD590::setADCResult(const uint16_t ADCResult) {
    adcResult = ADCResult;
}

float AD590::getTemperature() {
        return convertADCValueToTemperature(adcResult);
}

float AD590::convertADCValueToTemperature(uint16_t ADCconversion){
    const float voltageConversion = static_cast<float>(ADCconversion)/numOfBits*voltageValue;
    const float currentConversion = voltageConversion/resistorValue;
    return currentConversion - offsetCurrent + referenceTemperature ;
}
