#include <etl/array.h>
#include <cstdint>

class SHT3xDIS {
private:

    /**
     * I2C device address.
     */
    uint8_t I2CAddress;

    /**
     * I2C transaction error
     */
    TWIHS_ERROR error;

    /**
	 * Transforms the raw temperature that is measured into the physical (result in %).
	 */
    inline float convertTemperature(uint16_t temperature) {
        return 175 * (static_cast<float>(temperature) / 65535) - 45;
    }

    /**
     * Transforms the relative humidity that is measured into the physical (result in degrees Celsius).
     */
    inline float convertHumidity(uint16_t relativeHumidity) {
        return 100 * (static_cast<float>(relativeHumidity) / 65535);
    }

public:
    /**
    * All the available commands for the single shot mode.
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
    enum Heater {
        HEATER_ENABLED = 0x306D,
        HEATER_DISABLED = 0x3066
    };

    /**
     * All commands for the status register.
     */
    enum StatusRegister {
        READ_STATUS_REGISTER = 0xF32D,
        CLEAR_STATUS_REGISTER = 0x3041
    };

    SHT3xDIS(uint8_t address);

    /**
     * Reads the measurements given by the SHT3xDIS sensor.
     * @return an array containing the temperature and humidity measured.
     */
    etl::array<float, 2> readMeasurements();

    /**
     * Writes a command to register so that it starts the measurement
     */
    void writeCommand(uint16_t command);

    /**
     * Sets the type of measurement the sensor will execute.
     */
    void setMeasurement(Measurement command);

    /**
     * Sets the heater (On/Off).
     */
    void setHeater(Heater command);

    /**
     * Writes a command to the Status register so it reads from it or clears it.
     */
    void setStatusRegisterCommand(StatusRegister command);

    /**
     * Reads the data sent by the status register.
     */
    etl::array<uint16_t, 2> readStatusRegister();

    /**
     * Performs a soft reset to the sensor.
     */
    void setSoftReset();

    /**
     * Performs a general call reset on the whole I2C Bus.
     */
    void generalCallReset();

    /**
     * Implementation of CRC8 algorithm, parameters are set according to the manual (paragraph 4.12).
     *
     * Polynomial: 0x31 (x^8 + x^5 + x^4 + 1)
     * Initialization: 0xFF
     * Reflect input: False
     * Reflect output: False
     * Final XOR: 0x00
     */
    bool crc8(uint8_t msb, uint8_t lsb, uint8_t checksum);
}