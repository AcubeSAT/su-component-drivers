#include "INA228.hpp"

INA228::INA228(INA228::I2CAddress i2cAddress, INA228::Configuration configuration,
               INA228::ADCConfiguration adcConfiguration) : I2CChipAddress(i2cAddress) {

    setConfig(configuration);
    setADCConfig(adcConfiguration);
    setShuntCalRegister(configuration);
}

void INA228::setConfig(INA228::Configuration configuration) {
    constexpr uint8_t ConfigRegisterBytesSize = 3;
    etl::array<uint8_t, ConfigRegisterBytesSize> buffer{static_cast<uint8_t>(RegisterAddress::CONFIG),
                                                        static_cast<uint8_t>(
                                                                (static_cast<uint16_t>(configuration) >> 8) & 0xFF),
                                                        static_cast<uint8_t>(static_cast<uint16_t>(configuration) &
                                                                             0xFF)};

    writeRegister(buffer);
}

void INA228::setADCConfig(INA228::ADCConfiguration adcConfiguration) {
    constexpr uint8_t ADCConfigRegisterBytesSize = 3;
    etl::array<uint8_t, ADCConfigRegisterBytesSize> buffer{static_cast<uint8_t>(RegisterAddress::ADC_CONFIG),
                                                           static_cast<uint8_t>(
                                                                   (static_cast<uint16_t>(adcConfiguration) >> 8) &
                                                                   0xFF), static_cast<uint8_t>(
                                                                   static_cast<uint16_t>(adcConfiguration) & 0xFF)};
    writeRegister(buffer);
}

void INA228::setShuntCalRegister(INA228::Configuration configuration) {
    // Determine whether ADCRANGE is 0 or 1
    adcRangeValue = static_cast<uint16_t>(configuration) & 0b10000;

    if (adcRangeValue != 0) {
        shuntCalValue = shuntCalValue * 4;
    }

    constexpr uint8_t ShuntCalRegisterBytesSize = 3;
    etl::array<uint8_t, ShuntCalRegisterBytesSize> buffer{static_cast<uint8_t>(RegisterAddress::SHUNT_CAL),
                                                          static_cast<uint8_t>((shuntCalValue >> 8) & 0xFF),
                                                          static_cast<uint8_t>((shuntCalValue) & 0xFF)};

    writeRegister(buffer);
}

template<uint8_t RETURNED_BYTES>
etl::array<uint8_t, RETURNED_BYTES> INA228::readRegister(INA228::RegisterAddress registerAddress) const {
    etl::array<uint8_t, RETURNED_BYTES> bufferRead {0};

    constexpr uint8_t BytesToWrite = 1;
    etl::array<uint8_t, BytesToWrite> bufferWrite {static_cast<uint8_t>(registerAddress)};

    if (not INA228_TWIHS_WriteRead(static_cast<uint8_t>(I2CChipAddress), bufferWrite.data(), bufferWrite.size(), bufferRead.data(), RETURNED_BYTES)) {
        LOG_INFO << "Current monitor failed to perform I2C transaction: bus is busy";
    }

    while (INA228_TWIHS_IsBusy());

    if (INA228_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Current monitor failed to perform I2C transaction: device NACK";
    }

    return bufferRead;
}

bool INA228::writeRegister(etl::span<uint8_t> data) const {
    if (not INA228_TWIHS_Write(static_cast<uint8_t>(I2CChipAddress), data.data(), data.size())) {
        LOG_INFO << "Current monitor failed to perform I2C transaction: bus is busy";
        return false;
    }

    while (INA228_TWIHS_IsBusy());

    if (INA228_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Current monitor failed to perform I2C transaction: device NACK";
        return false;
    }

    return true;
}

