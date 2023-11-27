#include "AD590.hpp"

void AD590::setADCResult(uint16_t ADCResult) {
    adcResult = ADCResult;
}

etl::expected<float,bool> AD590::getTemperature() const{
    static_assert(numOfBits > 0 ,"Value must be above zero");
    if(resistorValue <= 0.0f){
        return etl::unexpected(true);
    }
    const float voltageConversion = adcResult/numOfBits*voltageValue;
    const float currentConversion = voltageConversion/resistorValue;
    return currentConversion - offsetCurrent + referenceTemperature;
}