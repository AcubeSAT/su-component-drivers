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
    etl::array<RegisterAddress_t, BytesToWrite> bufferWrite {static_cast<RegisterAddress_t>(registerAddress)};

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

template<uint8_t NUMBER_OF_BYTES, typename T>
T INA228::decodeReturnedData(const etl::array<uint8_t, NUMBER_OF_BYTES> &returnedData) const {
    static_assert(std::is_same_v<T, uint64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint8_t>, "Invalid template argument");

    if (NUMBER_OF_BYTES != returnedData.size()) {
        LOG_ERROR << "Error in decoding function";
        // TODO: Error-handling
    }

    T convertedBinary = 0;
    constexpr uint8_t ShiftStep = 8 * (NUMBER_OF_BYTES - 1);

    for (uint8_t i = 0; i < NUMBER_OF_BYTES; i++) {
        convertedBinary |= static_cast<T>(static_cast<T>(returnedData[i]) << (ShiftStep - 8*i));
    }

    return convertedBinary;
}

float INA228::getCurrent() const {
    const uint8_t CurrentRegisterBytes = 3;
    auto returnedData = readRegister<CurrentRegisterBytes>(RegisterAddress::CURRENT);

    if (returnedData.data() == nullptr){
        LOG_ERROR << "Current monitor failed to perform I2C transaction: returned data is nullptr";
        // TODO: Error handling
    }

    const auto Current = [=]() -> float {
        auto current = decodeReturnedData<CurrentRegisterBytes, uint32_t>(returnedData);
        current = (current >> 4) & static_cast<decltype(current)>(0xFFFFF);

        const uint32_t Sign = current & static_cast<decltype(current)>(0x80000);

        if (Sign != 0) {
            current = (~current & static_cast<decltype(current)>(0xFFFFF)) + 1;
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

    auto power = decodeReturnedData<PowerRegisterBytes, uint32_t>(returnedData);

    const float Resolution = 3.2f * CurrentLSB;

    return Resolution * static_cast<float>(power);
}

float INA228::getVoltage() const {
    const uint8_t VBusRegisterBytes = 3;
    auto returnedData = readRegister<VBusRegisterBytes>(RegisterAddress::VBUS);

    auto busVoltage = decodeReturnedData<VBusRegisterBytes, uint32_t>(returnedData);

    busVoltage = (busVoltage >> 4) & static_cast<decltype(busVoltage)>(0xFFFFF);

    constexpr float ResolutionSize = 0.0001953125f;

    return static_cast<float>(busVoltage) * ResolutionSize;
}

float INA228::getDieTemperature() const {
    const uint8_t DieTempRegisterBytes = 2;
    auto returnedData = readRegister<DieTempRegisterBytes>(RegisterAddress::DIETEMP);

    const auto InternalTemperature = [=]() -> float {
        auto internalTemperature = decodeReturnedData<DieTempRegisterBytes, uint16_t>(returnedData);

        const uint32_t Sign = internalTemperature & static_cast<decltype(internalTemperature)>(0x8000);

        if (Sign != 0) {
            internalTemperature = (~internalTemperature & static_cast<decltype(internalTemperature)>(0xFFFFF)) + 1;
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

    auto energy = decodeReturnedData<EnergyRegisterBytes>(returnedData);

    const float Resolution = 16.0f * 3.2f * CurrentLSB;

    return Resolution * static_cast<float>(energy);
}

float INA228::getShuntVoltage() const {
    const uint8_t VShuntRegisterBytes = 3;
    auto returnedData = readRegister<VShuntRegisterBytes>(RegisterAddress::VSHUNT);

    const auto ShuntVoltage = [=]() -> float {
        auto shuntVoltage = decodeReturnedData<VShuntRegisterBytes, uint32_t>(returnedData);

        shuntVoltage = (shuntVoltage >> 4) & static_cast<decltype(shuntVoltage)>(0xFFFFF);

        const uint32_t Sign = shuntVoltage & static_cast<decltype(shuntVoltage)>(0x80000);

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
