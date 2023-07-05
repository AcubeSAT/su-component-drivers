#pragma once

#include <etl/array.h>
#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.h"

/**
 * The SHT3xDIS_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71 MCU will be used.
 * By giving the corresponding value to SHT3xDIS_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 */

#if SHT3xDIS_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define SHT3xDIS_WriteRead TWIHS0_WriteRead
#define SHT3xDIS_Write TWIHS0_Write
#define SHT3xDIS_ErrorGet TWIHS0_ErrorGet
#define SHT3xDIS_Read TWIHS0_Read
#define SHT3xDIS_Initialize TWIHS0_Initialize
#define SHT3xDIS_IsBusy TWIHS0_IsBusy

#elif SHT3xDIS_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define SHT3xDIS_WriteRead TWIHS1_WriteRead
#define SHT3xDIS_TWIHS_Write TWIHS1_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS1_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS1_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS1_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS1_IsBusy

#elif SHT3xDIS_TWI_PORT == 2

#include "plib_twihs2_master.h"
#define SHT3xDIS_WriteRead TWIHS2_WriteRead
#define SHT3xDIS_TWIHS_Write TWIHS2_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS2_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS2_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS2_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS2_IsBusy
#endif

/**
 * Driver for the SHT3x-DIS family of Humidity and Temperature Sensors.
 * This is a driver to operate a SHT3x-DIS sensor with the ATSAMV71 microcontroller. All Microchip-specific
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
    * Milliseconds to wait for the sensor measurements to be completed in Single-shot Mode
    */
    inline constexpr uint8_t msToWait = 10;

    /**
     * I2C device address
     */
    constexpr SHT3xDIS_I2C_Address I2CAddress = 0x00;

    /**
     *
     */
    inline constexpr bool UseCRC = true;

    /**
     * Control commands for the single shot mode. The commands are in the form
     * Clock-StretchingConfiguration_RepeatabilityConfiguration
     * i.e ENABLED_HIGH means Clock-Stretching Enabled, Repeatability High
     */
    enum class SingleShotModeCommands : uint16_t {
        ENABLED_HIGH = 0x2C06, ///> not yet implemented
        ENABLED_MEDIUM = 0x2C0D, ///> not yet implemented
        ENABLED_LOW = 0x2C10, ///> not yet implemented
        DISABLED_HIGH = 0x2400,
        DISABLED_MEDIUM = 0x240B,
        DISABLED_LOW = 0x2416
    };

    /**
     * Control commands for the Heater
     */
    enum class HeaterCommands : uint16_t {
        ENABLE = 0x306D,
        DISABLE = 0x3066
    };

    /**
     * Control commands for the Status register
     */
    enum class StatusRegisterCommands : uint16_t {
        READ = 0xF32D,
        CLEAR = 0x3041
    };

    /**
     * Function that prevents hanging when a I2C device is not responding
     */
    inline void waitForResponse() {
        while (not SHT3xDIS_Read(I2CAddress, nullptr, 0)) { // use if instead of while
            while (SHT3xDIS_IsBusy()) {}

            if (SHT3xDIS_ErrorGet() == TWIHS_ERROR_NACK) {
                LOG_ERROR << "Humidity-Temperature sensor with address " << I2CAddress << " was not found";
                vTaskSuspend();
            }
        }
    }
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

    /**
     * Writes a command to register so that it starts the measurement
     * @param command
     */
    void sendCommandToSensor(uint16_t command);

    /**
     *
     * @param dataToWrite
     * @param numberOfdataToWrite
     * @param dataToRead
     * @param numberOfdataToRead
     */
    void executeWriteReadTransaction(i2cAddress, uint8_t bytesToWrite, uint8_t numberOfBytesToWrite, uint8_t bytesToRead, uint8_t numberOfBytesToRead);

    /**
     *
     */
    void readSensorDataSingleShotMode();

    /**
     *
     * @Note Negative values are converted properly
     * @param rawTemperature
     * @return
     */
    static inline float convertRawTemperatureValueToPhysicalScale(uint16_t rawTemperature) {
        return -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
    }

    /**
     *
     * @param rawHumidity
     * @return
     */
    static inline float convertRawHumidityValueToPhysicalScale(uint16_t rawHumidity) {
        return 100 * (static_cast<float>(rawHumidity) / 0xFFFF);
    }

    /**
     *
     * @param msb
     * @param lsb
     * @return
     */
    static inline uint16_t convertBytesToHalfWord(uint8_t msb, uint8_t lsb) {
        return (static_cast<uint16_t>(msb) << 8) | (lsb & 0xFF);
    }

public:
    /**
     *
     * @param i2cUserAddress
     */
    constexpr SHT3xDIS(SHT3xDIS_I2C_Address i2cUserAddress) : I2CAddress(i2cUserAddress) {}

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
     * Read the status register.
     */
    uint16_t readStatusRegister();

    /**
     * Performs a soft reset to the sensor.
     */
    void performSoftReset();

    /**
     * Performs a general call reset on the whole I2C Bus.
     */
    void generalCallReset();

};