float INA228::getCurrent() const {
    const uint8_t CurrentRegisterBytes = 3;
    auto returnedData = readRegister<CurrentRegisterBytes>(RegisterAddress::CURRENT);

    if (returnedData.data() == nullptr){
        LOG_ERROR << "Current monitor failed to perform I2C transaction: returned data is nullptr";
        // Error handling
    }

    const auto current = [=]() -> uint32_t {
        uint32_t current = static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000) | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00) | static_cast<uint32_t>(returnedData[2] & 0xFF);
        current = (current >> 4) & 0xFFFFF;

        const uint32_t sign = current & 0x80000;

        if (sign != 0) {
            current = (~current & 0xFFFFF) + 1;
        }

        return current;
    }();

    return static_cast<float>(current) * CurrentLSB;
}

float INA228::getPower() const {
    const uint8_t PowerRegisterBytes = 3;
    auto returnedData = readRegister<PowerRegisterBytes>(RegisterAddress::POWER);

    auto power = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                       | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                       | static_cast<uint32_t>(returnedData[2] & 0xFF));

    return 3.2f * CurrentLSB * static_cast<float>(power);
}

float INA228::getVoltage() const {
    const uint8_t VBusRegisterBytes = 3;
    auto returnedData = readRegister<VBusRegisterBytes>(RegisterAddress::VBUS);

    auto busVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                            | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                            | static_cast<uint32_t>(returnedData[2] & 0xFF));

    busVoltage = (busVoltage >> 4) & 0xFFFFF;

    constexpr float ResolutionSize = 0.0001953125f; // in volts

    return static_cast<float>(busVoltage) * ResolutionSize;
}

float INA228::getDieTemperature() const {
    const uint8_t DieTempRegisterBytes = 2;
    auto returnedData = readRegister<DieTempRegisterBytes>(RegisterAddress::DIETEMP);

    const auto internalTemperature = [=]() -> uint16_t {
        auto internalTemperature = static_cast<uint16_t>(static_cast<uint16_t>((static_cast<uint16_t>(returnedData[0]) << 8) & 0xFF00) | static_cast<uint16_t>(returnedData[1] & 0xFF));

        const uint32_t sign = internalTemperature & 0x8000;

        if (sign != 0) {
            internalTemperature = (~internalTemperature & 0xFFFFF) + 1;
        }

        return internalTemperature;
    }();

    constexpr float ResolutionSize = 0.0078125f;

    return static_cast<float>(internalTemperature) * ResolutionSize;
}

float INA228::getEnergy() const {
    const uint8_t EnergyRegisterBytes = 5;
    auto returnedData = readRegister<EnergyRegisterBytes>(RegisterAddress::ENERGY);

    auto energy = static_cast<uint64_t>(((static_cast<uint64_t>(returnedData[0]) << 32) & 0xFF00000000) |
                                        ((static_cast<uint64_t>(returnedData[1]) << 24) & 0xFF000000) |
                                        ((static_cast<uint64_t>(returnedData[2]) << 16) & 0xFF0000) |
                                        ((static_cast<uint64_t>(returnedData[3]) << 8) & 0xFF00) |
                                        ((static_cast<uint64_t>(returnedData[4])) & 0xFF));

    return 16.0f * 3.2f * CurrentLSB * static_cast<float>(energy);
}

float INA228::getShuntVoltage() const {
    const uint8_t VShuntRegisterBytes = 3;
    auto returnedData = readRegister<VShuntRegisterBytes>(RegisterAddress::VSHUNT);

    const auto shuntVoltage = [=]() -> uint32_t {
        auto shuntVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                  | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                                  | static_cast<uint32_t>(returnedData[2] & 0xFF));

        shuntVoltage = (shuntVoltage >> 4) & 0xFFFFF;

        const uint32_t sign = shuntVoltage & 0x80000;

        if (sign != 0) {
            shuntVoltage = (~shuntVoltage & 0xFFFFF) + 1;
        }

        return shuntVoltage;
    }();

    const float ResolutionSize = (adcRangeValue == 0) ? 0.0003125f : 0.000078125f;

    return static_cast<float>(shuntVoltage) * ResolutionSize;
}
