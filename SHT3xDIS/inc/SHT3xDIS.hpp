#include <iosteam>
#ifndef TEST_INTEGRATION_SHT3XDIS_HPP
#define TEST_INTEGRATION_SHT3XDIS_HPP


class SHT3xDIS {
public:
	enum Address{
		I2C_ADDR_1 = 0x44, 
 		I2C_ADDR_2 = 0x45	
 	};


	enum condition {
	     HIGH_ENABLED = 0x2C06, 
	     MEDIUM_ENABLED = 0x2C0D,  
	     LOW_ENABLED = 0x2C10,
	     HIGH_DISABLED = 0x2400, 
	     MEDIUM_DISABLED = 0x240B,  
	     LOW_DISABLED = 0x2416    
	};
	
	void readMeasurments(uint8_t address);

	void writeRegister(Address address, uint16_t data);

	float getTemperature(float);

	float getHumidity(float);

}


#endif //TEST_INTEGRATION_SHT3XDIR_HPP