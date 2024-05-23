#include "lib/su-component-drivers/HeaterDriver/inc/Heater.hpp"

template<uint8_t PeripheralNumber, uint8_t channel>
Heater<PeripheralNumber, channel>::Heater(uint32_t frequency):frequency(frequency) {
    this->period = convertHzFrequencyToHarmonyPeriod();
    setPeriod(period);
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
void Heater<PeripheralNumber, channel>::setPeriod(uint16_t period) {
    bool _heaterHasStarted = heaterHasStarted;
    if (not _heaterHasStarted) {
        startHeater();
    }
    this->period = period;
    PWM_ChannelPeriodSet<PeripheralNumber,channel>(period);
    PWM_ChannelDutySet<PeripheralNumber,channel>(convertDutyCyclePercentageToTicks());
    if (not _heaterHasStarted) {
        stopHeater();
    }
}

template<uint8_t PeripheralNumber, uint8_t channel>
void Heater<PeripheralNumber, channel>::setFrequency(uint32_t frequency) {
    this->frequency = frequency;
    setPeriod(convertHzFrequencyToHarmonyPeriod());
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint16_t Heater<PeripheralNumber, channel>::getPeriod() const {
    return period;
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint16_t Heater<PeripheralNumber, channel>::getFrequency() const {
    return frequency;
}

template<uint8_t PeripheralNumber, uint8_t channel>
uint8_t Heater<PeripheralNumber, channel>::getDutyCyclePercentage() const {
    return dutyCyclePercentage;
}