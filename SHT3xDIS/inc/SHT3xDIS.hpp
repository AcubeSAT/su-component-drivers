#include <etl/array.h>

class SHT3xDIS {
private:
	/**
	 * Register addresses are stored here.
	 */
	enum Register{
		I2CAddr1 = 0x44, 
 		I2CAddr2 = 0x45	
 	};

 	/**
	 * These are all the available commands for the single shoot mode.
	 */
	enum Command {
	     HIGH_ENABLED = 0x2C06, 
	     MEDIUM_ENABLED = 0x2C0D,  
	     LOW_ENABLED = 0x2C10,
	     HIGH_DISABLED = 0x2400, 
	     MEDIUM_DISABLED = 0x240B,  
	     LOW_DISABLED = 0x2416 
	     softReset = 0x30A2;   
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
	void writeCommandtoRegister(Register register, Command command);

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