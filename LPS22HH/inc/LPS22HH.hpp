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
 * The LPS22HH_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71 MCU will be used.
 * By giving the corresponding value to LPS22HH_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 */
#if LPS22HH_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define LPS22HH_TWIHS_WriteRead TWIHS0_WriteRead
#define LPS22HH_TWIHS_Write TWIHS0_Write
#define LPS22HH_TWIHS_ErrorGet TWIHS0_ErrorGet
#define LPS22HH_TWIHS_Read TWIHS0_Read
#define LPS22HH_TWIHS_Initialize TWIHS0_Initialize
#define LPS22HH_TWIHS_IsBusy TWIHS0_IsBusy

#elif LPS22HH_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define LPS22HH_TWIHS_WriteRead TWIHS1_WriteRead
#define LPS22HH_TWIHS_Write TWIHS1_Write
#define LPS22HH_TWIHS_ErrorGet TWIHS1_ErrorGet
#define LPS22HH_TWIHS_Read TWIHS1_Read
#define LPS22HH_TWIHS_Initialize TWIHS1_Initialize
#define LPS22HH_TWIHS_IsBusy TWIHS1_IsBusy

#elif LPS22HH_TWI_PORT == 2

#include "plib_twihs2_master.h"
#define LPS22HH_TWIHS_WriteRead TWIHS2_WriteRead
#define LPS22HH_TWIHS_Write TWIHS2_Write
#define LPS22HH_TWIHS_ErrorGet TWIHS2_ErrorGet
#define LPS22HH_TWIHS_Read TWIHS2_Read
#define LPS22HH_TWIHS_Initialize TWIHS2_Initialize
#define LPS22HH_TWIHS_IsBusy TWIHS2_IsBusy

#endif

/**
 * @class LPS22HH
 */
class LPS22HH {
public:
    /**
     * User defined I2C address.
     * I2C_ADDRESS_A : ADDR (pin 2) is connected to logic low (default)
     * I2C_ADDRESS_B : ADDR (pin 2) is connected to logic high
     */
    enum LPS22HH_I2C_Address : uint8_t {
        I2C_ADDRESS_A = 0x5C,
        I2C_ADDRESS_B = 0x5D
    };

    enum FIFOModes : uint8_t {
        BYPASS = 0x00,
        FIFO = 0x01,
        CONTINUOUS = 0x02,
        BYPASS_TO_FIFO = 0x01,
        BYPASS_TO_CONTINUOUS = 0x02,
        CONTINUOUS_TO_FIFO = 0x03,
    };

    /**
    * Register addresses as defined within the datasheet
    */
    enum RegisterAddress : uint8_t {
        CTRL_REG1 = 0x10,
        CTRL_REG2 = 0x11,
        CTRL_REG3 = 0x12,
        FIFO_CTRL = 0x13,
        FIFO_STATUS1 = 0x25,
        FIFO_STATUS2 = 0x26,
        STATUS = 0x27,
        PRESSURE_OUT_XL = 0x28,
        PRESSURE_OUT_L = 0x29,
        PRESSURE_OUT_H = 0x2A,
        TEMP_OUT_L = 0x2B,
        TEMP_OUT_H = 0x2C,
        FIFO_DATA_OUT_PRESS_XL = 0x78,
        FIFO_DATA_OUT_PRESS_L = 0x79,
        FIFO_DATA_OUT_PRESS_H = 0x7A,
        FIFO_DATA_OUT_TEMP_L = 0x7B,
        FIFO_DATA_OUT_TEMP_H = 0x7C,
        WHO_AM_I = 0x0F
    };

    /**
    * The output data rate (Hz) set in Control Register 1
    */
    enum OutputDataRate : uint8_t {
        ONE_SHOT = 0x00,
        HZ1 = 0x01,
        HZ10 = 0x02,
        HZ25 = 0x03,
        HZ50 = 0x04,
        HZ75 = 0x05,
        HZ100 = 0x06,
        HZ200 = 0x07,
    };

