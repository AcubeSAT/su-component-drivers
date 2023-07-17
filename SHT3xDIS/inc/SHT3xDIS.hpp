#pragma once

#include <etl/utility.h>
#include <etl/array.h>
#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

/**
 * The SHT3xDIS_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71 MCU will be used.
 * By giving the corresponding value to SHT3xDIS_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 */
#if SHT3xDIS_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define SHT3xDIS_TWIHS_WriteRead TWIHS0_WriteRead
#define SHT3xDIS_TWIHS_Write TWIHS0_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS0_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS0_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS0_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS0_IsBusy

#elif SHT3xDIS_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define SHT3xDIS_TWIHS_WriteRead TWIHS1_WriteRead
#define SHT3xDIS_TWIHS_Write TWIHS1_Write
#define SHT3xDIS_TWIHS_ErrorGet TWIHS1_ErrorGet
#define SHT3xDIS_TWIHS_Read TWIHS1_Read
#define SHT3xDIS_TWIHS_Initialize TWIHS1_Initialize
#define SHT3xDIS_TWIHS_IsBusy TWIHS1_IsBusy

#elif SHT3xDIS_TWI_PORT == 2

#include "plib_twihs2_master.h"

#define SHT3xDIS_TWIHS_WriteRead TWIHS2_WriteRead
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

    /**
     * Control commands for the single shot mode. The commands are in the form
     * Clock-StretchingConfiguration_RepeatabilityConfiguration
     * i.e ENABLED_HIGH means Clock-Stretching Enabled, Repeatability High
     */
    enum SingleShotModeCommands : uint16_t {
        ENABLED_HIGH = 0x2C06, ///> not yet implemented
        ENABLED_MEDIUM = 0x2C0D, ///> not yet implemented
        ENABLED_LOW = 0x2C10, ///> not yet implemented
        DISABLED_HIGH = 0x2400,
        DISABLED_MEDIUM = 0x240B,
        DISABLED_LOW = 0x2416
    };

