#include "Heater.hpp"

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(uint16_t period, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        channelMask(channelMask), pwmChannel(pwmChannel), period(period) {
    if (!isValid(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM
    pwmChannel))
    LOG_DEBUG << "invalid channel";
    if (!is_uint16(period))
        LOG_DEBUG << "invalid period value";
    setPeriod(period);
}

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        channelMask(channelMask), pwmChannel(pwmChannel) {
    if (!isValid(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM
    pwmChannel))
    LOG_DEBUG << "invalid channel";
    setDutyCyclePercentage(dutyCyclePercentage);
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
void Heater<PeripheralNumber>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    startHeater();
    this->dutyCyclePercentage = dutyCyclePercentage;
    PWM_ChannelDutySet<PeripheralNumber>(pwmChannel, convertDutyCyclePercentageToTicks());
    stopHeater();
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setPeriod(uint16_t period) {
    startHeater();
    PWM_ChannelPeriodSet<PeripheralNumber>(pwmChannel, period);
    stopHeater();
    this->period = period;
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber>
uint16_t Heater<PeripheralNumber>::getPeriod() const {
    return period;
}

template<uint8_t PeripheralNumber>
uint8_t Heater<PeripheralNumber>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}