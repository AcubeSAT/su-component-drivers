#include "LPS22HH.hpp"

template<typename F, typename... Arguments>
bool LPS22HH::executeI2CTransaction(F i2cFunction, Arguments... arguments) {
    if (i2cFunction(I2CAddress, arguments...)) {
        waitForI2CBuffer();
        checkForNACK();
        return true;
    }

    LOG_INFO << "Pressure sensor with address " << static_cast<uint8_t>(I2CAddress) << ": I2C bus is busy";
    return false;
}

void LPS22HH::checkForNACK() {
    auto error = LPS22HH_TWIHS_ErrorGet();
    if (error == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Pressure sensor with address " << static_cast<uint8_t>(I2CAddress) <<
                " , is disconnected, suspending task";
        vTaskSuspend(nullptr);
    }
}

void LPS22HH::writeToRegister(RegisterAddress registerAddress, uint8_t txData) {
    uint8_t sendData[2] = {registerAddress, txData};
    executeI2CTransaction(LPS22HH_TWIHS_Write, sendData, sizeof(sendData));
}

uint8_t LPS22HH::readFromRegister(RegisterAddress registerAddress) {
    uint8_t registerData = 0;
    uint8_t registerAddressArray[1] = {registerAddress};
    executeI2CTransaction(LPS22HH_TWIHS_WriteRead, registerAddressArray, NumberOfBytesInCommand,
                          &registerData, sizeof(registerData));
    return registerData;
}

float LPS22HH::readPressure() {
    triggerOneShotMode();

    const uint8_t pressureOutH = readFromRegister(PRESSURE_OUT_H);
    const uint8_t pressureOutL = readFromRegister(PRESSURE_OUT_L);
    const uint8_t pressureOutXL = readFromRegister(PRESSURE_OUT_XL);

    const int8_t sign = (pressureOutH & 0b1000'0000) ? 0b1111'1111 : 0;

    const int32_t pressureData = (sign << 24) |
                           static_cast<int32_t>(pressureOutH << 16) |
                           static_cast<int32_t>(pressureOutL << 8) |
                           static_cast<int32_t>(pressureOutXL);

    return static_cast<float>(pressureData) / PressureSensitivity;
}

float LPS22HH::readTemperature() {
    triggerOneShotMode();

    const uint8_t temperatureOutH = readFromRegister(TEMP_OUT_H);
    const uint8_t temperatureOutL = readFromRegister(TEMP_OUT_L);

    auto signedValue = static_cast<int16_t>(((static_cast<uint16_t>(temperatureOutH) << 8) & 0xFF00) | temperatureOutL);

    return static_cast<float>(signedValue) / TemperatureSensitivity;
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

void LPS22HH::triggerOneShotMode() {
    /* todo: read CTRL_REG2 first and then write, to preserve any previous config done */
    uint8_t oneShotBit = 0x01;
    writeToRegister(CTRL_REG2, oneShotBit);

    vTaskDelay(pdMS_TO_TICKS(500));
}

