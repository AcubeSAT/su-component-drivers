#include "lib/su-component-drivers/AfecGeneral/inc/AfecGeneral.hpp"

template<AFECPeripheral AfecPeripheral>
void  AfecGeneral<AfecPeripheral>::setADCResult(uint16_t _ADCResult){
    ADCResult=_ADCResult;
}

template<AFECPeripheral AfecPeripheral>
AFEC_CHANNEL_NUM  AfecGeneral<AfecPeripheral>::getADCChannelNum(){
    return afecChannelNum;
}

template class AfecGeneral<AFECPeripheral::AFEC0>;
template class AfecGeneral<AFECPeripheral::AFEC1>;