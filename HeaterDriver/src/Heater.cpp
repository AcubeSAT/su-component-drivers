#include "HeaterDriver/inc/Heater.hpp"

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel) :
        channelMask(channelMask), pwmChannel(pwmChannel), period(PWM_ChannelPeriodGet<PeripheralNumber>(pwmChannel)) {
    stopHeater();
}

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(uint16_t period, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        period(period), channelMask(channelMask), pwmChannel(pwmChannel) {
    PWM_ChannelPeriodSet<PeripheralNumber>(pwmChannel, period);
    stopHeater();
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::startHeater() {
    PWM_ChannelsStart<PeripheralNumber>(channelMask);
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::stopHeater() {
    PWM_ChannelsStop<PeripheralNumber>(channelMask);
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setDutyPercentage(uint8_t dutyCyclePercentage) {
    this->dutyCyclePercentage = dutyCyclePercentage;
    PWM_ChannelDutySet<PeripheralNumber>(pwmChannel, convertDutyCyclePercentageToTicks());
}

template<uint8_t PeripheralNumber>
uint16_t Heater<PeripheralNumber>::convertDutyCyclePercentageToTicks() {
    return period * (1 - (dutyCyclePercentage / 100));
}