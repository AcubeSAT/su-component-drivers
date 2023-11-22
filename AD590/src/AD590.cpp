#include "AD590.hpp"

void AD590::setADCResult(const uint16_t ADCResult) {
    adcResult = ADCResult;
}

float AD590::getTemperature() const{
    static_assert(numOfBits > 0 || resistorValue > 0,"Value must be above zero");
    const float voltageConversion = adcResult/numOfBits*voltageValue;
    const float currentConversion = voltageConversion/resistorValue;
    return currentConversion - offsetCurrent + referenceTemperature;
}