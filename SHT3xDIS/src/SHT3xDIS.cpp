#include "SHT3xDIS.hpp"

void SHT3xDIS::writeCommandtoRegister(Register address_register, uint16_t command) {
    uint8_t txData[] = {address_register, static_cast<uint8_t>(command >> 8), static_cast<uint8_t>(command & 0x00FF)};
    uint8_t ackData = 0;

    if (TWIHS2_Write(address_register, &ackData, 1)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(address_register, txData, 3)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }
}

etl::array<float, 2> SHT3xDIS::readMeasurements(Register address_register) {
    uint8_t data[6];
    etl::array<float, 2> measurements = {};
    uint16_t rawTemperature;
    uint16_t rawHumidity; 

    if (TWIHS2_Read(address_register, data, 6)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
        
        if (crc8(data[0], data[1], data[2]) && crc8(data[3], data[4], data[5])) {
            rawTemperature = (data[0]<<8) | data[1];
            rawHumidity = (data[3]<<8) | data[4];

            measurements[0] = temperatureConversion(rawTemperature);
            measurements[1] = humidityConversion(rawHumidity);
        }
    }
    return measurements;
}

void setMeasurement(Register address_register, SHT3xDIS::Measurement command) {
    writeCommandtoRegister(address_register, command);
}

void setHeater(Register address_register, SHT3xDIS::Heater command) {
    writeCommandtoRegister(address_register, command);
}

void setStatusRegisterCommand(Register address_register, SHT3xDIS::StatusRegister command) {
    writeCommandtoRegister(address_register, command);
}

etl::array<uint16_t, 2> SHT3xDIS::readStatusRegister(Register address_register) {
    uint8_t data[4];
    etl::array<uint16_t, 2> status = {}; 

    if (TWIHS2_Read(address_register, data, 4)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();

        if (crc8(data[1], data[2], data[3])) {
            status[0] = data[1];
            status[1] = data[2];
        }
    }
    return status;
}

void setSoftReset(Register address_register) {
    uint16_t softReset = 0x30A2;
    writeCommandtoRegister(address_register, softReset);
}

void SHT3xDIS::hardReset() {
    resetCommand = 0x0006;

    uint8_t txData[] = {static_cast<uint8_t>(resetCommand >> 8), static_cast<uint8_t>(resetCommand & 0x00FF)};
    uint8_t ackData = 0;

    if (TWIHS2_Write(address_register, &ackData, 1)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(address_register, txData, 2)) {
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