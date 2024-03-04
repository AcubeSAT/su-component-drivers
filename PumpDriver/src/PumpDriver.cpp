#include "PumpDriver.hpp"

PumpDriver::PumpDriver() = default;

void PumpDriver::init() {
    pwmGenerator.reset();

    pwmGenerator.setPWMChannel(pwmChannelsConfig[0].pwmChannel, pwmChannelsConfig[0].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[1].pwmChannel, pwmChannelsConfig[1].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[2].pwmChannel, pwmChannelsConfig[2].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[3].pwmChannel, pwmChannelsConfig[3].delayPercent);

    pwmGenerator.exitSleepMode();
}

void PumpDriver::open() {

}

void PumpDriver::close() {

}

void PumpDriver::setPumpSpeed(uint8_t speed) {

}
