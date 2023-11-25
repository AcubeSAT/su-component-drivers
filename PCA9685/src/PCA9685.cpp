#include "PCA9685.hpp"

bool PCA9685::i2cReadData(RegisterAddress registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus is busy";
            continue;
        }

        if (not PCA9685_TWIHS_WriteRead(static_cast<std::underlying_type_t<I2CAddress>>(i2cAddress),
                                        reinterpret_cast<uint8_t *>(&registerAddress), 1, rData, numberOfBytesToRead)) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
            return false;
        }

        return true;
    }

}

bool PCA9685::i2cWriteData(uint8_t* tData, uint8_t numberOfBytesToWrite) {

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus is busy";
            continue;
        }

        if (not PCA9685_TWIHS_Write(static_cast<std::underlying_type_t<I2CAddress>>(i2cAddress), tData, numberOfBytesToWrite)) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
            return false;
        }

        return true;
    }

}

void PCA9685::i2cWriteValueToRegister(RegisterAddress registerAddress, uint8_t transmittedByte) {
    uint8_t tData[2] = {static_cast<std::underlying_type_t<RegisterAddress>>(registerAddress), transmittedByte};
    i2cWriteData(&tData[0], sizeof(tData));
}

void PCA9685::setPWMChannel(PWMChannels channel, uint8_t dutyCyclePercent, uint8_t delayPercent) {
    const auto addressOfFirstChannel = static_cast<std::underlying_type_t<RegisterAddress>>(RegisterAddress::LED0_ON_L);

    const uint8_t LEDn_ON_L = addressOfFirstChannel + BytesPerPWM * static_cast<std::underlying_type_t<PWMChannels>>(channel);

    constexpr size_t I2CTransmittedDataSize = BytesPerPWM + 1;

    auto i2cTransmittedData = calculatePWMRegisterValues<I2CTransmittedDataSize>(dutyCyclePercent, delayPercent);
    i2cTransmittedData[0] = LEDn_ON_L;

    enableAutoIncrement();
    i2cWriteData(i2cTransmittedData.data(), I2CTransmittedDataSize);
    disableAutoIncrement();
}

void PCA9685::setPWMChannelAlwaysOff(PWMChannels channel) {
    setPWMChannel(channel, FullOffPWMDutyCycle);
}

void PCA9685::setPWMChannelAlwaysOn(PWMChannels channel, uint8_t delayPercent) {
    setPWMChannel(channel, FullOnPWMDutyCycle, delayPercent);
}

void PCA9685::setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent) {
    constexpr size_t I2CTransmittedDataSize = BytesPerPWM + 1;

    auto i2cTransmittedData = calculatePWMRegisterValues<I2CTransmittedDataSize>(dutyCyclePercent, delayPercent);
    i2cTransmittedData[0] = static_cast<std::underlying_type_t<RegisterAddress>>(RegisterAddress::ALL_LED_ON_L);

    enableAutoIncrement();
    i2cWriteData(i2cTransmittedData.data(), I2CTransmittedDataSize);
    disableAutoIncrement();
}

void PCA9685::setAllPWMChannelsOff() {
    constexpr uint8_t I2CTransmittedByte = 0x10;
    i2cWriteValueToRegister(RegisterAddress::ALL_LED_OFF_H, I2CTransmittedByte);
}

void PCA9685::setAllPWMChannelsOn(uint8_t delayPercent) {
    setAllPWMChannels(FullOnPWMDutyCycle, delayPercent);
}

