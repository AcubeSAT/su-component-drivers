#include "Heater.hpp"
#include "Logger.hpp"

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(uint32_t frequency, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        frequency(frequency), channelMask(channelMask), pwmChannel(pwmChannel) {
    this->period = convertHzFrequencyToHarmonyPeriod();
    if (channelMask < 0 || channelMask > 3 || pwmChannel < 0 || pwmChannel > 3)
        LOG_DEBUG << "invalid channel";
    if (frequency > 0xFFFF || period < 0)
        LOG_DEBUG << "invalid frequency value";
    setPeriod(period);
}

template<uint8_t PeripheralNumber>
Heater<PeripheralNumber>::Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel):
        channelMask(channelMask), pwmChannel(pwmChannel) {
    if (channelMask < 0 || channelMask > 3 || pwmChannel < 0 || pwmChannel > 3)
        LOG_DEBUG << "invalid channel";
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::startHeater() {
    PWM_ChannelsStart<PeripheralNumber>(channelMask);
    this->heaterHasStarted = true;
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::stopHeater() {
    PWM_ChannelsStop<PeripheralNumber>(channelMask);
    this->heaterHasStarted = false;
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    bool _heaterHasStarted = heaterHasStarted;
    if (!_heaterHasStarted) startHeater();
    this->dutyCyclePercentage = dutyCyclePercentage;
    PWM_ChannelDutySet<PeripheralNumber>(pwmChannel, convertDutyCyclePercentageToTicks());
    if (!_heaterHasStarted) stopHeater();
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setPeriod(uint16_t period) {
    bool _heaterHasStarted = heaterHasStarted;
    if (!_heaterHasStarted) startHeater();
    this->period = period;
    PWM_ChannelPeriodSet<PeripheralNumber>(pwmChannel, period);
    PWM_ChannelDutySet<PeripheralNumber>(pwmChannel, convertDutyCyclePercentageToTicks());
    if (!_heaterHasStarted) stopHeater();
}

template<uint8_t PeripheralNumber>
void Heater<PeripheralNumber>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriod(convertHzFrequencyToHarmonyPeriod());
}

template<uint8_t PeripheralNumber>
uint16_t Heater<PeripheralNumber>::getPeriod() const {
    return period;
}

template<uint8_t PeripheralNumber>
uint16_t Heater<PeripheralNumber>::getFrequency() const {
    return frequency;
}

template<uint8_t PeripheralNumber>
uint8_t Heater<PeripheralNumber>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}