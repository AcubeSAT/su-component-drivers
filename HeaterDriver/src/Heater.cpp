#include "Heater.hpp"

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PeripheralNumber, ChannelMask, ChannelNum>::Heater(uint32_t frequency, uint8_t dutyCyclePercentage)
        : dutyCyclePercentage(dutyCyclePercentage) {
    setFrequency(frequency);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PeripheralNumber, ChannelMask, ChannelNum>::Heater(uint32_t frequency): frequency(frequency) {
    setFrequency(frequency);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PeripheralNumber, ChannelMask, ChannelNum>::Heater() {
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PeripheralNumber, ChannelMask, ChannelNum>::startHeater() {
    HAL_PWM::PWM_ChannelsStart<PeripheralNumber>(ChannelMask);
    heaterHasStarted = true;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PeripheralNumber, ChannelMask, ChannelNum>::stopHeater() {
    HAL_PWM::PWM_ChannelsStop<PeripheralNumber>(ChannelMask);
    heaterHasStarted = false;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PeripheralNumber, ChannelMask, ChannelNum>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->dutyCyclePercentage = dutyCyclePercentage;
    HAL_PWM::PWM_ChannelDutySet<PeripheralNumber>(ChannelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PeripheralNumber, ChannelMask, ChannelNum>::setPeriodTicks(uint16_t periodTicks) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->periodTicks = periodTicks;
    HAL_PWM::PWM_ChannelPeriodSet<PeripheralNumber>(ChannelNum, periodTicks);
    HAL_PWM::PWM_ChannelDutySet<PeripheralNumber>(ChannelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
    frequency = convertHarmonyPeriodTomHzFrequency();
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PeripheralNumber, ChannelMask, ChannelNum>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriodTicks(convertmHzFrequencyToHarmonyPeriod());
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint16_t Heater<PeripheralNumber, ChannelMask, ChannelNum>::getPeriodTicks() const {
    return periodTicks;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint32_t Heater<PeripheralNumber, ChannelMask, ChannelNum>::getFrequency() const {
    return frequency;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint8_t Heater<PeripheralNumber, ChannelMask, ChannelNum>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}