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
