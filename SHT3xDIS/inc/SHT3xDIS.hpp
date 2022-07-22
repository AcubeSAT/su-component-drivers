#include <etl/array.h>

class SHT3xDIS {
private:
	/**
	 * Every register address.
	 */
	enum Register{
		I2CAddr1 = 0x44, 
 		I2CAddr2 = 0x45	
 	};

 	/**
	 * All the available commands for the single shoot mode.
	 */
	enum Measurement {
	     HIGH_ENABLED = 0x2C06, 
	     MEDIUM_ENABLED = 0x2C0D,  
	     LOW_ENABLED = 0x2C10,
	     HIGH_DISABLED = 0x2400, 
	     MEDIUM_DISABLED = 0x240B,  
	     LOW_DISABLED = 0x2416   
	};

	/**
	 * Commands for Heater
	 */
	enum Heater{
		HEATER_ENABLED = 0x306D,
		HEATER_DISABLED = 0x3066
	};

	/**
	 * All commands gor status register.
	 */
	enum StatusRegister
	{
		READ_STATUS_REGISTER = 0xF32D,
		CLEAR_STATUS_REGISTER = 0x3041
	};

	/**
     * High Speed Two-Wired Interface transaction error
     */
    TWIHS_ERROR error;

public:
	/**
	 * Reads the measurments given by the SHT3xDIS sensor.
	 * @return an array containg the temperature and humidity measured.
	 */
	etl::array<float,2> readMeasurements(Register register);

	/**
	 * Writes a command to register so that it starts the measurement
	 */
	void writeCommandtoRegister(Register register, uint16_t command);

	/**
	 * Sets the type of measurement the sensor will execute.
	 */
	void setMeasurement(Register register, Measurement command);

	/**
	 * Sets the heater(On/Off).
	 */
	void setHeater(Register register, Heater command);

	/**
	 * Writs a command to the Status register so it reads from it or clears it.
	 */
	void setStatusRegisterCommand(Register register, StatusRegister command);

	/**
	 * Reads the data sent by the status register.
	 */
	etl::array<uint16_t, 2> readStatusRegister(Register register);

	/**
	 * Performs a soft reset to the sensor.
	 */
	void setSoftReset(Register register);

	/**
	 * Performs a hard reset on the whole I2C Bus.
	 */
	void HardReset();

	/**
	 * Tranforms the raw temperature that is measured into the physical.
	 */
	inline float getTemperature(uint16_t temperature){
		return  175 * (temperature / 65535) - 45;
	}

	/**
	 * Tranforms the reletive humidity that is measured into the physical.
	 */
	inline float getHumidity(uint16_t relativeHumidity){
		return 100 * (relativeHumidity / 65535);
	}
}