#pragma once

#include <etl/array.h>
#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.h"

/**
 * The SHT3xDIS_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71Q21B MCU will be used.
 * By giving the corresponding value to SHT3xDIS_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 * If experimenting with the SAM V71 XPLAINED ULTRA EVALUATION KIT, TW0 on the J500 connector or TW2 on the J505 connector can be used.
 */

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

/**
 * Driver for the SHT3x-DIS family of Humidity and Temperature Sensors.
 * This is a driver to operate a SHT3x-DIS sensor with the ATSAMV71Q21B microcontroller. All Microchip-specific
 * functions are used solely within the core read and write functions.
 *
 * For more details about the operation of the sensor, see the datasheet found at:
 * https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf
 */
class SHT3xDIS {
public:
    /**
     * User defined I2C address.
     * I2C_ADDRESS_A : ADDR (pin 2) is connected to logic low (default)
     * I2C_ADDRESS_B : ADDR (pin 2) is connected to logic high
     */
    enum SHT3xDIS_I2C_Address : uint8_t {
        I2C_ADDRESS_A = 0x44,
        I2C_ADDRESS_B = 0x45
    };

private:
    /**
    * Wait period before a sensor read is skipped
    */
    constexpr uint8_t TimeoutTicks = 100;

    /**
     * I2C device address
     */
    constexpr SHT3xDIS_I2C_Address I2CAddress = 0x00;

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
     * Function that prevents hanging when a I2C device is not responding
    */
    inline void waitForResponse() {
        auto start = xTaskGetTickCount();
        while (SHT3xDIS_IsBusy()) {
            if (xTaskGetTickCount() - start > TimeoutTicks) {
                LOG_ERROR << "Humidity-Temperature sensor with address " << I2CAddress
                          << " has timed out";
                SHT3xDIS_Initialize();
            }
            taskYIELD();
        }
    };

    /**
     * All the available commands for the single shot mode
     */
    enum Repeatability : uint8_t {
        HIGH_ENABLED = 0x06,
        MEDIUM_ENABLED = 0x0D,
        LOW_ENABLED = 0x10,
        HIGH_DISABLED = 0x00,
        MEDIUM_DISABLED = 0x0B,
        LOW_DISABLED = 0x16
    };

    /**
     *
     */
    enum ClockStretching : uint8_t {
        ENABLED = 0x2C,
        DISABLED = 0x24
    };

    /**
     * Commands for Heater
     */
    enum Heater : uint16_t {
        HEATER_ENABLED = 0x306D,
        HEATER_DISABLED = 0x3066
    };

    /**
     * All commands for the status register.
     */
    enum StatusRegister : uint16_t {
        READ_STATUS_REGISTER = 0xF32D,
        CLEAR_STATUS_REGISTER = 0x3041
    };

public:
    /**
     *
     * @param i2cUserAddress
     */
    constexpr SHT3xDIS(SHT3xDIS_I2C_Address i2cUserAddress) : I2CAddress(i2cUserAddress) {}

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
