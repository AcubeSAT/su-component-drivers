#include "AD590.hpp"

void AD590::setADCResult(uint16_t ADCResult) {
    adcResult = ADCResult;
}

etl::expected<float,bool> AD590::getTemperature() const{
    static_assert(NumOfBits > 0 && VoltageValue > 0, "Value must be above zero");
    if(ResistorValue <= 0.0f){
        return etl::unexpected(true);
    }
    const float voltageConversion = adcResult/NumOfBits*VoltageValue;
    const float currentConversion = voltageConversion/ResistorValue;
    return currentConversion - offsetCurrent + ReferenceTemperature;
}