#pragma once

#include <etl/array.h>
#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.h"

#if SHT3xDIS_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define SHT3xDIS_Write TWIHS0_Write
#define SHT3xDIS_ErrorGet TWIHS0_ErrorGet
#define SHT3xDIS_Read TWIHS0_Read
#define SHT3xDIS_Initialize TWIHS0_Initialize
#define SHT3xDIS_IsBusy TWIHS0_IsBusy

#elif SHT3xDIS_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define SHT3xDIS_TWIHS_Write TWIHS1_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS1_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS1_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS1_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS1_IsBusy

#elif SHT3xDIS_TWI_PORT == 2

#include "plib_twihs2_master.h"

#define SHT3xDIS_TWIHS_Write TWIHS2_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS2_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS2_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS2_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS2_IsBusy
#endif


class SHT3xDIS {
private:
    /**
    * Wait period before a sensor read is skipped
    */
    const uint8_t TimeoutTicks = 100;

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

    /**
     * Function that prevents hanging when a I2C device is not responding.
        */
    inline void waitForResponse() {
        auto start = xTaskGetTickCount();
        while (TWIHS_IsBusy()) {
            if (xTaskGetTickCount() - start > TimeoutTicks) {
                LOG_ERROR << "Temperature/Humidity sensor with address " << I2CAddress
                          << " has timed out";
                TWIHS_Initialize();
            }
            taskYIELD();
        }
    };

public:
    /**
    * All the available commands for the single shot mode.
    */
    enum Repeatability : uint8_t {
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
     * Reads the measurements given by the SHT3x-DIS sensor.
     * @return an array containing the temperature and humidity measured.
     */
    void readRawMeasurements();

    /**
     * Transforms the raw temperature value to a real value and returns it.
     * @return the real temperature measured in Celsius.
     */
    float getTemperature() const;

    /**
     * Transforms the raw humidity value to a real value and returns it.
     * @return the real temperature as a percentage.
     */
    float getHumidity() const;

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
