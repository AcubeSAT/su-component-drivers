#include "LPS22HH.hpp"

uint8_t LPS22HH::readFromRegister(RegisterAddress registerAddress) const {
    uint8_t rxData;

    PIO_PinWrite(ssn, false);

    uint8_t txData = registerAddress | SPI_READ_COMMAND;
    LPS22HH_SPI_Write(&txData, 1);
    while (SPI0_IsBusy()) {}

    LPS22HH_SPI_Read(&rxData, 1);
    while (SPI0_IsBusy()) {}

    PIO_PinWrite(ssn, true);

    return rxData;
}

void LPS22HH::writeToRegister(RegisterAddress registerAddress, uint8_t txData) const {
    PIO_PinWrite(ssn, false);

    uint16_t spiCommand = (registerAddress << 8) | txData;
    LPS22HH_SPI_Write(&spiCommand, 2);

    PIO_PinWrite(ssn, true);
}

uint8_t LPS22HH::getStatus() {
    return readFromRegister(STATUS);
}

float LPS22HH::readPressure() {
    int32_t pressureData;

    uint8_t pressureOutH = readFromRegister(PRESSURE_OUT_H);
    uint8_t pressureOutL = readFromRegister(PRESSURE_OUT_L);
    uint8_t pressureOutXL = readFromRegister(PRESSURE_OUT_XL);

    int8_t sign = (pressureOutH & 0b10000000) ? 0b11111111 : 0;

    pressureData = (sign << 24) |
                   static_cast<int32_t> (pressureOutH << 16) |
                   static_cast<int32_t> (pressureOutL << 8) |
                   static_cast<int32_t> (pressureOutXL);

    pressureValue = static_cast<float>(pressureData) / PressureSensitivity;

    return pressureValue;
}

float LPS22HH::readTemperature() {
    PIO_PinWrite(ssn, false);
    uint8_t data[2] = {0x11, 1};

    LPS22HH_SPI_Write(&data, 2);
    while (LPS22HH_SPI_IsBusy()) {}
    PIO_PinWrite(ssn, true);

    vTaskDelay(pdMS_TO_TICKS(500));

    uint8_t temperatureOutH = readFromRegister(TEMP_OUT_H);
    uint8_t temperatureOutL = readFromRegister(TEMP_OUT_L);

    auto signedValue = static_cast<int16_t>(((static_cast<uint16_t>(temperatureOutH) << 8) & 0xFF00) | temperatureOutL);
//    temperatureData = unsigned_value | 0b10000000'00000000;

    temperatureValue = static_cast<float>(signedValue) / TemperatureSensitivity;

    return temperatureValue;
}

void LPS22HH::setODRBits(OutputDataRate rate) {
    writeToRegister(CTRL_REG1, rate);
}

void LPS22HH::setStopOnWTM(bool stopOnWTM) {
    uint8_t registerData = readFromRegister(FIFO_CTRL);
    registerData = registerData | (stopOnWTM << 3);
    writeToRegister(FIFO_CTRL, registerData);
}

void LPS22HH::setTrigModes(bool trigMode) {
    uint8_t registerData = readFromRegister(FIFO_CTRL);
    registerData = registerData | (trigMode << 2);
    writeToRegister(FIFO_CTRL, registerData);
}

void LPS22HH::setFIFOMode(FIFOModes mode) {
    uint8_t registerData = readFromRegister(FIFO_CTRL);
    registerData = registerData | mode;
    writeToRegister(FIFO_CTRL, registerData);
}

void LPS22HH::activateOneShotMode() {

    /* todo: read CTRL_REG2 first and then write, to preserve any previous config done */
    uint8_t txData = 0x1; /* ONE_SHOT bit of CTRL_REG2 register is bit 0 */
    writeToRegister(CTRL_REG2, txData);

}

void LPS22HH::performAreYouAliveCheck() {

};