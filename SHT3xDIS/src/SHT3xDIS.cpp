#include "SHT3xDIS.hpp"

void SHT3xDIS::sendCommandToSensor(uint16_t command) {
    inline constexpr uint8_t NumberOfCommandBytes = 2;
    uint8_t command[NumberOfCommandBytes] = {static_cast<uint8_t>((command >> 8) & 0xFF), static_cast<uint8_t>(command & 0xFF)};

    if (SHT3xDIS_Write(I2CAddress, command, NumberOfCommandBytes)) {
        while (SHT3xDIS_IsBusy()) {}
    }
}

void SHT3xDIS::readSensorDataSingleShotMode(uint8_t* sensorData) {
    inline constexpr uint8_t DataSizeWithCRC = 6;

    if (SHT3xDIS_Read(I2CAddress, sensorData, DataSizeWithCRC)) {
        while (SHT3xDIS_IsBusy()) {}
    }
    else {

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

    sendCommandToSensor(SingleShotCommands::HIGH_DISABLED);

    vTaskDelay(pdMS_TO_TICKS(msToWait));

    readSensorDataSingleShotMode(sensorData);

    return etl::pair<float, float> (convertRawTemperatureValueToPhysicalScale(convertBytesToHalfWord(sensorData[0], sensorData[1])),
                                    convertRawHumidityValueToPhysicalScale(convertBytesToHalfWord(sensorData[3], sensorData[4])))
}

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
