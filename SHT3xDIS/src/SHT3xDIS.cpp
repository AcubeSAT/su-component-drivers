#include "SHT3xDIS.hpp"

void SHT3xDIS::readRawMeasurements() {
    inline constexpr uint8_t DataSize = 6;
    etl::array<uint8_t, DataSize> data = {};

    inline constexpr uint8_t NumberOfMeasurements = 2;
    etl::array<float, NumberOfMeasurements> measurements = {};

    uint8_t ackData = 0;
    inline constexpr uint8_t NumberOfCommands = 2;
    uint8_t command[NumberOfCommands] = {DISABLED, HIGH_DISABLED};

    if (TWIHS2_Write(I2CAddress, &ackData, 1)) {
        waitForResponse();
        TWIHS_ERROR error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(I2CAddress, command, 4)) {
        while (TWIHS2_IsBusy()) {}

        error = TWIHS2_ErrorGet();
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    if (TWIHS2_Read(I2CAddress, data, 6)) {
        while (TWIHS2_IsBusy()) {}

        error = TWIHS2_ErrorGet();
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
