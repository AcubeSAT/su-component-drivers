#pragma once

#include <etl/array.h>
#include <cstdint>
#include "plib_twihs2_master.h"
#include "plib_systick.h"
#include "FreeRTOS.h"
#include "task.h"

class SHT3xDIS {
private:

    /**
     * I2C device address.
     */
    const uint8_t I2CAddress;

    /**
     * I2C transaction error
     */
    TWIHS_ERROR error;

    /**
     * Raw temperature read from the device
     */
    uint16_t rawTemperature;

    /**
     * Raw humidity read from the device
     */
    uint16_t rawHumidity;

public:
    /**
    * All the available commands for the single shot mode.
    */
    enum Repeatability {
        HIGH_ENABLED = 0x06,
        MEDIUM_ENABLED = 0x0D,
        LOW_ENABLED = 0x10,
        HIGH_DISABLED = 0x00,
        MEDIUM_DISABLED = 0x0B,
        LOW_DISABLED = 0x16
    };

    enum ClockStretching {
        ENABLED = 0x2C,
        DISABLED = 0x24
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
    void readRawMeasurements();

    /**
     * Transforms the raw temperature value to a real value and returns it.
     * @return the real temperature measured in Celsius.
     */
    float getTemperature();

    /**
     * Transforms the raw humidity value to a real value and returns it.
     * @return the real temperature as a percentage.
     */
    float getHumidity();

    /**
     * Writes a command to register so that it starts the measurement
     */
    void writeCommand(uint16_t command);

    /**
     * Sets the type of measurement the sensor will execute.
     */
    void setRepeatability(Repeatability command);

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
};
