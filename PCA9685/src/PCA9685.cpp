#include "PCA9685.hpp"

PCA9685::PCA9685(PCA9685::I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);

    mode1RegisterConfiguration = {RestartDevice::DISABLED, ExternalClock::INTERNAL, RegisterAutoIncrement::DISABLED,
                                  SleepMode::NORMAL, RespondToI2CBusAddresses::SUB1_NO_RESPOND,
                                  RespondToI2CBusAddresses::SUB2_NO_RESPOND, RespondToI2CBusAddresses::SUB3_NO_RESPOND,
                                  RespondToI2CBusAddresses::ALLCALL_NO_RESPOND};

    mode2RegisterConfiguration = {OutputInvert::NOT_INVERTED, OutputChangesOn::STOP,
                                  OutputConfiguration::OPEN_DRAIN_STRUCTURE, OEPinHighStates::LOW};
}

void PCA9685::readRegister(RegisterAddresses registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead) {
    bool success = PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t *>(&registerAddress), 1, rData,
                                           numberOfBytesToRead);

    if (!success) {
        LOG_INFO << "PCA9685 with address " << i2cAddress << ": I2C bus is busy";
        return;
    }

    while (TWIHS2_IsBusy());

    if (PCA9685_TWIHS_ErrorGet != TWIHS_ERROR_NONE) {
        LOG_ERROR << "PCA9685 with address " << i2cAddress << " , is disconnected, suspending task";
        vTaskDelay(0);
    }

}

void PCA9685::writeDataToRegisters(uint8_t *tData, uint8_t numberOfBytesToWrite) {
    bool success = PCA9685_TWIHS_Write(slaveAddressWrite, tData, numberOfBytesToWrite);

    if (!success) {
        LOG_INFO << "PCA9685 with address " << i2cAddress << ": I2C bus is busy";
        return;
    }

    while (TWIHS2_IsBusy());

    if (PCA9685_TWIHS_ErrorGet != TWIHS_ERROR_NONE) {
        LOG_ERROR << "PCA9685 with address " << i2cAddress << " , is disconnected, suspending task";
        vTaskDelay(0);
    }
}

void PCA9685::writeToSpecificRegister(uint8_t registerAddress, uint8_t transmittedByte) {
    auto registerAddressArray = reinterpret_cast<uint8_t *>(registerAddress);
    auto transmittedByteArray = reinterpret_cast<uint8_t *>(transmittedByte);

    writeDataToRegisters(registerAddressArray, static_cast<uint8_t>(sizeof registerAddressArray));
    writeDataToRegisters(transmittedByteArray, static_cast<uint8_t >(sizeof transmittedByteArray));
}

void PCA9685::writeToAllRegisters() {

}

void PCA9685::setMode1Register() {
    uint8_t registerDataByte = static_cast<uint8_t>(mode1RegisterConfiguration.restart) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.externalClock) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.autoIncrement) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.sleepMode) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.sub1) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.sub2) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.sub3) |
                               static_cast<uint8_t>(mode1RegisterConfiguration.allCall);

    writeToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), registerDataByte);
}

void PCA9685::setMode2Register() {
    uint8_t registerDataByte = static_cast<uint8_t>(mode2RegisterConfiguration.outputInvert) |
                               static_cast<uint8_t>(mode2RegisterConfiguration.outputChangesOn) |
                               static_cast<uint8_t>(mode2RegisterConfiguration.outputConfiguration) |
                               static_cast<uint8_t>(mode2RegisterConfiguration.oePinHighStates);

    writeToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE2), registerDataByte);
}

