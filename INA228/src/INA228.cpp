#include "INA228.hpp"

INA228::INA228(INA228::I2CAddress i2cAddress, INA228::Configuration configuration,
               INA228::ADCConfiguration adcConfiguration) : i2cAddress(i2cAddress) {

    setConfig(configuration);
    setADCConfig(adcConfiguration);
    setShuntCalRegister();
}

void INA228::setConfig(INA228::Configuration configuration) {
    uint8_t data[3] = {static_cast<uint8_t>(RegisterAddress::CONFIG)
            ,static_cast<uint8_t>((static_cast<uint16_t>(configuration) >> 8) & 0xFF)
            ,static_cast<uint8_t>(static_cast<uint16_t>(configuration) & 0xFF)};

    INA228_TWIHS_Write(i2cAddress, data, 3);

    while(TWIHS2_IsBusy()) {

    }
}

void INA228::setADCConfig(INA228::ADCConfiguration adcConfiguration) {
    uint8_t data[3] = {static_cast<uint8_t>(RegisterAddress::ADC_CONFIG)
            ,static_cast<uint8_t>((static_cast<uint16_t>(adcConfiguration) >> 8) & 0xFF)
            ,static_cast<uint8_t>(static_cast<uint16_t>(adcConfiguration) & 0xFF)};

    INA228_TWIHS_Write(i2cAddress, data, 3);

    while(TWIHS2_IsBusy()) {

    }
}

void INA228::setShuntCalRegister(INA228::Configuration configuration) {
    // Determine whether adcrange is 0 or 1
    uint16_t adcrange = static_cast<uint16_t>(configuration) & 0b10000;

    if(adcrange != 0) {
        ShuntCalValue = ShuntCalValue * 4;
    }

    uint8_t data[3] = {static_cast<uint8_t>(RegisterAddress::SHUNT_CAL)
            ,static_cast<uint8_t>((ShuntCalValue >> 8) & 0xFF)
            ,static_cast<uint8_t>((ShuntCalValue) & 0xFF)};

    INA228_TWIHS_Write(i2cAddress, data, 3);

    while(TWIHS2_IsBusy()) {

    }
}

void INA228::readRegister(RegisterAddress registerAddress, uint8_t* returnedData, uint8_t numberOfBytesToRead) {
    INA228_TWIHS_WriteRead(i2cAddress, reinterpret_cast<uint8_t *>(&registerAddress), 1, returnedData, numberOfBytesToRead);

    while(TWIHS2_IsBusy()) {

    }

}

float INA228::getCurrent() {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::CURRENT, returnedData, 3);

    uint32_t current = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                                | static_cast<uint32_t>(returnedData[2] & 0xFF));

    current = (current >> 4) & 0xFFFFF;

    uint8_t sign = current & 0x80000;

    if (sign != 0) {
        current = (~current & 0xFFFFF) + 1;
    }

    return static_cast<float>(current) * CurrentLSB;
}

float INA228::getPower() {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::POWER, returnedData, 3);

    uint32_t power = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                             | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                             | static_cast<uint32_t>(returnedData[2] & 0xFF));

    return 3.2f * CurrentLSB * static_cast<float>(power);
}

float INA228::getVoltage() {
    float resolutionSize = 0.0001953125; // in volts

    uint8_t returnedData[3];
    readRegister(RegisterAddress::VBUS, returnedData, 3);

    uint32_t busVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                  | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                                  | static_cast<uint32_t>(returnedData[2] & 0xFF));

    busVoltage = (busVoltage >> 4) & 0xFFFFF;

    return static_cast<float>(busVoltage) * resolutionSize
}

float INA228::getDieTemperature() {
    float resolutionSize = 0.0078125;

    uint8_t returnedData[2];
    readRegister(RegisterAddress::DIETEMP, returnedData, 2);

    uint16_t internalTemperature = static_cast<uint16_t>(static_cast<uint16_t>((static_cast<uint16_t>(returnedData[0]) << 8) & 0xFF00)
                                                            | static_cast<uint16_t>(returnedData[1] & 0xFF));

    uint8_t sign = internalTemperature & 0x8000;

    if(sign != 0) {
        internalTemperature = (~internalTemperature & 0xFFFFF) + 1;
    }

    return static_cast<float>(internalTemperature) * resolutionSize;
}

float INA228::getEnergy() {
    uint8_t returnedData[5];
    readRegister(RegisterAddress::ENERGY, returnedData, 5);

    uint64_t energy = static_cast<uint64_t>(((static_cast<uint64_t>(returnedData[0]) << 32) & 0xFF00000000) |
                                            ((static_cast<uint64_t>(returnedData[1]) << 24) & 0xFF000000) |
                                            ((static_cast<uint64_t>(returnedData[2]) << 16) & 0xFF0000) |
                                            ((static_cast<uint64_t>(returnedData[3]) << 8) & 0xFF00) |
                                            ((static_cast<uint64_t>(returnedData[4])) & 0xFF));

    return 16.0f * 3.2f * CurrentLSB * static_cast<float>(energy);
}

float INA228::getShuntVoltage() {
    float resolutionSize = (ADCRange == 0) ? 0.0003125 : 0.000078125;

    uint8_t returnedData[3];
    readRegister(RegisterAddress::VSHUNT, returnedData, 3);

    uint32_t shuntVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                    | static_cast<uint32_t>((returnedData[1] << 8) & 0xFF00)
                                                    | static_cast<uint32_t>(returnedData[2] & 0xFF));

    shuntVoltage = (shuntVoltage >> 4) & 0xFFFFF;

    uint8_t sign = shuntVoltage & 0x80000;

    if (sign != 0) {
        shuntVoltage = (~shuntVoltage & 0xFFFFF) + 1;
    }

    return static_cast<float>(shuntVoltage) * resolutionSize;
}
