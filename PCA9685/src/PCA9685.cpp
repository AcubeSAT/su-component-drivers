#include "PCA9685.hpp"

PCA9685::PCA9685(PCA9685::I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);
}

void PCA9685::readRegister(RegisterAddress registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead) {
    bool success = PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t *>(&registerAddress), 1, rData, numberOfBytesToRead);

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
    bool success = PCA9685_TWIHS_Write(slaveAddressWrite, tData, numberOfBytesToWrite);

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

void PCA9685::setMode1Register() {
    uint8_t restart = 0x0;
    auto extclk = static_cast<uint8_t>(Oscillator::INTERNAL);
    auto autoIncrement = static_cast<uint8_t>(RegisterAutoIncrement::DISABLED);
    auto sleep = static_cast<uint8_t>(SleepMode::NORMAL);
    uint8_t sub1 = 0x0;
    uint8_t sub2 = 0x0;
    uint8_t sub3 = 0x0;
    uint8_t allCall = 0x0;

    uint8_t tData[1] = {static_cast<uint8_t>(static_cast<uint8_t >(restart<<7) | static_cast<uint8_t >(extclk<<6)
                                             | static_cast<uint8_t >(autoIncrement<<5) | static_cast<uint8_t>(sleep)
                                             | static_cast<uint8_t >(sub1<<3) | static_cast<uint8_t >(sub2<<2)
                                             | static_cast<uint8_t >(sub3<<1) | static_cast<uint8_t >(allCall))};

    writeRegister(tData, sizeof(tData));
}

void PCA9685::setMode2Register() {
    auto invrt = static_cast<uint8_t>(OutputInvert::NOT_INVERTED);
    auto och = static_cast<uint8_t>(OutputChangesOn::STOP);
    auto outdrv = static_cast<uint8_t>(OutputConfiguration::OPEN_DRAIN_STRUCTURE);
    auto outne = static_cast<uint8_t>(OEPinHighStates::LOW);

    uint8_t tData[1] = {static_cast<uint8_t>(static_cast<uint8_t>(invrt<<4) | static_cast<uint8_t >(och<<3)
                        | static_cast<uint8_t>(outdrv<<2) | outne)};

    writeRegister(tData, sizeof(tData));
}


