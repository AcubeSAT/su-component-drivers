#include "PCA9685.hpp"

PCA9685::PCA9685(I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);

    initMode1Register();
    initMode2Register();

}

bool PCA9685::i2cReadData(RegisterAddresses registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus is busy";
            continue;
        } else {
            if (PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t *>(&registerAddress), 1, rData,
                                        numberOfBytesToRead)) {
                return true;
            } else {
                LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus NAK";
                return false;
            }
        }
    }

}

bool PCA9685::i2cWriteData(uint8_t *tData, uint8_t numberOfBytesToWrite) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus is busy";
            continue;
        } else {
            if (PCA9685_TWIHS_Write(slaveAddressWrite, tData, numberOfBytesToWrite)) {
                return true;
            } else {
                LOG_INFO << "PCA9685 (" << i2cAddress << ") was not able to perform any transaction: I2C bus NAK";
                return false;
            }
        }
    }

}

void PCA9685::i2cWriteToSpecificRegister(uint8_t registerAddress, uint8_t transmittedByte) {
    auto registerAddressArray = reinterpret_cast<uint8_t *>(registerAddress);
    auto transmittedByteArray = reinterpret_cast<uint8_t *>(transmittedByte);

    i2cWriteData(registerAddressArray, static_cast<uint8_t>(sizeof registerAddressArray));
    i2cWriteData(transmittedByteArray, static_cast<uint8_t>(sizeof transmittedByteArray));
}

void PCA9685::initMode1Register() {

    if (mode1RegisterConfiguration.restart)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::RESTART_DEVICE_ENABLE);

    if (mode1RegisterConfiguration.externalClock)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::EXTERNAL_CLOCK_ENABLE);

    if (mode1RegisterConfiguration.autoIncrement)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::AUTO_INCREMENT_ENABLE);

    if (mode1RegisterConfiguration.sleepMode)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::SLEEP_ENABLE);

    if (mode1RegisterConfiguration.sub1)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::SUB1_RESPOND_ENABLE);

    if (mode1RegisterConfiguration.sub2)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::SUB2_RESPOND_ENABLE);

    if (mode1RegisterConfiguration.sub3)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::SUB3_RESPOND_ENABLE);

    if (mode1RegisterConfiguration.allCall)
        mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::ALLCALL_RESPOND_ENABLE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), mode1RegisterByte);

}

void PCA9685::initMode2Register() {

    if(mode2RegisterConfiguration.outputInvert)
        mode2RegisterByte = mode2RegisterByte | static_cast<uint8_t>(Mode2RegisterMasks::OUTPUT_INVERT_ENABLE);

    if(not mode2RegisterConfiguration.outputChangesOnStop)
        mode2RegisterByte = mode2RegisterByte | static_cast<uint8_t>(Mode2RegisterMasks::OUTPUT_CHANGES_ON_ACK);

    if(not mode2RegisterConfiguration.outputConfigurationOpenDrain)
        mode2RegisterByte = mode2RegisterByte | static_cast<uint8_t>(Mode2RegisterMasks::OUTPUT_CONFIGURATION_TOTEM_POLE);

    if(not mode2RegisterConfiguration.oePinHighState00 && mode2RegisterConfiguration.oePinHighState01 && not mode2RegisterConfiguration.oePinHighState1x)
        mode2RegisterByte = mode2RegisterByte | static_cast<uint8_t>(Mode2RegisterMasks::OUTPUT_ENABLE_STATE_HIGH);

    if(not mode2RegisterConfiguration.oePinHighState00 && not mode2RegisterConfiguration.oePinHighState01 && mode2RegisterConfiguration.oePinHighState1x)
        mode2RegisterByte = mode2RegisterByte | static_cast<uint8_t>(Mode2RegisterMasks::OUTPUT_ENABLE_STATE_HIGH_IMPEDANCE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE2), mode2RegisterByte);

}

void PCA9685::setPWMChannel(PWMChannels channel, uint8_t dutyCyclePercent, uint8_t delayPercent) {

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

    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * static_cast<uint8_t>(channel);

    constexpr size_t I2CTransmittedDataSize = NumberOfBytesPerPWMChannelRegisters + 1;

    etl::array<uint8_t, I2CTransmittedDataSize> i2cTransmittedData = {LEDn_ON_L,
                                                                      static_cast<uint8_t>(pwmTurnHighAtStepLSB),
                                                                      static_cast<uint8_t>(pwmTurnHighAtStepMSB),
                                                                      static_cast<uint8_t>(pwmTurnLowAtStepLSB),
                                                                      static_cast<uint8_t>(pwmTurnLowAtStepMSB)};

    enableAutoIncrement();
    i2cWriteData(i2cTransmittedData.data(), static_cast<uint8_t>(sizeof i2cTransmittedData));
    disableAutoIncrement();

}

void PCA9685::setPWMChannelAlwaysOff(PWMChannels channel) {
    setPWMChannel(channel, 0);
}

void PCA9685::setPWMChannelAlwaysOn(PWMChannels channel, uint8_t delayPercent) {
    setPWMChannel(channel, 100, delayPercent);
}

void PCA9685::setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent) {

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
        /// Note: If ALL_LED_ON_H[4] and ALL_LED_OFF_H[4] are set at the same time, the ALL_LED_OFF_H[4] function takes precedence.
        pwmTurnLowAtStepMSB |= MSBRegisterBit4;
    } else if (dutyCyclePercent > 99) {
        pwmTurnHighAtStepMSB |= MSBRegisterBit4;
        pwmTurnLowAtStepMSB = 0;
    }

    constexpr size_t I2CTransmittedDataSize = NumberOfBytesPerPWMChannelRegisters + 1;

    etl::array<uint8_t, I2CTransmittedDataSize> i2cTransmittedData = {
            static_cast<uint8_t>(RegisterAddresses::ALL_LED_ON_L),
            static_cast<uint8_t>(pwmTurnHighAtStepLSB),
            static_cast<uint8_t>(pwmTurnHighAtStepMSB),
            static_cast<uint8_t>(pwmTurnLowAtStepLSB),
            static_cast<uint8_t>(pwmTurnLowAtStepMSB)};

    enableAutoIncrement();
    i2cWriteData(i2cTransmittedData.data(), static_cast<uint8_t>(sizeof i2cTransmittedData));
    disableAutoIncrement();

}

void PCA9685::setAllPWMChannelsOff() {

    constexpr uint8_t I2CTransmittedByte = 0x10;

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::ALL_LED_OFF_H), I2CTransmittedByte);

}

void PCA9685::setAllPWMChannelsOn(uint8_t delayPercent) {
    setAllPWMChannels(100, delayPercent);
}

void PCA9685::enableAutoIncrement() {
    mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::AUTO_INCREMENT_ENABLE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), mode1RegisterByte);
}

void PCA9685::disableAutoIncrement() {
    mode1RegisterByte = mode1RegisterByte & static_cast<uint8_t>(Mode1RegisterMasks::AUTO_INCREMENT_DISABLE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), mode1RegisterByte);
}

void PCA9685::enableExternalClock() {
    mode1RegisterByte = mode1RegisterByte | static_cast<uint8_t>(Mode1RegisterMasks::EXTERNAL_CLOCK_ENABLE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), mode1RegisterByte);
}

void PCA9685::disableExternalClock() {
    mode1RegisterByte = mode1RegisterByte & static_cast<uint8_t>(Mode1RegisterMasks::EXTERNAL_CLOCK_DISABLE);

    i2cWriteToSpecificRegister(static_cast<uint8_t>(RegisterAddresses::MODE1), mode1RegisterByte);
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

