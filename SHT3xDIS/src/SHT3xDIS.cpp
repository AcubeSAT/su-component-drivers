#include "SHT3xDIS.hpp"

void SHT3xDIS::sendCommand(uint16_t command) {
    inline constexpr uint8_t NumberOfCommandBytes = 2;
    uint8_t command[NumberOfCommandBytes] = {static_cast<uint8_t>((command >> 8) & 0xFF), static_cast<uint8_t>(command & 0xFF)};

    if (SHT3xDIS_Write(I2CAddress, command, NumberOfCommandBytes)) {
        while (SHT3xDIS_IsBusy()) {}
    }
}

void SHT3xDIS::readRawMeasurements() {
    inline constexpr uint8_t DataSize = 6;
    etl::array<uint8_t, DataSize> data = {};

    inline constexpr uint8_t NumberOfMeasurements = 2;
    etl::array<float, NumberOfMeasurements> measurements = {};

    uint8_t ackData = 0;
    inline constexpr uint8_t NumberOfCommands = 2;
    uint8_t command[NumberOfCommands] = {DISABLED, HIGH_DISABLED};

    if (SHT3xDIS_Write(I2CAddress, command, 4)) {
        while (SHT3xDIS_IsBusy()) {}

        error = SHT3xDIS_ErrorGet();
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    if (SHT3xDIS_Read(I2CAddress, data, 6)) {
        while (SHT3xDIS_IsBusy()) {}

        error = SHT3xDIS_ErrorGet();
    }

    rawTemperature = (static_cast<uint16_t>(data[0]) << 8) | (data[1] & 0xFF);
    rawHumidity = (static_cast<uint16_t>(data[3]) << 8) | (data[4] & 0xFF);
}

float SHT3xDIS::getTemperature() const {
    return -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
}

float SHT3xDIS::getHumidity() const {
    return 100 * (static_cast<float>(rawHumidity) / 0xFFFF);
}
