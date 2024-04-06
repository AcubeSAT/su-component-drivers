#include "HeaterDriver/inc/Heater.hpp"

template<uint8_t peripheralNumber>
Heater<peripheralNumber>::Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel) :
        channelMask(channelMask), pwmChannel(pwmChannel) {
    period = PWM_ChannelPeriodGet<peripheralNumber>(channelMask);
    stopHeater();
}

template<uint8_t peripheralNumber>
Heater<peripheralNumber>::Heater(uint16_t period, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        period(period), channelMask(channelMask), pwmChannel(pwmChannel) {
    PWM_ChannelPeriodSet<peripheralNumber>(channelMask, period);
    stopHeater();
}

template<uint8_t peripheralNumber>
void Heater<peripheralNumber>::startHeater() {
    PWM_ChannelsStart<peripheralNumber>(channelMask);
}

template<uint8_t peripheralNumber>
void Heater<peripheralNumber>::stopHeater() {
    PWM_ChannelsStop<peripheralNumber>(channelMask);
}

template<uint8_t peripheralNumber>
void Heater<peripheralNumber>::setDutyPercentage() {
    PWM_ChannelDutySet<peripheralNumber>(channelMask, convertDutyCyclePercentageToTicks(dutyCyclePercentage));
}

template<uint8_t peripheralNumber>
uint16_t Heater<peripheralNumber>::convertDutyCyclePercentageToTicks() {
    return period * (dutyCyclePercentage / 100);
}