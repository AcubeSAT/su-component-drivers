#include "SHT3xDIS.hpp"

SHT3xDIS::SHT3xDIS(uint8_t address) : I2CAddress(address) {}

void SHT3xDIS::readRawMeasurements() {
    uint8_t data[6];
    etl::array<float, 2> measurements = {};
    uint8_t ackData = 0;
    uint8_t command[2] = {DISABLED, HIGH_DISABLED};

    if (TWIHS2_Write(I2CAddress, &ackData, 1)) {
        waitForResponse();
        TWIHS_ERROR error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(I2CAddress, command, 4)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    if (TWIHS2_Read(I2CAddress, data, 6)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    rawTemperature = (static_cast<uint16_t>(data[0]) << 8) | (data[1] & 0xff);
    rawHumidity = (static_cast<uint16_t>(data[3]) << 8) | (data[4] & 0xff);
}

float SHT3xDIS::getTemperature() const {
    return -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
}

float SHT3xDIS::getHumidity() const {
    return 100 * (static_cast<float>(rawHumidity) / 0xFFFF);
}
