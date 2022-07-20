#include "SHT3xDIS.hpp"

void SHT3xDIS::writeCommandtoRegister(Register register, Command command){
    uint8_t txData[] = {
            register,
            static_cast<uint8_t>(command >> 8),
            static_cast<uint8_t>(command & 0x00FF)
    };

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

    } else {
        return 0;
    }

    measurements[0] = getTemperature(rawTemperature);
    measurements[1] = getHumidity(rawHumidity);

    return measurements;
}