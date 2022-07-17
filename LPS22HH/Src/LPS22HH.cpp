#include "LPS22HH.hpp"

uint8_t LPS22HH::readFromRegister(RegisterAddress registerAddress){
    uint8_t rxData;

    PIO_PinWrite(ssn, false);

    uint8_t txData = registerAddress | 0b10000000;
    SPI_WriteRead(&txData, 1, &rxData, 1);

    PIO_PinWrite(ssn, true);

    return rxData;
}

void LPS22HH::writeToRegister(uint8_t registerAddress, etl::array<uint8_t, LPS22HH::maxWriteBytes> txData){

    PIO_PinWrite(ssn, false);

    SPI_WriteRead(&registerAddress, 1, nullptr, 0);
    SPI_WriteRead(&txData, txData.size(), nullptr, 0);

    PIO_PinWrite(ssn, true);
}

uint8_t LPS22HH::getStatus(){
    return readFromRegister(STATUS);
}

bool LPS22HH::temperatureDataAvailableCheck(){
    uint8_t status = getStatus();
    return (status | 0b00000010) >> 1;
}

bool LPS22HH::pressureDataAvailableCheck(){
    uint8_t status = getStatus();
    return (status | 0b00000001);
}

LPS22HH::LPS22HH(PIO_PIN ssn) : ssn(ssn){
    PIO_PinWrite(ssn, true);
}

void LPS22HH::readPressure(){
    int32_t pressureData;

    uint8_t pressureOutH = readFromRegister(PRESSURE_OUT_H);
    uint8_t pressureOutL = readFromRegister(PRESSURE_OUT_L);
    uint8_t pressureOutXL = readFromRegister(PRESSURE_OUT_XL);

    int8_t sign = (pressureOutH & 0b10000000) ? 0b11111111 : 0;

    pressureData =  (sign << 24) |
                    static_cast<int32_t> (pressureOutH << 16) |
                    static_cast<int32_t> (pressureOutL << 8)  |
                    static_cast<int32_t> (pressureOutXL);

    pressureValue = static_cast<float>(pressureData) / LPS22HH::pressureSensitivity;
}

void LPS22HH::readTemperature(){
    int16_t temperatureData;

    uint8_t temperatureOutH = readFromRegister(TEMP_OUT_H);
    uint8_t temperatureOutL = readFromRegister(TEMP_OUT_L);

    uint16_t unsigned_value = static_cast<uint16_t> (temperatureOutH) << 8 | temperatureOutL;
    temperatureData = unsigned_value | 0b1000000000000000;

    temperatureValue = static_cast<float>(temperatureData) / LPS22HH::temperatureSensitivity;
}



