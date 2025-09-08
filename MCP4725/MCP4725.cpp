
#include "MCP4725.hpp"

#include <cmath>
#include <Logger.hpp>


MCP4725::MCP4725(uint8_t i2cAddress, float _maxVoltage) : address{i2cAddress}, maxVoltage {_maxVoltage}, voltageRaw{0} {
    etl::optional<etl::array<uint8_t,5>> data = readRegisters();
    if (!data) {
        LOG_ERROR << "Failed to read DAC voltage, assuming 0";
        return;
    }
    uint16_t eepromVoltageRaw = (((*data)[3] & 0b1111) << 8) | (*data)[4];

    voltageRaw = eepromVoltageRaw;

}


template <bool WriteToEEPROM>
MCP4725::Error MCP4725::setVoltage(float voltage) {
    if (voltage < 0.0f || voltage > maxVoltage) {
        return Error::INVALID_ARGUMENT;
    }

    etl::optional<etl::array<uint8_t,5>> readData = readRegisters();
    if ((!readData.has_value()) || ((*readData)[0] & 0b10000000) == 0) {
        return Error::EEPROM_BUSY;
    }
    uint16_t rawValue = std::round(MaxVoltageRaw * voltage / maxVoltage);


    constexpr uint8_t byte0 = getCommandByte(WriteToEEPROM);
    etl::array<uint8_t, 3> buffer = {byte0, static_cast<uint8_t>(rawValue >> 4), static_cast<uint8_t>((rawValue & 0b1111) << 4)};
    bool success = TWIHS0_Write(address, reinterpret_cast<uint8_t*>(&buffer), 3);
    if (!success) {
        return Error::WRITE_FAIL;
    }
    while (TWIHS0_IsBusy()) {

    }
    voltageRaw = rawValue;
    return Error::NO_ERROR;
}

template MCP4725::Error MCP4725::setVoltage<true>(float voltage);
template MCP4725::Error MCP4725::setVoltage<false>(float voltage);

etl::optional<etl::array<uint8_t, 5>> MCP4725::readRegisters() const {
    etl::array<uint8_t, 5> buffer{};
    bool success = TWIHS0_Read(address, buffer.data(), 5);
    while (TWIHS0_IsBusy()) {

    }
    if (!success) {
        LOG_ERROR << "Failed to read from EEPROM";
        return etl::nullopt;
    }
    return buffer;
}
