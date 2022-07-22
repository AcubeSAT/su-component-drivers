#include "SHT3xDIS.hpp"

void SHT3xDIS::writeCommandtoRegister(Register register, uint16_t command){
    uint8_t txData[] = { register, static_cast<uint8_t>(command >> 8), static_cast<uint8_t>(command & 0x00FF) };
    uint8_t ackData = 0;

    if (TWIHS2_Write(register, &ackData, 1)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(register, txData, 3)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }
}

etl::array<float, 2> SHT3xDIS::readMeasurements(Register register){
    uint8_t data[6];
    etl::array<float,2> measurements = {};
    uint16_t rawTemperature;
    uint16_t rawHumidity; 

    if (TWIHS2_Read(register, data, 6)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
        
        rawTemperature = (data[0]<<8) | data[1];
        rawHumidity = (data[3]<<8) | data[4];

        measurements[0] = getTemperature(rawTemperature);
        measurements[1] = getHumidity(rawHumidity);
    }
    return measurements;
}

void setMeasurement(Register register, SHT3xDIS::Measurement command){
    writeCommandtoRegister(register, command);
}

void setHeater(Register register, SHT3xDIS::Heater command){
    writeCommandtoRegister(register, command);
}

void setStatusRegisterCommand(Register register, SHT3xDIS::StatusRegister command){
    writeCommandtoRegister(register, command);
}

etl::array<uint16_t, 2> SHT3xDIS::readStatusRegister(Register register){
    uint8_t data[4];
    etl::array<uint16_t, 2> status = {}; 

    if (TWIHS2_Read(register, data, 4)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();

        status[0] = data[1];
        status[1] = data[2];
    }
    return status;
}

void setSoftReset(Register register){
    uint16_t softReset = 0x30A2;
    writeCommandtoRegister(register, softReset);
}

void SHT3xDIS::HardReset(){
    resetCommand = 0x0006;

    uint8_t txData[] = { static_cast<uint8_t>(resetCommand >> 8), static_cast<uint8_t>(resetCommand & 0x00FF) };
    uint8_t ackData = 0;

    if (TWIHS2_Write(register, &ackData, 1)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(register, txData, 2)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }
}