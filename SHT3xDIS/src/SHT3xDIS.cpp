#include "SHT3xDIS.hpp"

void SHT3xDIS::writeRegister(Address address, uint16_t data){
    uint8_t txData[] = {
            address,
            static_cast<uint8_t>(data >> 8),
            static_cast<uint8_t>(data & 0x00FF)
    };

    uint8_t ackData = 0;

    if (TWIHS2_Write(address, &ackData, 1)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }

    if (TWIHS2_Write(address, txData, 1)) {
    while (TWIHS2_IsBusy());
    error = TWIHS2_ErrorGet();
    }
}

uint16_t* SHT3xDIS::readMeasurments(Address address){
    uint8_t data[6];
    uint16_t measurments[2];
    uint8_t ackData = 0;
    uint16_t rawTemperature;
    uint16_t rawHumidity; 

    //intializing the connection, not sure if its necessary
    if (TWIHS2_Write(address, &ackData, 1)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
    }

    //reading the measurments
    if (TWIHS2_Read(address, data, 6)) {
        while (TWIHS2_IsBusy());
        error = TWIHS2_ErrorGet();
        
        rawTemperature = (data[0]<<8) | data[1];
        rawHumidity = (data[3]<<8) | data[4];

    } else {
        return 0;
    }

    measurments[0] = getTemperature(rawTemperature);
    measurments[1] = getHumidity(rawHumidity);

    return measurments;

}

uint16_t SHT3xDIS::getTemperature(float ST)
{
	return  175 * (ST / 65535) - 45;
}

uint16_t SHT3xDIS::getHumidity(float SRH)
{
	return 100 * (SRH / 65535);
}