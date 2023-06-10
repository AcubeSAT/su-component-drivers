#include "SHT3xDIS.hpp"

SHT3xDIS::SHT3xDIS(uint8_t address) : I2CAddress(address) {}

void SHT3xDIS::writeCommand(uint16_t command) {
    uint8_t txData[] = {I2CAddress,
                        static_cast<uint8_t>(command >> 8),
                        static_cast<uint8_t>(command & 0x00FF)};
    uint8_t ackData = 0;

    if (TWIHS2_Write(I2CAddress, &ackData, 1)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(I2CAddress, txData, 3)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }
}

void SHT3xDIS::readRawMeasurements() {
    uint8_t data[6];
    etl::array<float, 2> measurements = {};
    uint8_t ackData = 0;
    uint8_t command[2] = {DISABLED, HIGH_DISABLED};

    TWIHS2_Write(I2CAddress, &ackData, 1);
    while (TWIHS2_IsBusy());
    TWIHS_ERROR error = TWIHS2_ErrorGet();

    TWIHS2_Write(I2CAddress, command, 4);
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();

    vTaskDelay(pdMS_TO_TICKS(20));
    if (TWIHS2_Read(I2CAddress, data, 6));
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();

    rawTemperature = (data[0] << 8) | (data[1] & 0xff);
    rawHumidity = (data[3] << 8) | (data[4] & 0xff);
}

float SHT3xDIS::getTemperature() {
    return -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
}

float SHT3xDIS::getHumidity() {
    return 100 * (static_cast<float>(rawHumidity) / 0xFFFF);
}

void SHT3xDIS::setRepeatability(SHT3xDIS::Repeatability command) {
    writeCommand(command);
}

void SHT3xDIS::setHeater(SHT3xDIS::Heater command) {
    writeCommand(command);
}

void SHT3xDIS::setStatusRegisterCommand(SHT3xDIS::StatusRegister command) {
    writeCommand(command);
}

etl::array<uint16_t, 2> SHT3xDIS::readStatusRegister() {
    uint8_t data[4];
    etl::array<uint16_t, 2> status = {};

    if (TWIHS2_Read(I2CAddress, data, 4)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();

        if (crc8(data[1], data[2], data[3])) {
            status[0] = data[1];
            status[1] = data[2];
        }
    }
    return status;
}

void SHT3xDIS::setSoftReset() {
    uint16_t softReset = 0x30A2;
    writeCommand(softReset);
}

void SHT3xDIS::generalCallReset() {
    uint16_t resetCommand = 0x0006;

    uint8_t txData[] = {static_cast<uint8_t>(resetCommand >> 8),
                        static_cast<uint8_t>(resetCommand & 0x00FF)};
    uint8_t ackData = 0;

    if (TWIHS2_Write(I2CAddress, &ackData, 1)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(I2CAddress, txData, 2)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }
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