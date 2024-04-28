#include "lib/su-component-drivers/HeaterDriver/inc/Heater.hpp"

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(uint16_t period, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        channelMask(channelMask), pwmChannel(pwmChannel), period(period) {
    startHeater();
    setPeriod(period);
    stopHeater();
}

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        period(PWM_ChannelPeriodGet<PeripheralNumber>(pwmChannel)), channelMask(channelMask), pwmChannel(pwmChannel) {
    startHeater();
    setDutyPercentage(dutyCyclePercentage);
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
    return period * dutyCyclePercentage / 100;
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setPeriod(uint16_t period) {
    PWM_ChannelPeriodSet<PeripheralNumber>(pwmChannel, period);
    this->period = period;
    setDutyPercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber>
uint16_t Heater<PeripheralNumber>::getPeriod() {
    return period;
}

template<uint8_t PeripheralNumber>
uint8_t Heater<PeripheralNumber>::getDutyCyclePercentage() {
    return dutyCyclePercentage;
}