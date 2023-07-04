#include "SHT3xDIS.hpp"

void SHT3xDIS::sendCommand(uint16_t command) {
    inline constexpr uint8_t NumberOfCommandBytes = 2;
    uint8_t command[NumberOfCommandBytes] = {static_cast<uint8_t>((command >> 8) & 0xFF), static_cast<uint8_t>(command & 0xFF)};

    if (SHT3xDIS_Write(I2CAddress, command, NumberOfCommandBytes)) {
        while (SHT3xDIS_IsBusy()) {}
    }
}

void SHT3xDIS::readHumidityAndTemperature(uint8_t* data) {
    inline constexpr uint8_t DataSizeWithCRC = 6;

    if (SHT3xDIS_Read(I2CAddress, data, DataSizeWithCRC)) {
        while (SHT3xDIS_IsBusy()) {}
    }
    else {

    }

    if constexpr (UseCRC) {

    }

}

etl::pair<float, float> SHT3xDIS::getOneShotMeasurements() {
    inline constexpr uint8_t DataSizeWithCRC = 6;
    uint8_t data[DataSizeWithCRC];

    sendCommand(SingleShotCommands::HIGH_DISABLED);

    vTaskDelay(pdMS_TO_TICKS(1));

    readHumidityAndTemperature(data);

    uint16_t rawTemperature = (static_cast<uint16_t>(data[0]) << 8) | (data[1] & 0xFF);
    uint16_t rawHumidity = (static_cast<uint16_t>(data[3]) << 8) | (data[4] & 0xFF);

    float temperature = -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
    float humidity = 100 * (static_cast<float>(rawHumidity) / 0xFFFF);

    return etl::pair<float, float> (temperature, humidity)
}

bool SHT3xDIS::crc8(uint8_t msb, uint8_t lsb, uint8_t checksum) {

}
