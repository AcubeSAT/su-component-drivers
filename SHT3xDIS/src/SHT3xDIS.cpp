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

    }

}

etl::pair<float, float> SHT3xDIS::getOneShotMeasurement() {
    inline constexpr uint8_t DataSizeWithCRC = 6;
    uint8_t sensorData[DataSizeWithCRC];

    sendCommandToSensor(SingleShotCommands::HIGH_DISABLED);

    vTaskDelay(pdMS_TO_TICKS(1));

    readSensorDataSingleShotMode(sensorData);

    return etl::pair<float, float> (convertRawTemperatureValueToPhysicalScale(convertBytesToHalfWord(data[0], data[1])),
                                    convertRawHumidityValueToPhysicalScale(convertBytesToHalfWord(data[3], data[4])))
}

bool SHT3xDIS::crc8(uint8_t msb, uint8_t lsb, uint8_t checksum) {

}
