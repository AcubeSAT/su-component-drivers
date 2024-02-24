#include "INA228.hpp"

void INA228::setupConfigurationRegisters() const {
    auto createDataPacket = [](RegisterAddress registerAddress, uint16_t data) {
        const uint8_t NumberOfBytesToTransmit = sizeof(data)/sizeof(uint8_t) + sizeof(std::underlying_type_t<RegisterAddress>);

        const etl::array<uint8_t, NumberOfBytesToTransmit> Buffer {static_cast<uint8_t>(registerAddress), static_cast<uint8_t>((static_cast<uint16_t>(data)&0xFF00)>>8), static_cast<uint8_t>(data&0xFF)};
        return Buffer;
    };

    auto configRegisterBuffer = createDataPacket(RegisterAddress::CONFIG, static_cast<uint16_t>(ConfigurationSelected));
    auto adcConfigRegisterBuffer = createDataPacket(RegisterAddress::ADC_CONFIG, static_cast<uint16_t>(ADCConfigurationSelected));
    auto shuntCalRegisterBuffer = createDataPacket(RegisterAddress::SHUNT_CAL, ShuntCalValue);

    writeRegister(configRegisterBuffer);
    writeRegister(adcConfigRegisterBuffer);
    writeRegister(shuntCalRegisterBuffer);
}

template<uint8_t RETURNED_BYTES>
etl::array<uint8_t, RETURNED_BYTES> INA228::readRegister(INA228::RegisterAddress registerAddress) const {
    etl::array<uint8_t, RETURNED_BYTES> bufferRead {0};

    constexpr uint8_t BytesToWrite = 1;
    etl::array<uint8_t, BytesToWrite> bufferWrite {static_cast<uint8_t>(registerAddress)};

    if (not INA228_TWIHS_WriteRead(static_cast<uint8_t>(I2CChipAddress), bufferWrite.data(), bufferWrite.size(), bufferRead.data(), RETURNED_BYTES)) {
        LOG_INFO << "Current monitor failed to perform I2C transaction: bus is busy";
        // TODO: Error handling
    }

    while (INA228_TWIHS_IsBusy());

    if (INA228_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Current monitor failed to perform I2C transaction: device NACK";
        // TODO: Error handling
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
        // TODO: Error handling
    }

    const auto Current = [=]() -> float {
        uint32_t current = static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000) | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00) | static_cast<uint32_t>(returnedData[2] & 0xFF);
        current = (current >> 4) & 0xFFFFF;

        const uint32_t Sign = current & 0x80000;

        if (Sign != 0) {
            current = (~current & 0xFFFFF) + 1;
            const auto CurrentFloat = - static_cast<float>(current);
            return CurrentFloat;
        }

        return static_cast<float>(current);
    }();

    return Current * CurrentLSB;
}

float INA228::getPower() const {
    const uint8_t PowerRegisterBytes = 3;
    auto returnedData = readRegister<PowerRegisterBytes>(RegisterAddress::POWER);

    auto power = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                       | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                       | static_cast<uint32_t>(returnedData[2] & 0xFF));

    const float Resolution = 3.2f * CurrentLSB;

    return Resolution * static_cast<float>(power);
}

float INA228::getVoltage() const {
    const uint8_t VBusRegisterBytes = 3;
    auto returnedData = readRegister<VBusRegisterBytes>(RegisterAddress::VBUS);

    auto busVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                            | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                            | static_cast<uint32_t>(returnedData[2] & 0xFF));

    busVoltage = (busVoltage >> 4) & 0xFFFFF;

    constexpr float ResolutionSize = 0.0001953125f;

    return static_cast<float>(busVoltage) * ResolutionSize;
}

float INA228::getDieTemperature() const {
    const uint8_t DieTempRegisterBytes = 2;
    auto returnedData = readRegister<DieTempRegisterBytes>(RegisterAddress::DIETEMP);

    const auto InternalTemperature = [=]() -> float {
        auto internalTemperature = static_cast<uint16_t>(static_cast<uint16_t>((static_cast<uint16_t>(returnedData[0]) << 8) & 0xFF00) | static_cast<uint16_t>(returnedData[1] & 0xFF));

        const uint32_t Sign = internalTemperature & 0x8000;

        if (Sign != 0) {
            internalTemperature = (~internalTemperature & 0xFFFFF) + 1;
            const auto InternalTemperatureFloat = - static_cast<float>(internalTemperature);
            return InternalTemperatureFloat;
        }

        return static_cast<float>(internalTemperature);
    }();

    constexpr float ResolutionSize = 0.0078125f;

    return InternalTemperature * ResolutionSize;
}

float INA228::getEnergy() const {
    const uint8_t EnergyRegisterBytes = 5;
    auto returnedData = readRegister<EnergyRegisterBytes>(RegisterAddress::ENERGY);

    auto energy = static_cast<uint64_t>(((static_cast<uint64_t>(returnedData[0]) << 32) & 0xFF00000000) |
                                        ((static_cast<uint64_t>(returnedData[1]) << 24) & 0xFF000000) |
                                        ((static_cast<uint64_t>(returnedData[2]) << 16) & 0xFF0000) |
                                        ((static_cast<uint64_t>(returnedData[3]) << 8) & 0xFF00) |
                                        ((static_cast<uint64_t>(returnedData[4])) & 0xFF));

    const float Resolution = 16.0f * 3.2f * CurrentLSB;

    return Resolution * static_cast<float>(energy);
}

float INA228::getShuntVoltage() const {
    const uint8_t VShuntRegisterBytes = 3;
    auto returnedData = readRegister<VShuntRegisterBytes>(RegisterAddress::VSHUNT);

    const auto ShuntVoltage = [=]() -> float {
        auto shuntVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                  | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                                  | static_cast<uint32_t>(returnedData[2] & 0xFF));

        shuntVoltage = (shuntVoltage >> 4) & 0xFFFFF;

        const uint32_t Sign = shuntVoltage & 0x80000;

        if (Sign != 0) {
            shuntVoltage = (~shuntVoltage & 0xFFFFF) + 1;
            auto ShuntVoltageFloat = - static_cast<float>(shuntVoltage);
            return ShuntVoltageFloat;
        }

        return static_cast<float>(shuntVoltage);
    }();

    const auto ResolutionSize = [=]() -> float {

        if (ConfigurationSelected == Configuration::Configuration2) {
            const float ResolutionADCRange1 = 0.000078125f;
            return ResolutionADCRange1;
        }

        const float ResolutionADCRange0 = 0.0003125f;
        return ResolutionADCRange0;
    }();

    return ShuntVoltage * ResolutionSize;
}
