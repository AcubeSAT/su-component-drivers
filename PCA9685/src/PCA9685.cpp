#include "PCA9685.hpp"

template<size_t SIZE, typename T>
etl::expected<etl::array<T, SIZE>, bool> PCA9685::i2cReadData() {
    etl::array<T, SIZE> buffer{0};

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus is busy";
            continue;
        }

        if (not PCA9685_TWIHS_Read(static_cast<I2CAddress_t>(i2cAddress), buffer.data(), buffer.size())) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
            return etl::unexpected<bool>(true);
        }

        vTaskDelay(pdMS_TO_TICKS(50));

        return buffer;
    }

}

template<typename T>
bool PCA9685::i2cWriteData(etl::span<uint8_t> buffer) {
    auto slaveAddress = static_cast<I2CAddress_t>(i2cAddress);

    if constexpr (std::is_same_v<T, SoftwareReset>) {
        slaveAddress = static_cast<SoftwareReset_t>(SoftwareReset::SLAVE_ADDRESS);
    }

    while (true) {
        if (TWIHS2_IsBusy()) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus is busy";
            continue;
        }

        if (not PCA9685_TWIHS_Write(slaveAddress, buffer.data(), buffer.size())) {
            LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
            return false;
        }

        vTaskDelay(pdMS_TO_TICKS(50));

        return true;
    }

}

void PCA9685::i2cWriteValueToRegister(RegisterAddress registerAddress, uint8_t transmittedByte) {
    etl::array<uint8_t, 2> buffer = {static_cast<RegisterAddress_t>(registerAddress), transmittedByte};
    i2cWriteData(etl::span<std::remove_reference_t<decltype(buffer[0])>>(buffer));
}

auto PCA9685::calculatePWMRegisterValues(uint8_t dutyCyclePercent, uint8_t delayPercent) {
    const auto dutyCycleStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                            (static_cast<float>(dutyCyclePercent) / 100.0f));
    const auto delayStepsNumber = static_cast<uint16_t>(static_cast<float>(GrayscaleMaximumSteps) *
                                                        (static_cast<float>(delayPercent) / 100.0f));

    constexpr uint8_t MSBRegisterBit4 = 0x10;

    const uint16_t pwmTurnHighAtStepLSB = delayStepsNumber & MaskLSB;

    const auto pwmTurnHighAtStepMSB = [=]() -> uint16_t {
        uint16_t calculatedTurnHighAtStepMSB = (delayStepsNumber & MaskLSB) >> 8;
        if (dutyCyclePercent > 99)
            return calculatedTurnHighAtStepMSB | MSBRegisterBit4;
        return calculatedTurnHighAtStepMSB;
    }();

    const auto turnLowAtStep = [=]() -> uint16_t {
        uint16_t calculatedTurnLowAtStep = delayStepsNumber + dutyCycleStepsNumber - 1;
        if (delayPercent + dutyCyclePercent > 99)
            return calculatedTurnLowAtStep - GrayscaleMaximumSteps;
        return calculatedTurnLowAtStep;
    }();

    const uint16_t pwmTurnLowAtStepLSB = turnLowAtStep & MaskLSB;

    const auto pwmTurnLowAtStepMSB = [=]() -> uint16_t {
        uint16_t calculatedTurnLowAtStepMSB = (turnLowAtStep & MaskMSB) >> 8;
        /// Note: If LEDn_ON_H[4] and LEDn_OFF_H[4] are set at the same time, the LEDn_OFF_H[4] function takes precedence.
        /// Note: If ALL_LED_ON_H[4] and ALL_LED_OFF_H[4] are set at the same time, the ALL_LED_OFF_H[4] function takes precedence.
        if (dutyCyclePercent == 0)
            return calculatedTurnLowAtStepMSB | MSBRegisterBit4;
        else if (dutyCyclePercent > 99)
            return 0;
        return calculatedTurnLowAtStepMSB;
    }();

    return etl::array<uint8_t, BytesPerPWM + 1>{0,
                                                static_cast<uint8_t>(pwmTurnHighAtStepLSB),
                                                static_cast<uint8_t>(pwmTurnHighAtStepMSB),
                                                static_cast<uint8_t>(pwmTurnLowAtStepLSB),
                                                static_cast<uint8_t>(pwmTurnLowAtStepMSB)};
}

