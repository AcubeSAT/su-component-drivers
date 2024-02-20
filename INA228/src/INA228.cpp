#include "INA228.hpp"

INA228::INA228(INA228::I2CAddress i2cAddress, INA228::Configuration configuration,
               INA228::ADCConfiguration adcConfiguration) : i2cAddress(i2cAddress) {

    setConfig(configuration);
    setADCConfig(adcConfiguration);
    setShuntCalRegister(configuration);
}

void INA228::setConfig(INA228::Configuration configuration) {
    uint8_t tData[3] = {static_cast<uint8_t>(RegisterAddress::CONFIG)
            ,static_cast<uint8_t>((static_cast<uint16_t>(configuration) >> 8) & 0xFF)
            ,static_cast<uint8_t>(static_cast<uint16_t>(configuration) & 0xFF)};

    writeRegister(tData, sizeof(tData));

}

void INA228::setADCConfig(INA228::ADCConfiguration adcConfiguration) {
    uint8_t tData[3] = {static_cast<uint8_t>(RegisterAddress::ADC_CONFIG)
            ,static_cast<uint8_t>((static_cast<uint16_t>(adcConfiguration) >> 8) & 0xFF)
            ,static_cast<uint8_t>(static_cast<uint16_t>(adcConfiguration) & 0xFF)};

    writeRegister(tData, sizeof(tData));

}

void INA228::setShuntCalRegister(INA228::Configuration configuration) {
    // Determine whether ADCRANGE is 0 or 1
    adcRangeValue = static_cast<uint16_t>(configuration) & 0b10000;

    if(adcRangeValue != 0) {
        shuntCalValue = shuntCalValue * 4;
    }

    uint8_t tData[3] = {static_cast<uint8_t>(RegisterAddress::SHUNT_CAL)
            ,static_cast<uint8_t>((shuntCalValue >> 8) & 0xFF)
            ,static_cast<uint8_t>((shuntCalValue) & 0xFF)};

    writeRegister(tData, sizeof(tData));

}

bool INA228::readRegister(INA228::RegisterAddress registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead) const {
    if (not INA228_TWIHS_WriteRead(static_cast<uint8_t>(i2cAddress), reinterpret_cast<uint8_t *>(&registerAddress), 1, rData, numberOfBytesToRead)) {
        LOG_INFO << "Current monitor failed to perform I2C transaction: bus is busy";
        return false;
    }

    while(INA228_TWIHS_IsBusy());

    if(INA228_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Current monitor failed to perform I2C transaction: device NACK";
        return false;
    }

    return true;
}

bool INA228::writeRegister(uint8_t *tData, uint8_t numberOfBytesToWrite) const {
    if (not INA228_TWIHS_Write(static_cast<uint8_t>(i2cAddress), tData, numberOfBytesToWrite)) {
        LOG_INFO << "Current monitor failed to perform I2C transaction: bus is busy";
        return false;
    }

    while(INA228_TWIHS_IsBusy());

    if(INA228_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Current monitor failed to perform I2C transaction: device NACK";
        return false;
    }

    return true;
}

float INA228::getCurrent() const {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::CURRENT, returnedData, 3);

    auto current = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                         | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                         | static_cast<uint32_t>(returnedData[2] & 0xFF));

    current = (current >> 4) & 0xFFFFF;

    uint32_t sign = current & 0x80000;

    if (sign != 0) {
        current = (~current & 0xFFFFF) + 1;
    }

    return static_cast<float>(current) * CurrentLSB;
}

float INA228::getPower() const {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::POWER, returnedData, 3);

    auto power = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                       | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                       | static_cast<uint32_t>(returnedData[2] & 0xFF));

    return 3.2f * CurrentLSB * static_cast<float>(power);
}

float INA228::getVoltage() const {
    constexpr float ResolutionSize = 0.0001953125f; // in volts

    uint8_t returnedData[3];
    readRegister(RegisterAddress::VBUS, returnedData, 3);

    auto busVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                            | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                            | static_cast<uint32_t>(returnedData[2] & 0xFF));

    busVoltage = (busVoltage >> 4) & 0xFFFFF;

    return static_cast<float>(busVoltage) * ResolutionSize;
}

float INA228::getDieTemperature() const {
    constexpr float ResolutionSize = 0.0078125f;

    uint8_t returnedData[2];
    readRegister(RegisterAddress::DIETEMP, returnedData, 2);

    auto internalTemperature = static_cast<uint16_t>(static_cast<uint16_t>((static_cast<uint16_t>(returnedData[0]) << 8) & 0xFF00)
                                                     | static_cast<uint16_t>(returnedData[1] & 0xFF));

    uint32_t sign = internalTemperature & 0x8000;

    if(sign != 0) {
        internalTemperature = (~internalTemperature & 0xFFFFF) + 1;
    }

    return static_cast<float>(internalTemperature) * ResolutionSize;
}

float INA228::getEnergy() const {
    uint8_t returnedData[5];
    readRegister(RegisterAddress::ENERGY, returnedData, 5);

    auto energy = static_cast<uint64_t>(((static_cast<uint64_t>(returnedData[0]) << 32) & 0xFF00000000) |
                                        ((static_cast<uint64_t>(returnedData[1]) << 24) & 0xFF000000) |
                                        ((static_cast<uint64_t>(returnedData[2]) << 16) & 0xFF0000) |
                                        ((static_cast<uint64_t>(returnedData[3]) << 8) & 0xFF00) |
                                        ((static_cast<uint64_t>(returnedData[4])) & 0xFF));

    return 16.0f * 3.2f * CurrentLSB * static_cast<float>(energy);
}

float INA228::getShuntVoltage() const {
    float resolutionSize = (adcRangeValue == 0) ? 0.0003125f : 0.000078125f;

    uint8_t returnedData[3];
    readRegister(RegisterAddress::VSHUNT, returnedData, 3);

    auto shuntVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                              | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                              | static_cast<uint32_t>(returnedData[2] & 0xFF));

    shuntVoltage = (shuntVoltage >> 4) & 0xFFFFF;

    uint32_t sign = shuntVoltage & 0x80000;

    if (sign != 0) {
        shuntVoltage = (~shuntVoltage & 0xFFFFF) + 1;
    }

    return static_cast<float>(shuntVoltage) * resolutionSize;
}
