#include "PCA9685.hpp"

PCA9685::PCA9685(PCA9685::I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);

}

bool PCA9685::i2cReadData(RegisterAddresses registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            continue;
        } else {
            if (PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t *>(&registerAddress), 1, rData,
                                        numberOfBytesToRead)) {
                return true;
            } else {
                LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus is busy";
                return false;
            }
        }
    }

}

bool PCA9685::i2cWriteData(uint8_t *tData, uint8_t numberOfBytesToWrite) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            continue;
        } else {
            if (PCA9685_TWIHS_Write(slaveAddressWrite, tData, numberOfBytesToWrite)) {
                return true;
            } else {
                LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus is busy";
                return false;
            }
        }
    }

}

void PCA9685::writeToSpecificRegister(uint8_t registerAddress, uint8_t transmittedByte) {
    auto registerAddressArray = reinterpret_cast<uint8_t *>(registerAddress);
    auto transmittedByteArray = reinterpret_cast<uint8_t *>(transmittedByte);

    i2cWriteData(registerAddressArray, static_cast<uint8_t>(sizeof registerAddressArray));
    i2cWriteData(transmittedByteArray, static_cast<uint8_t >(sizeof transmittedByteArray));
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

    auto dutyCycleStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                      (static_cast<float>(dutyCyclePercent) / 100.0f));
    auto delayStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                  (static_cast<float>(delayPercent) / 100.0f));

    uint16_t pwmTurnHighAtStepLSB = delayStepsNumber & MaskLSB;
    uint16_t pwmTurnHighAtStepMSB = delayStepsNumber & MaskMSB;

    uint16_t turnLowAtStep = delayStepsNumber + dutyCycleStepsNumber - 1;

    if (delayPercent + dutyCyclePercent > 99)
        turnLowAtStep -= GrayscaleMaximumSteps;

    uint16_t pwmTurnLowAtStepLSB = turnLowAtStep & MaskLSB;
    uint16_t pwmTurnLowAtStepMSB = turnLowAtStep & MaskMSB;

    constexpr uint8_t MSBRegisterBit4 = 0x10;

    if (dutyCyclePercent == 0) {
        /// Note: If LEDn_ON_H[4] and LEDn_OFF_H[4] are set at the same time, the LEDn_OFF_H[4] function takes precedence.
        pwmTurnLowAtStepMSB |= MSBRegisterBit4;
    } else if (dutyCyclePercent > 99) {
        pwmTurnHighAtStepMSB |= MSBRegisterBit4;
        pwmTurnLowAtStepMSB = 0;
    }

    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;

    constexpr size_t i2cTransmittedDataSize = 2 * NumberOfBytesPerPWMChannelRegisters;

    etl::array<uint8_t, i2cTransmittedDataSize> i2cTransmittedData = {LEDn_ON_L,
                                                                      static_cast<uint8_t>(pwmTurnHighAtStepLSB),
                                                                      LEDn_ON_H,
                                                                      static_cast<uint8_t>(pwmTurnHighAtStepMSB),
                                                                      LEDn_OFF_L,
                                                                      static_cast<uint8_t>(pwmTurnLowAtStepLSB),
                                                                      LEDn_OFF_H,
                                                                      static_cast<uint8_t>(pwmTurnLowAtStepMSB)};

    i2cWriteData(i2cTransmittedData.data(), static_cast<uint8_t>(sizeof i2cTransmittedData));
}

void PCA9685::setPWMChannelAlwaysOff(uint8_t channel) {
    setPWMChannel(channel, 0);
}

void PCA9685::setPWMChannelAlwaysOn(uint8_t channel, uint8_t delayPercent) {
    setPWMChannel(channel, 100, delayPercent);
}

void PCA9685::setAllPWMChannelsOff() {
    uint8_t pwmAlwaysOffMSB = 0x10;

    uint8_t tData[] = {static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_H, static_cast<uint8_t>(0)),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_H, pwmAlwaysOffMSB)};

    i2cWriteData(tData, static_cast<uint8_t>(sizeof tData));
}

void PCA9685::setAllPWMChannelsOn() {
    uint8_t pwmAlwaysOnMSB = 0x10;

    uint8_t tData[] = {static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_H, pwmAlwaysOnMSB),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_L), static_cast<uint8_t>(0),
                       static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_H, static_cast<uint8_t>(0))};

    i2cWriteData(tData, static_cast<uint8_t>(sizeof tData));
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

void PCA9685::setDeviceFrequency(uint16_t
                                 frequency) {

}

