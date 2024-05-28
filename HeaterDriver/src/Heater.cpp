#include "lib/su-component-drivers/HeaterDriver/inc/Heater.hpp"

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
Heater<PeripheralNumber, channelMask, channelNum>::Heater(uint32_t frequency, uint8_t dutyCyclePercentage):frequency(frequency) , dutyCyclePercentage(dutyCyclePercentage){
    this->periodTicks = convertHzFrequencyToHarmonyPeriod();
    setPeriodTicks(periodTicks);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
Heater<PeripheralNumber, channelMask, channelNum>::Heater(uint32_t frequency):frequency(frequency) {
    this->periodTicks = convertHzFrequencyToHarmonyPeriod();
    setPeriodTicks(periodTicks);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
Heater<PeripheralNumber, channelMask, channelNum>::Heater() {
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
void Heater<PeripheralNumber, channelMask, channelNum>::startHeater() {
    PWM_ChannelsStart<PeripheralNumber>(channelMask);
    this->heaterHasStarted = true;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
void Heater<PeripheralNumber, channelMask, channelNum>::stopHeater() {
    PWM_ChannelsStop<PeripheralNumber>(channelMask);
    this->heaterHasStarted = false;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
void Heater<PeripheralNumber, channelMask, channelNum>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->dutyCyclePercentage = dutyCyclePercentage;
    PWM_ChannelDutySet<PeripheralNumber>(channelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
void Heater<PeripheralNumber, channelMask, channelNum>::setPeriodTicks(uint16_t periodTicks) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->periodTicks = periodTicks;
    PWM_ChannelPeriodSet<PeripheralNumber>(channelNum, periodTicks);
    PWM_ChannelDutySet<PeripheralNumber>(channelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
void Heater<PeripheralNumber, channelMask, channelNum>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriodTicks(convertHzFrequencyToHarmonyPeriod());
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
uint16_t Heater<PeripheralNumber, channelMask, channelNum>::getPeriodTicks() const {
    return periodTicks;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
uint16_t Heater<PeripheralNumber, channelMask, channelNum>::getFrequency() const {
    return frequency;
}

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
uint8_t Heater<PeripheralNumber, channelMask, channelNum>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}