template<uint8_t numOfBytes>
etl::array<uint8_t, numOfBytes>
PCA9685::calculatePWMRegisterValues(uint8_t dutyCyclePercent, uint8_t delayPercent) {
    const auto dutyCycleStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                            (static_cast<float>(dutyCyclePercent) / 100.0f));
    const auto delayStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                        (static_cast<float>(delayPercent) / 100.0f));

    const uint16_t pwmTurnHighAtStepLSB = delayStepsNumber & MaskLSB;
    uint16_t pwmTurnHighAtStepMSB = delayStepsNumber & MaskMSB;

    uint16_t turnLowAtStep = delayStepsNumber + dutyCycleStepsNumber - 1;

    if (delayPercent + dutyCyclePercent > 99) {
        turnLowAtStep -= GrayscaleMaximumSteps;
    }

    const uint16_t pwmTurnLowAtStepLSB = turnLowAtStep & MaskLSB;
    uint16_t pwmTurnLowAtStepMSB = turnLowAtStep & MaskMSB;

    constexpr uint8_t MSBRegisterBit4 = 0x10;

    if (dutyCyclePercent == 0) {
        /// Note: If LEDn_ON_H[4] and LEDn_OFF_H[4] are set at the same time, the LEDn_OFF_H[4] function takes precedence.
        /// Note: If ALL_LED_ON_H[4] and ALL_LED_OFF_H[4] are set at the same time, the ALL_LED_OFF_H[4] function takes precedence.
        pwmTurnLowAtStepMSB |= MSBRegisterBit4;
    } else if (dutyCyclePercent > 99) {
        pwmTurnHighAtStepMSB |= MSBRegisterBit4;
        pwmTurnLowAtStepMSB = 0;
    }

    return etl::array<uint8_t, numOfBytes>{0,
                                           static_cast<uint8_t>(pwmTurnHighAtStepLSB),
                                           static_cast<uint8_t>(pwmTurnHighAtStepMSB),
                                           static_cast<uint8_t>(pwmTurnLowAtStepLSB),
                                           static_cast<uint8_t>(pwmTurnLowAtStepMSB)};
}

void PCA9685::enableAutoIncrement() {
    mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::AUTO_INCREMENT_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::disableAutoIncrement() {
    mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::AUTO_INCREMENT_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::enableExternalClock() {
    mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::EXTERNAL_CLOCK_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::disableExternalClock() {
    mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::EXTERNAL_CLOCK_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::setDeviceToSleepMode() {
    mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SLEEP_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::stopDeviceFromSleepMode() {
    mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SLEEP_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::enableDeviceResponseToSubAddresses(bool sub1, bool sub2, bool sub3, bool allCall) {
    if (sub1)
        mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB1_RESPOND_ENABLE);

    if (sub2)
        mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB2_RESPOND_ENABLE);

    if (sub3)
        mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB3_RESPOND_ENABLE);

    if (allCall)
        mode1RegisterByte |= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::ALLCALL_RESPOND_ENABLE);

    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::disableDeviceResponseToSubAddresses(bool sub1, bool sub2, bool sub3, bool allCall) {
    if (sub1)
        mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB1_RESPOND_DISABLE);

    if (sub2)
        mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB2_RESPOND_DISABLE);

    if (sub3)
        mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::SUB3_RESPOND_DISABLE);

    if (allCall)
        mode1RegisterByte &= static_cast<std::underlying_type_t<Mode1RegisterMasks>>(Mode1RegisterMasks::ALLCALL_RESPOND_DISABLE);

    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::invertOutputs(bool invert) {
    if (invert)
        mode2RegisterByte |= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_INVERT_ENABLE);
    else
        mode2RegisterByte &= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_INVERT_DISABLE);

    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setOutputChangeOn(bool stop) {
    if (not stop)
        mode2RegisterByte |= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_CHANGES_ON_ACK);
    else
        mode2RegisterByte &= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_CHANGES_ON_STOP);

    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setTotemPoleOutputs() {
    mode2RegisterByte |= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_CONFIGURATION_TOTEM_POLE);
    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setOpenDrainOutputs() {
    mode2RegisterByte &= static_cast<std::underlying_type_t<Mode2RegisterMasks>>(Mode2RegisterMasks::OUTPUT_CONFIGURATION_OPEN_DRAIN);
    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setOutputEnableState(bool outne00, bool outne01, bool outne1x) {

}

void PCA9685::softwareResetOfAllDevices() {

}

