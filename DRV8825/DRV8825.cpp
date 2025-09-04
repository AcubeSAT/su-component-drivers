#include "DRV8825.hpp"

#include "plib_pio.h"

DRV8825::DRV8825(PumpStepMode mode, bool sleeping, float frequency) : pca9685{PcaAddress}, ina{InaAddress}, dac{DacAddress,3.3f} {

    pca9685.reset();
    pca9685.setAllPWMChannelsOff();
    pca9685.setPWMFrequency(frequency);

    if (sleeping) {
        enterSleep();
    } else {
        exitSleep();
    }

    setDirection(PumpDirection::ForwardDirection);

    pca9685.setPWMChannel(Step, StepDutyCycle, 0);

    bool modeBit0 = static_cast<uint8_t>(mode) & 1;

    bool modeBit1 = static_cast<uint8_t>(mode) & 2;

    pca9685.setPWMChannel(Mode0, 100 * modeBit0, 0);

    pca9685.setPWMChannel(Mode1, 100 * modeBit1, 0);

    //Investigate required configuration, using default for now
    //pca9685.start();

    //Possibly unnecessary delay before stopping the reset signal
    vTaskDelay(200 / portTICK_PERIOD_MS);

    pca9685.setPWMChannelAlwaysOn(Reset, 0);


}

void DRV8825::exitSleep() {
    pca9685.setPWMChannelAlwaysOn(Sleep, 0.0f);
}

void DRV8825::enterSleep() {
    //TODO: Investigate if the pca itself can also be placed in sleep mode here
    pca9685.setPWMChannelAlwaysOff(Sleep);
}

void DRV8825::setDirection(PumpDirection direction) {
    if (direction == PumpDirection::ReverseDirection) {
        LOG_WARNING << "Pump has been set to Reverse!";
        pca9685.setPWMChannelAlwaysOff(Dir);
    } else {
        pca9685.setPWMChannelAlwaysOn(Dir, 0);
    }
}

bool DRV8825::getPumpFault() {
    auto activeLowFault = DRV8833_FAULT_Get();

    return !activeLowFault;
}

float DRV8825::getPumpCurrent() const
{
    return ina.getCurrent();
}
