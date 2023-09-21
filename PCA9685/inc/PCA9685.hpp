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
     * @brief Contains the I2C addresses, depending on the two address pins, A0 and A1.
     */
    enum class I2CAddress : uint8_t {
        I2CAddress_000000 = 0x00,
        I2CAddress_000001 = 0x01,
        I2CAddress_000010 = 0x02,
        I2CAddress_000011 = 0x03,
        I2CAddress_000100 = 0x04,
        I2CAddress_000101 = 0x05,
        I2CAddress_000110 = 0x06,
        I2CAddress_000111 = 0x07,
        I2CAddress_001000 = 0x08,
        I2CAddress_001001 = 0x09,
        I2CAddress_001010 = 0x0A,
        I2CAddress_001011 = 0x0B,
        I2CAddress_001100 = 0x0C,
        I2CAddress_001101 = 0x0D,
        I2CAddress_001110 = 0x0E,
        I2CAddress_001111 = 0x0F,
        I2CAddress_010000 = 0x10,
        I2CAddress_010001 = 0x11,
        I2CAddress_010010 = 0x12,
        I2CAddress_010011 = 0x13,
        I2CAddress_010100 = 0x14,
        I2CAddress_010101 = 0x15,
        I2CAddress_010110 = 0x16,
        I2CAddress_010111 = 0x17,
        I2CAddress_011000 = 0x18,
        I2CAddress_011001 = 0x19,
        I2CAddress_011010 = 0x1A,
        I2CAddress_011011 = 0x1B,
        I2CAddress_011100 = 0x1C,
        I2CAddress_011101 = 0x1D,
        I2CAddress_011110 = 0x1E,
        I2CAddress_011111 = 0x1F,
        I2CAddress_100000 = 0x20,
        I2CAddress_100001 = 0x21,
        I2CAddress_100010 = 0x22,
        I2CAddress_100011 = 0x23,
        I2CAddress_100100 = 0x24,
        I2CAddress_100101 = 0x25,
        I2CAddress_100110 = 0x26,
        I2CAddress_100111 = 0x27,
        I2CAddress_101000 = 0x28,
        I2CAddress_101001 = 0x29,
        I2CAddress_101010 = 0x2A,
        I2CAddress_101011 = 0x2B,
        I2CAddress_101100 = 0x2C,
        I2CAddress_101101 = 0x2D,
        I2CAddress_101110 = 0x2E,
        I2CAddress_101111 = 0x2F,
        I2CAddress_110000 = 0x30,
        I2CAddress_110001 = 0x31,
        I2CAddress_110010 = 0x32,
        I2CAddress_110011 = 0x33,
        I2CAddress_110100 = 0x34,
        I2CAddress_110101 = 0x35,
        I2CAddress_110110 = 0x36,
        I2CAddress_110111 = 0x37,
        I2CAddress_111000 = 0x38,
        I2CAddress_111001 = 0x39,
        I2CAddress_111010 = 0x3A,
        I2CAddress_111011 = 0x3B,
        I2CAddress_111100 = 0x3C,
        I2CAddress_111101 = 0x3D,
        I2CAddress_111110 = 0x3E,
        I2CAddress_111111 = 0x3F
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
