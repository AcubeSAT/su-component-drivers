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

void SHT3xDIS::sendCommandToSensor(uint16_t command) {
    inline constexpr uint8_t NumberOfCommandBytes = 2;
    uint8_t command[NumberOfCommandBytes] = {static_cast<uint8_t>((command >> 8) & 0xFF), static_cast<uint8_t>(command & 0xFF)};

    if (SHT3xDIS_Write(I2CAddress, command, NumberOfCommandBytes)) {
        while (SHT3xDIS_IsBusy()) {} // TODO preferably add timeouts, while loops are dangerous
    }
}

void SHT3xDIS::executeWriteReadTransaction(i2cAddress, uint8_t bytesToWrite, uint8_t numberOfBytesToWrite, uint8_t bytesToRead, uint8_t numberOfBytesToRead) {
    if(SHT3xDIS_WriteRead(i2cAddress, bytesToWrite, numberOfBytesToWrite, bytesToRead, numberOfBytesToRead)) {
        while (SHT3xDIS_IsBusy()) {}
    }
    else {
        LOG_INFO << "Humidity sensor: I2C bus is busy";
        return
    }
}


void SHT3xDIS::readSensorDataSingleShotMode(uint8_t* sensorData) {
    inline constexpr uint8_t DataSizeWithCRC = 6;

    if (SHT3xDIS_Read(I2CAddress, sensorData, DataSizeWithCRC)) {
        while (SHT3xDIS_IsBusy()) {}
    }
    else {
        LOG_INFO << "Humidity sensor: I2C bus is busy";
        return
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
    inline constexpr uint8_t DataSizeWithCRC = 6;
    uint8_t sensorData[DataSizeWithCRC];

    sendCommandToSensor(SingleShotModeCommands::DISABLED_HIGH);

    vTaskDelay(pdMS_TO_TICKS(msToWait)); //TODO this is safer than continuously polling the bus and clock stretching

    readSensorDataSingleShotMode(sensorData);

    return etl::pair<float, float> (convertRawTemperatureValueToPhysicalScale(convertBytesToHalfWord(sensorData[0], sensorData[1])),
                                    convertRawHumidityValueToPhysicalScale(convertBytesToHalfWord(sensorData[3], sensorData[4])))
}

uint16_t SHT3xDIS::readStatusRegister() {

}