void PCA9685::setPWMChannel(uint8_t channel, uint8_t dutyCyclePercent, uint8_t delayPercent) {
    // reminder: delay even if set to 0 or 100
    if (dutyCyclePercent == 0)
        setPWMChannelAlwaysOff(channel);
    else if (dutyCyclePercent > 99)
        setPWMChannelAlwaysOn(channel);

    if (delayPercent + dutyCyclePercent > 100) {
        // DO SOMETHING
    }

    uint8_t frac = static_cast<uint8_t>(100) / dutyCyclePercent;
    uint16_t dutyCycle = GrayscaleMaximumSteps / static_cast<uint16_t>(frac);

    frac = static_cast<uint8_t>(100) / delayPercent;
    uint16_t delay = GrayscaleMaximumSteps / static_cast<uint16_t>(frac) - 1;

    uint16_t turnLowAtStep = delay + dutyCycle;

    uint16_t maskLSB = 0xFF;
    uint16_t maskMSB = 0xF00;

    uint16_t pwmStartAtStepLSB = delay & maskLSB;
    uint16_t pwmStartAtStepMSB = delay & maskMSB;

    uint16_t pwmTurnLowAtStepLSB = turnLowAtStep & maskLSB;
    uint16_t pwmTurnLowAtStepMSB = turnLowAtStep & maskMSB;

    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;

    uint8_t tData[] = {LEDn_ON_L, static_cast<uint8_t>(pwmStartAtStepLSB), LEDn_ON_H,
                       static_cast<uint8_t>(pwmStartAtStepMSB), LEDn_OFF_L, static_cast<uint8_t>(pwmTurnLowAtStepLSB),
                       LEDn_OFF_H, static_cast<uint8_t>(pwmTurnLowAtStepMSB)};

    writeDataToRegisters(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setPWMChannelAlwaysOff(uint8_t channel) {
    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;

    uint8_t pwmAlwaysOffMSB = 0x10;

    uint8_t tData[] = {LEDn_ON_L, static_cast<uint8_t>(0), LEDn_ON_H, static_cast<uint8_t>(0), LEDn_OFF_L,
                       static_cast<uint8_t>(0), LEDn_OFF_H, pwmAlwaysOffMSB};

    writeDataToRegisters(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setPWMChannelAlwaysOn(uint8_t channel) {
    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;

    uint8_t pwmAlwaysOnMSB = 0x10;

    uint8_t tData[] = {LEDn_ON_L, static_cast<uint8_t>(0), LEDn_ON_H, pwmAlwaysOnMSB, LEDn_OFF_L,
                       static_cast<uint8_t>(0), LEDn_OFF_H, static_cast<uint8_t>(0)};

    writeDataToRegisters(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setAllPWMChannelsOff() {
    uint8_t pwmAlwaysOffMSB = 0x10;

    uint8_t tData[] = {static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_H, static_cast<uint8_t>(0)),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_H, pwmAlwaysOffMSB)};

    writeDataToRegisters(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setAllPWMChannelsOn() {
    uint8_t pwmAlwaysOnMSB = 0x10;

    uint8_t tData[] = {static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_H, pwmAlwaysOnMSB),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_H, static_cast<uint8_t>(0))};

    writeDataToRegisters(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent) {
    if (dutyCyclePercent == 0) {
        setAllPWMChannelsOff();
        return;
    } else if (dutyCyclePercent > 99) {
        setAllPWMChannelsOn();
        return;
    }

    for (uint8_t i = 0; i < PWMChannels; i++)
        setPWMChannel(i, dutyCyclePercent, delayPercent);

}

void PCA9685::allowAutoIncrement(bool autoIncrement) {
    if (autoIncrement)
        mode1RegisterConfiguration.autoIncrement = RegisterAutoIncrement::ENABLED;
    else
        mode1RegisterConfiguration.autoIncrement = RegisterAutoIncrement::DISABLED;
}

void PCA9685::setExternalClock(bool externalClock) {
    if (externalClock)
        mode1RegisterConfiguration.externalClock = ExternalClock::EXTERNAL;
    else
        mode1RegisterConfiguration.externalClock = ExternalClock::INTERNAL;
}

void PCA9685::setDeviceToSleep(bool sleep) {
    if (sleep)
        mode1RegisterConfiguration.sleepMode = SleepMode::SLEEP;
    else
        mode1RegisterConfiguration.sleepMode = SleepMode::NORMAL;
}

void PCA9685::restartDevice(bool restart) {
    if (restart)
        mode1RegisterConfiguration.restart = RestartDevice::ENABLED;
    else
        mode1RegisterConfiguration.restart = RestartDevice::DISABLED;
}

void PCA9685::setDeviceFrequency(uint16_t frequency) {

}

