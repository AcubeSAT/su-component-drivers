#include "PCA9685.hpp"

PCA9685::PCA9685(PCA9685::I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);
}

void PCA9685::readRegister(RegisterAddress registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead) {
    bool success = PCA9685_TWIHS_WriteRead(static_cast<uint8_t>(i2cAddress), reinterpret_cast<uint8_t *>(&registerAddress), 1, rData, numberOfBytesToRead);

    if (!success) {
        LOG_INFO << "PCA9685 with address " << i2cAddress << ": I2C bus is busy";
        return;
    }

    while(TWIHS2_IsBusy());

    if(PCA9685_TWIHS_ErrorGet != TWIHS_ERROR_NONE) {
        LOG_ERROR << "PCA9685 with address " << i2cAddress << " , is disconnected, suspending task";
        vTaskDelay(0);
    }

}

void PCA9685::writeRegister(uint8_t *tData, uint8_t numberOfBytesToWrite) {
    bool success = PCA9685_TWIHS_Write(static_cast<uint8_t>(i2cAddress), tData, numberOfBytesToWrite);

    if (!success) {
        LOG_INFO << "PCA9685 with address " << i2cAddress << ": I2C bus is busy";
        return;
    }

    while(TWIHS2_IsBusy());

    if(PCA9685_TWIHS_ErrorGet != TWIHS_ERROR_NONE) {
        LOG_ERROR << "PCA9685 with address " << i2cAddress << " , is disconnected, suspending task";
        vTaskDelay(0);
    }
}
