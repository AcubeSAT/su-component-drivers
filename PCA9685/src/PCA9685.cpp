#include "PCA9685.hpp"

template<uint8_t BUFFER_SIZE, typename T>
etl::expected<etl::array<T, BUFFER_SIZE>, bool> PCA9685::i2cReadData() {
    etl::array<T, BUFFER_SIZE> buffer{0};

    if (not PCA9685_TWIHS_Read(static_cast<I2CAddress_t>(i2cAddress), buffer.data(), buffer.size())) {
        LOG_INFO << "PCA9685 was not able to perform any transaction";
        return etl::unexpected<bool>(true);
    }

    while (TWIHS2_IsBusy());

    if (PCA9685_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
    }

    return buffer;
}

template<typename T>
bool PCA9685::i2cWriteData(etl::span<uint8_t> buffer) {
    auto slaveAddress = static_cast<I2CAddress_t>(i2cAddress);

    if constexpr (std::is_same_v<T, SoftwareReset>) {
        slaveAddress = static_cast<SoftwareReset_t>(SoftwareReset::SLAVE_ADDRESS);
    }

    if (not PCA9685_TWIHS_Write(slaveAddress, buffer.data(), buffer.size())) {
        LOG_INFO << "PCA9685 was not able to perform any transaction";
        return false;
    }

    while (PCA9685_TWIHS_IsBusy());

    if (PCA9685_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_INFO << "PCA9685 was not able to perform any transaction: I2C bus NAK";
        return false;
    }

    return true;
}

void PCA9685::i2cWriteValueToRegister(RegisterAddress registerAddress, uint8_t transmittedByte) {
    constexpr uint8_t BufferSize = 2;
    etl::array<uint8_t, BufferSize> buffer = {static_cast<RegisterAddress_t>(registerAddress), transmittedByte};
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
        uint16_t calculatedTurnHighAtStepMSB = (delayStepsNumber & MaskMSB) >> 8;
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

void PCA9685::setPWMChannel(PWMChannel channel, uint8_t dutyCyclePercent, uint8_t delayPercent) {
    const auto addressOfFirstChannel = static_cast<RegisterAddress_t>(RegisterAddress::LED0_ON_L);

    const uint8_t LEDn_ON_L = addressOfFirstChannel + BytesPerPWM * static_cast<PWMChannel_t>(channel);

    auto i2cTransmittedData = calculatePWMRegisterValues(dutyCyclePercent, delayPercent);
    i2cTransmittedData[0] = LEDn_ON_L;

    enableAutoIncrement();
    i2cWriteData(etl::span<std::remove_reference_t<decltype(i2cTransmittedData[0])>>(i2cTransmittedData));
    disableAutoIncrement();
}

void PCA9685::setPWMChannelAlwaysOff(PWMChannel channel) {
    setPWMChannel(channel, FullOffPWMDutyCycle);
}

void PCA9685::setPWMChannelAlwaysOn(PWMChannel channel, uint8_t delayPercent) {
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

void PCA9685::enterSleepMode() {
    mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SLEEP_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::exitSleepMode() {
    mode1RegisterByte &= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::SLEEP_DISABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
    vTaskDelay(pdMS_TO_TICKS(SleepModeDelayMS));
    mode1RegisterByte |= static_cast<Mode1RegisterMasks_t>(Mode1RegisterMasks::RESTART_DEVICE_ENABLE);
    i2cWriteValueToRegister(RegisterAddress::MODE1, mode1RegisterByte);
}

void PCA9685::reset() {
    i2cWriteData<SoftwareReset>(
            etl::span<uint8_t>(etl::array<uint8_t, 1>{static_cast<SoftwareReset_t>(SoftwareReset::DATA_BYTE_1)}));
}

void PCA9685::setPWMFrequency(float frequency) {
    constexpr float ConvertToMHz = 1e6f;

    const auto MinimumRefreshRate = [=]() -> float {
        if (deviceClock == PCA9685Configuration::DeviceClock::EXTERNAL_CLOCK)
            return ExternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / float{MaximumPreScaleValue + 1};

        return InternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / float{MaximumPreScaleValue + 1};
    }();

    const auto MaximumRefreshRate = [=]() -> float {
        if (deviceClock == PCA9685Configuration::DeviceClock::EXTERNAL_CLOCK)
            return InternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / float{MinimumPreScaleValue + 1};

        return InternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / float{MinimumPreScaleValue + 1};
    }();

    if (frequency > MaximumRefreshRate || frequency < MinimumRefreshRate) {
        LOG_INFO << "PCA9685 do not support the specified PWM frequency... Frequency was not updated";
        return;
    }

    const auto PreScaleValue = [=]() -> float {
        if (deviceClock == PCA9685Configuration::DeviceClock::EXTERNAL_CLOCK) {
            return round(ExternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / frequency) - 1;
        }
        return round(InternalOscillatorFrequency * ConvertToMHz / GrayscaleMaximumSteps / frequency) - 1;
    }();

    enterSleepMode();
    i2cWriteValueToRegister(RegisterAddress::PRE_SCALE, static_cast<uint8_t>(PreScaleValue));
    exitSleepMode();
}

void PCA9685::start(const PCA9685Configuration::Configuration &config) {
    deviceClock = config.deviceClock;
    setOutputDriveType(config.outputDriveType);
    setOutputLogicState(config.outputLogicState);
}
