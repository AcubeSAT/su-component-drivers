#include "Pump.hpp"

void Pump::init() {
    pwmGenerator.reset();

    pwmGenerator.setPWMChannel(pwmChannelsConfig[0].pwmChannel, pwmChannelsConfig[0].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[1].pwmChannel, pwmChannelsConfig[1].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[2].pwmChannel, pwmChannelsConfig[2].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[3].pwmChannel, pwmChannelsConfig[3].delayPercent);
}

void Pump::openPump() {
    pwmGenerator.exitSleepMode();
}

void Pump::closePump() {
    pwmGenerator.enterSleepMode();
}

void Pump::setSpeed(float speed) {
    pwmGenerator.setPWMFrequency();
}
