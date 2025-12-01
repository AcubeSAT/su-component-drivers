#include "Pump/inc/Pump.hpp"

void Pump::Pump(bool sleeping, float frequency) {
    pwmGenerator.reset();
    setFrequency(frequency);
    if (sleeping) {
        enterSleep();
    } else {
        exitSleep();
    }
    setDirection(PumpDirection::ForwardDirection);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[0].pwmChannel, pwmChannelsConfig[0].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[1].pwmChannel, pwmChannelsConfig[1].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[2].pwmChannel, pwmChannelsConfig[2].delayPercent);
    pwmGenerator.setPWMChannel(pwmChannelsConfig[3].pwmChannel, pwmChannelsConfig[3].delayPercent);
}

void Pump::exitSleep() {
    pwmGenerator.exitSleepMode();
}

bool Pump::getPumpFault() {
    return (not PIO_PinRead(DRV8833_FAULT_PIN));
}

float Pump::getPumpCurrent() const {
    auto current = ina228.getCurrent();
    return current;
}

void Pump::enterSleep() {
    pwmGenerator.enterSleepMode();
}

void Pump::setDirection(PumpDirection direction) {
    ///Not yet implemented
}

void Pump::setFrequency(float frequency) {
    pwmGenerator.setPWMFrequency(frequency);
}
