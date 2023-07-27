#include "INA228.hpp"

INA228::INA228() {
    uint8_t data[3] = {static_cast<uint8_t>(RegisterAddress::SHUNT_CAL)
                       , static_cast<uint8_t>((ShuntCalValue >> 8) & 0xFF)
                       , static_cast<uint8_t>((ShuntCalValue) & 0xFF)};

    INA228_TWIHS_Write(i2cAddress, data, 3);

    while(TWIHS2_IsBusy()) {}
}

void INA228::readRegister(RegisterAddress registerAddress, uint8_t* returnedData, uint8_t numberOfBytesToRead) {
    INA228_TWIHS_WriteRead(i2cAddress, reinterpret_cast<uint8_t *>(&registerAddress), 1, returnedData, numberOfBytesToRead);

    while(TWIHS2_IsBusy()) {}

}

float INA228::getCurrent() {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::CURRENT, returnedData, 3);

    uint32_t current = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                | static_cast<uint32_t>((returnedData[1] <<8) & 0xFF00)
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
                                             | static_cast<uint32_t>((returnedData[1] <<8) & 0xFF00)
                                             | static_cast<uint32_t>(returnedData[2] & 0xFF));

    return 3.2f * CurrentLSB * static_cast<float>(power);
}

float INA228::getVoltage() {
    uint8_t returnedData[3];
    readRegister(RegisterAddress::VBUS, returnedData, 3);

    uint32_t busVoltage = static_cast<uint32_t>(static_cast<uint32_t>((returnedData[0] << 16) & 0xFF0000)
                                                  | static_cast<uint32_t>((returnedData[1] <<8) & 0xFF00)
                                                  | static_cast<uint32_t>(returnedData[2] & 0xFF));

    busVoltage = (busVoltage >> 4) & 0xFFFFF;

    uint8_t sign = busVoltage & 0x80000;

    if (sign != 0) {
        busVoltage = (~busVoltage & 0xFFFFF) + 1;
    }

    return static_cast<float>(busVoltage) * CurrentLSB
}
