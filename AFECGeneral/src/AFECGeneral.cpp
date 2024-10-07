#include "AFECGeneral.hpp"

template<AFECPeripheral AfecPeripheral>
void AFECGeneral<AfecPeripheral>::setAdcResult(uint16_t adcResult) {
    this->adcResult = adcResult;
}

template<AFECPeripheral AfecPeripheral>
AFEC_CHANNEL_NUM AFECGeneral<AfecPeripheral>::getADCChannelNum() {
    return afecChannelNum;
}

template
class AFECGeneral<AFECPeripheral::AFEC0>;

template
class AFECGeneral<AFECPeripheral::AFEC1>;