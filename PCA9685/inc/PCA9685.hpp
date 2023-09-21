#pragma once

#include <cstdint>
#include <etl/utility.h>
#include <etl/array.h>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

#define PCA9685_TWI_PORT 2

/**
 * The PCA9685_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71 MCU will be used.
 * By giving the corresponding value to PCA9685_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 */
#if PCA9685_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define PCA9685_TWIHS_WriteRead TWIHS0_WriteRead
#define PCA9685_TWIHS_Write TWIHS0_Write
#define PCA9685_TWIHS_ErrorGet TWIHS0_ErrorGet
#define PCA9685_TWIHS_Read TWIHS0_Read
#define PCA9685_TWIHS_Initialize TWIHS0_Initialize
#define PCA9685_TWIHS_IsBusy TWIHS0_IsBusy

#elif PCA9685_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define PCA9685_TWIHS_WriteRead TWIHS1_WriteRead
#define PCA9685_TWIHS_Write TWIHS1_Write
#define PCA9685_TWIHS_ErrorGet TWIHS1_ErrorGet
#define PCA9685_TWIHS_Read TWIHS1_Read
#define PCA9685_TWIHS_Initialize TWIHS1_Initialize
#define PCA9685_TWIHS_IsBusy TWIHS1_IsBusy

#elif PCA9685_TWI_PORT == 2

#include "plib_twihs2_master.h"

#define PCA9685_TWIHS_WriteRead TWIHS2_WriteRead
#define PCA9685_TWIHS_Write TWIHS2_Write
#define PCA9685_TWIHS_ErrorGet TWIHS2_ErrorGet
#define PCA9685_TWIHS_Read TWIHS2_Read
#define PCA9685_TWIHS_Initialize TWIHS2_Initialize
#define PCA9685_TWIHS_IsBusy TWIHS2_IsBusy
#endif

/**
 * C++ class for interfacing with the PCA9685 LED driver device.
 *
 * @brief This class provides functions to read various electrical parameters from the INA228 device,
 * such as voltage, temperature, current, power, energy, and charge.
 */
class PCA9685 {
public:

    /**
     * @enum I2CAddress
     *
     * @brief Contains the I2C addresses, depending on the six address pins, A5|A4|A3|A2|A1|A0.
     *
     * The LED All Call (address 05h), Software Reset (00h) and LED Sub Call (02h, 03h, 04h), if enabled, I2C-bus
     * addresses are considered to be reserved.
     */
    enum class I2CAddress : uint8_t {
        I2CAddress_000001 = 0b000001,
        I2CAddress_000110 = 0b000110,
        I2CAddress_000111 = 0b000111,
        I2CAddress_001000 = 0b001000,
        I2CAddress_001001 = 0b001001,
        I2CAddress_001010 = 0b001010,
        I2CAddress_001011 = 0b001011,
        I2CAddress_001100 = 0b001100,
        I2CAddress_001101 = 0b001101,
        I2CAddress_001110 = 0b001110,
        I2CAddress_001111 = 0b001111,
        I2CAddress_010000 = 0b010000,
        I2CAddress_010001 = 0b010001,
        I2CAddress_010010 = 0b010010,
        I2CAddress_010011 = 0b010011,
        I2CAddress_010100 = 0b010100,
        I2CAddress_010101 = 0b010101,
        I2CAddress_010110 = 0b010110,
        I2CAddress_010111 = 0b010111,
        I2CAddress_011000 = 0b011000,
        I2CAddress_011001 = 0b011001,
        I2CAddress_011010 = 0b011010,
        I2CAddress_011011 = 0b011011,
        I2CAddress_011100 = 0b011100,
        I2CAddress_011101 = 0b011101,
        I2CAddress_011110 = 0b011110,
        I2CAddress_011111 = 0b011111,
        I2CAddress_100000 = 0b100000,
        I2CAddress_100001 = 0b100001,
        I2CAddress_100010 = 0b100010,
        I2CAddress_100011 = 0b100011,
        I2CAddress_100100 = 0b100100,
        I2CAddress_100101 = 0b100101,
        I2CAddress_100110 = 0b100110,
        I2CAddress_100111 = 0b100111,
        I2CAddress_101000 = 0b101000,
        I2CAddress_101001 = 0b101001,
        I2CAddress_101010 = 0b101010,
        I2CAddress_101011 = 0b101011,
        I2CAddress_101100 = 0b101100,
        I2CAddress_101101 = 0b101101,
        I2CAddress_101110 = 0b101110,
        I2CAddress_101111 = 0b101111,
        I2CAddress_110000 = 0b110000,
        I2CAddress_110001 = 0b110001,
        I2CAddress_110010 = 0b110010,
        I2CAddress_110011 = 0b110011,
        I2CAddress_110100 = 0b110100,
        I2CAddress_110101 = 0b110101,
        I2CAddress_110110 = 0b110110,
        I2CAddress_110111 = 0b110111,
        I2CAddress_111000 = 0b111000,
        I2CAddress_111001 = 0b111001,
        I2CAddress_111010 = 0b111010,
        I2CAddress_111011 = 0b111011,
        I2CAddress_111100 = 0b111100,
        I2CAddress_111101 = 0b111101,
        I2CAddress_111110 = 0b111110,
        I2CAddress_111111 = 0b111111
    };


    /**
     * @brief Constructor for the PCA9685 class.
     * @param something The I2C master port number used for communication.
     */
    explicit PCA9685(I2CAddress i2cAddress);

private:

    /**
     * The address for the I2C protocol of the PCA9685 device.
     */
    I2CAddress i2cAddress;

    /**
     * Function that reads from a specified register of the PCA9685 device.
     *
     * @param registerAddress The address of the register.
     * @param rData  The response of the device as an array of bytes.
     * @param numberOfBytesToRead The number of bytes that are read from the register.
     */
    void readRegister(RegisterAddress registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead);

    /**
     * Function that writes to a specified register of the PCA9685 device.
     *
     * @param tData The data sent to the specified register as an array of bytes.
     * @param numberOfBytesToWrite The number of bytes of the data sent to the register.
     */
    void writeRegister(uint8_t* tData, uint8_t numberOfBytesToWrite);

};
