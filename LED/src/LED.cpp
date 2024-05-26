#include "LED.hpp"

template<HAL_PWM::Peripheral PeripheralNumber, PWM_CHANNEL_NUM ChannelNum, PWM_CHANNEL_MASK ChannelMask>
void LED<PeripheralNumber, ChannelNum, ChannelMask>::openLED() {
    const auto MCURegisterPeriod = [=]() -> uint32_t {
        return MCUClockFrequency / config.ledDimmingFrequency;
    }();

    const auto MCURegisterDutyCycle = [=]() -> uint32_t {
        return (MCURegisterPeriod * config.ledBrightness) / 100;
    }();

    HAL_PWM::PWM_ChannelsStart<PeripheralNumber>(ChannelMask);
    HAL_PWM::PWM_ChannelDutySet<PeripheralNumber>(ChannelNum, MCURegisterDutyCycle);
    HAL_PWM::PWM_ChannelPeriodSet<PeripheralNumber>(ChannelNum, MCURegisterPeriod);

    ledStatus = OperationalState::LED_OPEN;
}

template<HAL_PWM::Peripheral PeripheralNumber, PWM_CHANNEL_NUM ChannelNum, PWM_CHANNEL_MASK ChannelMask>
void LED<PeripheralNumber, ChannelNum, ChannelMask>::openLED(const LED::Configuration& ledConfig) {
    config = ledConfig;
    openLED();
}
