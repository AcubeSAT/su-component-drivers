#include "PCA9685.hpp"

PCA9685::PCA9685(PCA9685::I2CAddress i2cAddress) : i2cAddress(i2cAddress) {

    slaveAddressWrite = ((static_cast<uint8_t>(i2cAddress) << 1) | static_cast<uint8_t>(0b10000000));
    slaveAddressRead = slaveAddressWrite | static_cast<uint8_t>(1);
}

void PCA9685::readRegister(RegisterAddresses registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead) {
    bool success = PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t*>(&registerAddress), 1, rData, numberOfBytesToRead);

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

void PCA9685::readRegister(uint8_t registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead) {
    bool success = PCA9685_TWIHS_WriteRead(slaveAddressRead, reinterpret_cast<uint8_t*>(&registerAddress), 1, rData, numberOfBytesToRead);

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
    auto restart = static_cast<uint8_t>(HardwareRestart::DISABLED);
    auto extclk = static_cast<uint8_t>(Oscillator::INTERNAL);
    auto autoIncrement = static_cast<uint8_t>(RegisterAutoIncrement::DISABLED);
    auto sleep = static_cast<uint8_t>(SleepMode::NORMAL);
    auto sub1 = static_cast<uint8_t>(RespondToI2CBusAddresses::SUB1_NO_RESPOND);
    auto sub2 = static_cast<uint8_t>(RespondToI2CBusAddresses::SUB2_NO_RESPOND);
    auto sub3 = static_cast<uint8_t>(RespondToI2CBusAddresses::SUB3_NO_RESPOND);
    auto allCall = static_cast<uint8_t>(RespondToI2CBusAddresses::ALLCALL_RESPOND);

    uint8_t tData[1] = {static_cast<uint8_t>(restart | extclk | autoIncrement | sleep | sub1 | sub2 | sub3 | allCall)};

    writeRegister(tData, sizeof(tData));
}

void PCA9685::setMode2Register() {
    auto invrt = static_cast<uint8_t>(OutputInvert::NOT_INVERTED);
    auto och = static_cast<uint8_t>(OutputChangesOn::STOP);
    auto outdrv = static_cast<uint8_t>(OutputConfiguration::OPEN_DRAIN_STRUCTURE);
    auto outne = static_cast<uint8_t>(OEPinHighStates::LOW);

    uint8_t tData[1] = {static_cast<uint8_t>(invrt | och | outdrv | outne)};

    writeRegister(tData, sizeof(tData));
}

void PCA9685::setDeviceToSleep(bool sleep) {

}

void PCA9685::setDeviceFrequency(uint16_t frequency) {

}

void PCA9685::setPWMChannel(uint8_t channel, uint8_t dutyCyclePercent) {

    if(dutyCyclePercent == 0)
        setPWMChannelAlwaysOff(channel);
    else if(dutyCyclePercent > 99)
        setPWMChannelAlwaysOn(channel);

    uint8_t frac = static_cast<uint8_t>(100) / dutyCyclePercent;
    uint16_t dutyCycle = GrayscaleMaximumSteps / static_cast<uint16_t>(frac) - 1;

    uint16_t maskLSB = 0xFF;
    uint16_t maskMSB = 0xF00;

    uint16_t dutyCycleLSB = dutyCycle & maskLSB;
    uint16_t dutyCycleMSB = dutyCycle & maskMSB;

    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;

    uint8_t byte0 = 0x0;
    uint8_t byte1 = 0x0;
    auto byte2 = static_cast<uint8_t>(dutyCycleLSB);
    auto byte3 = static_cast<uint8_t>(dutyCycleMSB);

    uint8_t rData[1];

    readRegister(LEDn_ON_L, rData, static_cast<uint8_t>(sizeof rData));
    readRegister(LEDn_ON_H, rData, static_cast<uint8_t>(sizeof rData));
    readRegister(LEDn_OFF_L, rData, static_cast<uint8_t>(sizeof rData));
    readRegister(LEDn_OFF_H, rData, static_cast<uint8_t>(sizeof rData));

}

void PCA9685::setPWMChannel(uint8_t channel, uint8_t dutyCycle, uint8_t delay) {
    uint8_t frac = static_cast<uint8_t>(100) / dutyCycle;
    uint16_t cyclesPWMHigh = GrayscaleMaximumSteps / static_cast<uint16_t>(frac);

    uint8_t LEDn_ON_L = RegisterAddressOfFirstPWMChannel + NumberOfBytesPerPWMChannelRegisters * channel;
    uint8_t LEDn_ON_H = LEDn_ON_L + 1;
    uint8_t LEDn_OFF_L = LEDn_ON_L + 2;
    uint8_t LEDn_OFF_H = LEDn_ON_L + 3;
}

void PCA9685::setPWMChannelAlwaysOff(uint8_t channel) {

}

void PCA9685::setPWMChannelAlwaysOn(uint8_t channel) {

}