private:
    /**
     * I2C device address.
     */
    const SHT3xDIS_I2C_Address I2CAddress;

    /**
     * GPIO pin connected to the nRESET pin of the sensor
     * @Note the GPIO pin must be programmed as output, active low
     */
    const PIO_PIN NResetPin = PIO_PIN_NONE;

    /**
     * GPIO connected to the Alert pin of the sensor
     * @Note the GPIO pin must be programmed as input
     */
    const PIO_PIN AlertPin = PIO_PIN_NONE;

    /**
     * Milliseconds to wait for the sensor measurements to be completed in Single-shot Mode or a sensor reset to complete.
     * This value was chosen arbitrarily and seems to be working, it is not stated in the datasheet.
     */
    static inline constexpr uint8_t msToWait = 20;

    /**
     * The number of bytes a command consists of.
     */
    static inline constexpr uint8_t NumberOfBytesInCommand = 2;

    /**
     * The number of bytes that are sent from the sensor after sending the command to read the Status Register.
     * The Status Register consists of 2 bytes plus 1 CRC byte.
     */
    static inline constexpr uint8_t NumberOfBytesOfStatusRegisterWithCRC = 3;

    /**
     * The number of bytes hat are sent from the sensor after sending the command for Single-Shot Mode.
     * The data consist of 6 bytes, 2 raw sensor data bytes for each physical measurement plus 1 byte each for the checksum.
     */
    static inline constexpr uint8_t NumberOfBytesOfMeasurementsWithCRC = 6;

    /**
     * Variable to select the between using or not the checksum for the sensor data.
     */
    static inline constexpr bool UseCRC = true;

    /**
     * Wait period before for abandoning an I2C transaction because the send/receive buffer does not get unloaded/gets loaded.
     */
    static inline constexpr uint16_t TimeoutTicks = 1000;

    /**
     * Control commands for the Heater.
     */
    enum HeaterCommands : uint16_t {
        ENABLE = 0x306D,
        DISABLE = 0x3066
    };

    /**
     * Control commands for the Status register.
     */
    enum StatusRegisterCommands : uint16_t {
        READ = 0xF32D,
        CLEAR = 0x3041
    };

    enum ResetSensorCommands : uint16_t {
        INTERFACE_RESET = 0x00,
        SOFT_RESET = 0x30A2,
        GENERAL_CALL_RESET = 0x0006
    };

    /**
     * Function that prevents hanging when a I2C device is not responding.
     */
    inline void waitForI2CBuffer() const {
        auto start = xTaskGetTickCount();
        while (SHT3xDIS_TWIHS_IsBusy()) {
            if (xTaskGetTickCount() - start > TimeoutTicks) {
                LOG_ERROR << "Humidity sensor with address " << I2CAddress << " , communication has timed out";
                SHT3xDIS_TWIHS_Initialize();
                break;
            }
        }
    };

    /**
     * Implementation of CRC8 algorithm, parameters are set according to the manual (paragraph 4.12).
     * Polynomial: 0x31 (x^8 + x^5 + x^4 + 1)
     * Initialization: 0xFF
     * Reflect input: False
     * Reflect output: False
     * Final XOR: 0x00
     */
    static bool crc8(uint8_t msb, uint8_t lsb, uint8_t checksum);

    /**
     * Lowers the SDA line to begin a TWIHS transaction
     */
    void wakeUpDevice();

    /**
     * An abstraction layer function that is the only one that interacts with the HAL. Executes one of the Read, Write or ReadWrite functions of
     * the HAL with the correct number of parameters.
     * @tparam F function template
     * @tparam Arguments template for arbitrary number of arguments
     * @param i2cFunction the HAL I2C function to execute
     * @param arguments the arguments for the I2C function
     * @return true if transaction was successful, false otherwise
     */
    template<typename F, typename... Arguments>
    bool executeI2CTransaction(F i2cFunction, Arguments... arguments);

    /**
     * Function that prevents hanging when a I2C device is not responding.
     */
    void checkForNACK();

    /**
     * Sends a command to the sensor.
     * @param command can be one of the command enum types
     */
    void sendCommandToSensor(uint16_t command);

    /**
     * Executes a continues Write-Read Transaction with a Repeated start condition as it is required for the Status
     * Register commands
     * @param statusRegisterData the Status Register data as a byte array
     */
    void executeWriteReadTransaction(etl::array<uint8_t, NumberOfBytesOfStatusRegisterWithCRC> &statusRegisterData);

    /**
     * Attempts to read temperature and humidity data the sensor measured. 6 bytes are to be read in total, 2 raw
     * sensor data bytes for each physical measurement plus 1 byte each for the checksum.
     * @param sensorData
     */
    void readSensorDataSingleShotMode(etl::array<uint8_t, NumberOfBytesOfMeasurementsWithCRC> &sensorData);

    /**
     * Converts the raw temperature data to the physical scale according to the section 4.13 of the datasheet.
     * @Note Negative values are converted properly
     * @param rawTemperature raw temperature data as received from the sensor
     * @return temperature in Celsius
     */
    [[nodiscard]] static inline float convertRawTemperatureValueToPhysicalScale(uint16_t rawTemperature) {
        return -45 + 175 * (static_cast<float>(rawTemperature) / 0xFFFF);
    }

    /**
     * Converts the raw humidity data to the physical scale according to the section 4.13 of the datasheet.
     * @param rawHumidity raw humidity data as received from the sensor
     * @return humidity in Relative humidity %
     */
    [[nodiscard]] static inline float convertRawHumidityValueToPhysicalScale(uint16_t rawHumidity) {
        return 100 * (static_cast<float>(rawHumidity) / 0xFFFF);
    }

    /**
     * Create 16-bit data by concatenating 2 bytes
     * @param msb the first 8 bits of the half word
     * @param lsb the last 8 bits of the half word
     * @return the half word
     */
    [[nodiscard]] static inline uint16_t concatenateTwoBytesToHalfWord(uint8_t msb, uint8_t lsb) {
        return ((static_cast<uint16_t>(msb) << 8) & 0xFF00) | ((static_cast<uint16_t>(lsb) & 0xFF));
    }

    /**
     * Split 16-bit data into a byte-array in order to transmit them through the HAL I2C functions
     * @param dataArray the byte-array passed as parameter to the HAL functions
     * @param halfWord the 16-bit data to split
     */
    static inline void
    splitHalfWordToByteArray(etl::array<uint8_t, NumberOfBytesInCommand> &dataArray, uint16_t halfWord) {
        dataArray = {static_cast<uint8_t>((halfWord >> 8) & 0xFF), static_cast<uint8_t>(halfWord & 0xFF)};
    }

    /**
     * Initialize the sensor by clearing the Status Register and pulling the nRESET pin High.
     */
    inline void initializeSensor() {
//        clearStatusRegister();
//        PIO_PinWrite(NResetPin, true);
    }


public:
    /**
     * Constructor used for initializing the sensor I2C address
     * @param i2cUserAddress the I2C address. Must be of type SHT3xDIS_I2C_Address
     * @param nResetPin the GPIO of the MCU connected to the nRESET pin (pin 6) of the sensor
     * @param alertPin the GPIO of the MCU connected to the Alert pin (pin 3) of the sensor
     */
    explicit SHT3xDIS(SHT3xDIS_I2C_Address i2cUserAddress, PIO_PIN nResetPin = PIO_PIN_NONE,
                      PIO_PIN alertPin = PIO_PIN_NONE) :
            I2CAddress(i2cUserAddress), NResetPin(nResetPin), AlertPin(alertPin) {
        initializeSensor();
    }

    /**
     * Get temperature and humidity data from the sensor in physical scale with the Single Shot Data Acquisition Mode.
     * @param command the Single Shot sensor measurement configuration, @see SingleShotModeCommands
     * @return a pair of float types. The first is the temperature and second one is the humidity
     */
    etl::pair<float, float> getOneShotMeasurement(SingleShotModeCommands command);

    /**
     * Sets the heater (On/Off).
     * @param command the command to either turn the heater off or on
     */
    void setHeater(HeaterCommands command);

    /**
     * Clears the Status Register.
     */
    void clearStatusRegister();

    /**
     * Reads the Status Register.
     * @return the 16-bits of the Status Register
     */
    uint16_t readStatusRegister();

    /**
     * Soft resets the sensor.
     */
    void performSoftReset();
};
