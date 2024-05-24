#include "Heater.hpp"

template<uint8_t PeripheralNumber, uint8_t channel>
Heater<PeripheralNumber, channel>::Heater(uint32_t frequency):frequency(frequency) {
    this->periodTicks = convertHzFrequencyToHarmonyPeriod();
    setPeriodTicks(periodTicks);
}

template<uint8_t PeripheralNumber, uint8_t channel>
Heater<PeripheralNumber, channel>::Heater() {
    setDutyCyclePercentage(dutyCyclePercentage);
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::startHeater() {
    PWM_ChannelsStart<PeripheralNumber,channel>();
    this->heaterHasStarted = true;
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::stopHeater() {
    PWM_ChannelsStop<PeripheralNumber,channel>();
    this->heaterHasStarted = false;
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::setDutyCyclePercentage(uint8_t dutyCyclePercentage) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->dutyCyclePercentage = dutyCyclePercentage;
    PWM_ChannelDutySet<PeripheralNumber,channel>(convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::setPeriodTicks(uint16_t periodTicks) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->periodTicks = periodTicks;
    PWM_ChannelPeriodSet<PeripheralNumber,channel>(periodTicks);
    PWM_ChannelDutySet<PeripheralNumber,channel>(convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriodTicks(convertHzFrequencyToHarmonyPeriod());
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint16_t Heater<PeripheralNumber, channel>::getPeriodTicks() const {
    return periodTicks;
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint16_t Heater<PeripheralNumber, channel>::getFrequency() const {
    return frequency;
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint8_t Heater<PeripheralNumber, channel>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}