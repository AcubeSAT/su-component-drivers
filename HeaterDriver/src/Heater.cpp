#include "Heater.hpp"
#include "HAL_PWM.hpp"

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PWMPeripheral, ChannelMask, ChannelNum>::Heater(uint32_t frequency, uint8_t dutyCyclePercentage)
        : dutyCyclePercentage(dutyCyclePercentage) {
    setFrequency(frequency);
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PWMPeripheral, ChannelMask, ChannelNum>::Heater(uint32_t frequency): frequency(frequency) {
    setFrequency(frequency);
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
Heater<PWMPeripheral, ChannelMask, ChannelNum>::Heater() {
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PWMPeripheral, ChannelMask, ChannelNum>::startHeater() {
    HAL_PWM::PWM_ChannelsStart<PWMPeripheral>(ChannelMask);
    heaterHasStarted = true;
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PWMPeripheral, ChannelMask, ChannelNum>::stopHeater() {
    HAL_PWM::PWM_ChannelsStop<PWMPeripheral>(ChannelMask);
    heaterHasStarted = false;
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PWMPeripheral, ChannelMask, ChannelNum>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->dutyCyclePercentage = dutyCyclePercentage;
    HAL_PWM::PWM_ChannelDutySet<PWMPeripheral>(ChannelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PWMPeripheral, ChannelMask, ChannelNum>::setPeriodTicks(uint16_t periodTicks) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->periodTicks = periodTicks;
    HAL_PWM::PWM_ChannelPeriodSet<PWMPeripheral>(ChannelNum, periodTicks);
    HAL_PWM::PWM_ChannelDutySet<PWMPeripheral>(ChannelNum, convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
    frequency = convertHarmonyPeriodTomHzFrequency();
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
void Heater<PWMPeripheral, ChannelMask, ChannelNum>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriodTicks(convertmHzFrequencyToHarmonyPeriod());
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint16_t Heater<PWMPeripheral, ChannelMask, ChannelNum>::getPeriodTicks() const {
    return periodTicks;
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint32_t Heater<PWMPeripheral, ChannelMask, ChannelNum>::getFrequency() const {
    return frequency;
}

template<PeripheralNumber PWMPeripheral, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
uint8_t Heater<PWMPeripheral, ChannelMask, ChannelNum>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}