    /**
     * Constructor used for initializing the sensor I2C address
     * @param i2cUserAddress the I2C address. Must be of type LPS22HH_I2C_Address
     * @param nResetPin the GPIO of the MCU connected to the nRESET pin (pin 6) of the sensor
     * @param alertPin the GPIO of the MCU connected to the Alert pin (pin 3) of the sensor
     */
    explicit LPS22HH(LPS22HH_I2C_Address i2cUserAddress) : I2CAddress(i2cUserAddress) {
    }

    /**
     * Read the current pressure measurement in hPa.
     * @return the calculated pressure value.
     */
    float readPressure();

    /**
     * Read the current temperature measurement in °C.
     * @return the calculated temperature value.
     */
    float readTemperature();

    /**
    * Sets the Output Data Rate bits for the Control Register 1 (CTRL_REG1(10h))
    * @param rate ODR bits
    */
    void setODRBits(OutputDataRate rate);

    /**
     * Sets the bit that defines the use of the FIFO watermark level in the FIFO Control Register (FIFO_CTRL(13h))
     * @param stopOnWTM flag to enable the mode or not
     */
    void setStopOnWTM(bool stopOnWTM);

    /**
     * Sets the bit that enables triggered FIFO modes in the FIFO Control Register (FIFO_CTRL(13h))
     * @param trigMode flag to enable trigger mode or not
     */
    void setTrigModes(bool trigMode);

    /**
     * Sets the FIFO mode in the FIFO Control Register (FIFO_CTRL(13h))
     */
    void setFIFOMode(FIFOModes mode);

    /**
     * Activates ONE_SHOT bit of CTRL_REG2 in order for the sensor to acquire a measurement
     */
    void triggerOneShotMode();

private:
    /**
     * I2C device address.
     */
    const LPS22HH_I2C_Address I2CAddress;

    /**
     * Milliseconds to wait for the sensor measurements to be completed in Single-shot Mode or a sensor reset to complete.
     * This value was chosen arbitrarily and seems to be working, it is not stated in the datasheet.
     */
    static constexpr uint8_t msToWait = 30;

    /**
     * The number of bytes a command consists of.
     */
    static constexpr uint8_t NumberOfBytesInCommand = 1;

    /**
     * The number of bytes hat are sent from the sensor after sending the command for Single-Shot Mode.
     * The data consist of 6 bytes, 2 raw sensor data bytes for each physical measurement plus 1 byte each for the checksum.
     */
    static constexpr uint8_t NumberOfBytesToReadFromRegister = 1;

    /**
     * Wait period before for abandoning an I2C transaction because the send/receive buffer does not get unloaded/gets loaded.
     */
    static constexpr uint16_t TimeoutTicks = 1000;

    /**
        * The pressure sensitivity value according to the datasheet in LSB/hPa. It is used to calculate the pressure.
        */
    static constexpr uint16_t PressureSensitivity = 4096;

    /**
     * The temperature sensitivity value according to the datasheet in LSB/°C. It is used to calculate the temperature.
     */
    static constexpr  uint8_t TemperatureSensitivity = 100;

    /**
     * The temperature measurement in °C.
     */
    float temperatureValue;

    /**
     * The pressure measurement in hPa.
     */
    float pressureValue;

    /**
     * Function that prevents hanging when a I2C device is not responding.
     */
    void waitForI2CBuffer() const {
        auto start = xTaskGetTickCount();
        while (LPS22HH_TWIHS_IsBusy()) {
            if (xTaskGetTickCount() - start > TimeoutTicks) {
                LOG_ERROR << "Humidity sensor with address " << I2CAddress << " , communication has timed out";
                LPS22HH_TWIHS_Initialize();
                break;
            }
        }
    };

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
     * @param registerAddress can be one of the register address enum types
     * @param txData the byte to be sent and written on the selected register
     */
    void writeToRegister(RegisterAddress registerAddress, uint8_t txData);

    /**
     * Attempts to read register data from the provided register address
     * @param registerAddress
     */
    uint8_t readFromRegister(RegisterAddress registerAddress);

};