void PCA9685::setPWMChannel(PWMChannels channel, uint8_t dutyCyclePercent, uint8_t delayPercent) {
    const auto addressOfFirstChannel = static_cast<RegisterAddress_t>(RegisterAddress::LED0_ON_L);

    const uint8_t LEDn_ON_L = addressOfFirstChannel + BytesPerPWM * static_cast<PWMChannels_t>(channel);

    auto i2cTransmittedData = calculatePWMRegisterValues(dutyCyclePercent, delayPercent);
    i2cTransmittedData[0] = LEDn_ON_L;

    enableAutoIncrement();
    i2cWriteData(etl::span<uint8_t>(i2cTransmittedData));
    disableAutoIncrement();
}

void PCA9685::setPWMChannelAlwaysOff(PWMChannels channel) {
    setPWMChannel(channel, FullOffPWMDutyCycle);
}

void PCA9685::setPWMChannelAlwaysOn(PWMChannels channel, uint8_t delayPercent) {
    setPWMChannel(channel, FullOnPWMDutyCycle, delayPercent);
}

void PCA9685::setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent) {
    auto i2cTransmittedData = calculatePWMRegisterValues(dutyCyclePercent, delayPercent);
    i2cTransmittedData[0] = static_cast<RegisterAddress_t>(RegisterAddress::ALL_LED_ON_L);

    enableAutoIncrement();
    i2cWriteData(etl::span<uint8_t>(i2cTransmittedData));
    disableAutoIncrement();
}

void PCA9685::setAllPWMChannelsOff() {
    constexpr uint8_t I2CTransmittedByte = 0x10;
    i2cWriteValueToRegister(RegisterAddress::ALL_LED_OFF_H, I2CTransmittedByte);
}

void PCA9685::setAllPWMChannelsOn(uint8_t delayPercent) {
    setAllPWMChannels(FullOnPWMDutyCycle, delayPercent);
}

void PCA9685::enableAutoIncrement() {
    mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::AUTO_INCREMENT_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::disableAutoIncrement() {
    mode1RegisterByte &= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::AUTO_INCREMENT_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::disableExternalClock() {
    mode1RegisterByte &= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::EXTERNAL_CLOCK_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::sendToSleep() {
    mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SLEEP_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::recoverFromSleep() {
    mode1RegisterByte &= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SLEEP_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::setI2CBusSubAddresses(bool sub1, bool sub2, bool sub3, bool allCall) {
    if (sub1)
        mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SUB1_RESPOND_ENABLE);

    if (sub2)
        mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SUB2_RESPOND_ENABLE);

    if (sub3)
        mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SUB3_RESPOND_ENABLE);

    if (allCall)
        mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::ALLCALL_RESPOND_ENABLE);

    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::invertOutputs(bool invert) {
    if (invert)
        mode2RegisterByte |= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_INVERT_ENABLE);
    else
        mode2RegisterByte &= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_INVERT_DISABLE);

    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setOutputChangeOn(bool stop) {
    if (not stop)
        mode2RegisterByte |= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_CHANGES_ON_ACK);
    else
        mode2RegisterByte &= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_CHANGES_ON_STOP);

    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setTotemPoleOutputs() {
    mode2RegisterByte |= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_CONFIGURATION_TOTEM_POLE);
    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::setOpenDrainOutputs() {
    mode2RegisterByte &= static_cast<Mode2RegisterMasks_t>(Mode2RegisterMasks::OUTPUT_CONFIGURATION_OPEN_DRAIN);
    i2cWriteValueToRegister(RegisterAddress::MODE2, mode2RegisterByte);
}

void PCA9685::reset() {
    i2cWriteData<SoftwareReset>(etl::span<uint8_t>(etl::array<uint8_t, 1> {static_cast<SoftwareReset_t>(SoftwareReset::DATA_BYTE_1)}));
}
