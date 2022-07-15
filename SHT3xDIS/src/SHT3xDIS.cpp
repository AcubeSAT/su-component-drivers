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

void SHT3xDIS::readMeasurments(){
    //more on this tommorow
}

float SHT3xDIS::getTemperature(float ST)
{
	return  175 * (ST / 65535) - 45;
}

float SHT3xDIS::getHumidity(float SRH)
{
	return 100 * (SRH / 65535);
}