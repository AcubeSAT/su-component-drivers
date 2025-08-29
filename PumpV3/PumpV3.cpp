#include "PumpV3.hpp"

#include "plib_pio.h"

PumpV3::PumpV3(PumpStepMode mode, bool sleeping, float frequency, bool direction) : pca9685{PcaAddress} {

    pca9685.setAllPWMChannelsOff();

    pca9685.setPWMFrequency(frequency);

    if (sleeping) {
        enterSleep();
    }
    else {
        exitSleep();
    }

    setDirection(direction);

    pca9685.setPWMChannel(Step, 50, 0);

    bool modeBit0 = static_cast<uint8_t>(mode) & 1;

    bool modeBit1 = static_cast<uint8_t>(mode) & 2;

    pca9685.setPWMChannel(Mode0, 100 * modeBit0,0);

    pca9685.setPWMChannel(Mode1, 100 * modeBit1,0);

    //Investigate required configuration, using default for now
    //pca9685.start();

    //Possibly unnecessary delay before stopping the reset signal
    vTaskDelay(200 / portTICK_PERIOD_MS);

    pca9685.setPWMChannelAlwaysOn(Reset, 0);
}

void PumpV3::exitSleep() {
    pca9685.setPWMChannelAlwaysOn(Sleep,0.0f);
}

void PumpV3::enterSleep() {
    //TODO: Investigate if the pca itself can also be placed in sleep mode here
    pca9685.setPWMChannelAlwaysOff(Sleep);
}

void PumpV3::setDirection(bool direction) {
    pca9685.setPWMChannel(Dir, 100 * direction, 0);
}

bool PumpV3::getPumpFault() {

    auto activeLowFault = DRV8833_FAULT_Get();

    return !activeLowFault;
}
