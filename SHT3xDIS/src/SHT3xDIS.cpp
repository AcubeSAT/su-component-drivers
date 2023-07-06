#include "SHT3xDIS.hpp"

bool SHT3xDIS::crc8(uint8_t msb, uint8_t lsb, uint8_t checksum) {
    uint8_t CRC = 0xFF;
    uint8_t polynomial = 0x31;

    CRC ^= msb;
    for (uint8_t index = 0; index < 8; index++) {
        CRC = CRC & 0x80 ? (CRC << 1) ^ polynomial : CRC << 1;
    }

    CRC ^= lsb;
    for (uint8_t index = 0; index < 8; index++) {
        CRC = CRC & 0x80 ? (CRC << 1) ^ polynomial : CRC << 1;
    }

    return CRC == checksum;
}

template<typename F, typename... Arguments>
bool SHT3xDIS::executeI2CTransaction(F i2cFunction, Arguments... arguments) {
    if (i2cFunction(I2CAddress, arguments...)) {
        waitForI2CBuffer();
        checkForNACK();
        return true;
    } else {
        LOG_INFO << "Humidity sensor: I2C bus is busy";
        return false;
    }
}

void SHT3xDIS::checkForNACK() {
    if (SHT3xDIS_TWIHS_ErrorGet() == TWIHS_ERROR_NACK) {
        LOG_ERROR << "Humidity-Temperature sensor is disconnected, suspending task";
        vTaskSuspend(nullptr);
    }
}

void SHT3xDIS::sendCommandToSensor(uint16_t command) {
    etl::array<uint8_t, NumberOfBytesInCommand> commandBytes{};
    splitHalfWordToByteArray(commandBytes, command);

    executeI2CTransaction(SHT3xDIS_TWIHS_Write, commandBytes.data(), NumberOfBytesInCommand);
}

void SHT3xDIS::executeWriteReadTransaction(etl::array<uint8_t, NumberOfBytesOfStatusRegisterWithCRC>& statusRegisterData) {
    etl::array<uint8_t, NumberOfBytesInCommand> commandBytes{};

    splitHalfWordToByteArray(commandBytes, StatusRegisterCommands::READ);

    if(not executeI2CTransaction(SHT3xDIS_TWIHS_WriteRead, commandBytes.data(), NumberOfBytesInCommand, statusRegisterData.data(),
                        NumberOfBytesOfStatusRegisterWithCRC)) {
        return;
    }

    if constexpr (UseCRC) {
        if (not crc8(statusRegisterData[0], statusRegisterData[1], statusRegisterData[2])) {
            LOG_ERROR << "Error in Humidity Sensor Status Register checksum";
        }
    }
}

void SHT3xDIS::readSensorDataSingleShotMode(etl::array<uint8_t, NumberOfBytesOfMeasurementsWithCRC>& sensorData) {
    if(not executeI2CTransaction(SHT3xDIS_TWIHS_Read, sensorData.data(), NumberOfBytesOfMeasurementsWithCRC)) {
        return;
    }

    if constexpr (UseCRC) {
        if (not crc8(sensorData[0], sensorData[1], sensorData[2])) {
            LOG_ERROR << "Error in Humidity Sensor temperature value checksum";
        }

        if (not crc8(sensorData[3], sensorData[4], sensorData[5])) {
            LOG_ERROR << "Error in Humidity Sensor humidity value checksum";
        }
    }

}

etl::pair<float, float> SHT3xDIS::getOneShotMeasurement() {
    etl::array<uint8_t, NumberOfBytesOfMeasurementsWithCRC> sensorData{};

    sendCommandToSensor(SingleShotModeCommands::DISABLED_HIGH);

    vTaskDelay(pdMS_TO_TICKS(msToWait));

    readSensorDataSingleShotMode(sensorData);

    return {convertRawTemperatureValueToPhysicalScale(concatenateTwoBytesToHalfWord(sensorData[0], sensorData[1])),
            convertRawHumidityValueToPhysicalScale(concatenateTwoBytesToHalfWord(sensorData[3], sensorData[4]))};
}

void SHT3xDIS::setHeater(SHT3xDIS::HeaterCommands command) {
    sendCommandToSensor(command);
}

void SHT3xDIS::clearStatusRegister() {
    sendCommandToSensor(StatusRegisterCommands::CLEAR);
}

uint16_t SHT3xDIS::readStatusRegister() {
    etl::array<uint8_t, NumberOfBytesOfStatusRegisterWithCRC> statusRegisterData{};

    executeWriteReadTransaction(statusRegisterData);

    return concatenateTwoBytesToHalfWord(statusRegisterData[0], statusRegisterData[1]);
}

void SHT3xDIS::performSoftReset() {
    PIO_PinWrite(NResetPin, false);
    vTaskDelay(pdMS_TO_TICKS(msToWait));
    PIO_PinWrite(NResetPin, true);
}
