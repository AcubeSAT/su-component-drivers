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
        I2CAddress_101001 = 0b101001,
        I2CAddress_101010 = 0b101010,
        I2CAddress_101011 = 0b101011,
        I2CAddress_101100 = 0b101100,
    };

    /**
     * @brief Constructor for the PCA9685 class.
     * @param something The I2C master port number used for communication.
     */
    explicit PCA9685(I2CAddress i2cAddress);

    void setDeviceToSleep(bool sleep);
    void setExternalClock(bool externalClock);
    void performHardwareRestart(bool hardwareRestart);
    bool readHardwareRestartStatus();
    void allowRegisterAutoIncrement(bool allow);
    void applySoftwareReset();

private:

    /**
     * The number of bytes/registers that define the form of the PWM
     */
    static constexpr uint8_t NumberOfBytesPerPWMChannel = 0x04;

    /**
     * The address of LED0_ON_L register
     */
    static constexpr uint8_t RegisterAddressOfFirstPWMChannel = 0x06;

    /**
     * Software Reset (SWRST) data byte
     *
     * @brief Once the General Call address has been sent and acknowledged, the master sends this specific value
     * (SWRST data byte 1) in order for the PCA9685 to reset to power-up values.
     */
    static constexpr uint8_t softwareResetDataByte = 0x6;

    /**
     * @enum RegisterAddresses
     *
     * An enum that contains the various register addresses that PCA9685 acknowledges
     */
    enum class RegisterAddresses : uint8_t {
        MODE1 = 0x00,
        MODE2 = 0x01,
        SUBADR1 = 0x02,
        SUBADR2 = 0x03,
        SUBADR3 = 0x04,
        ALLCALLADR = 0x05,
        ALL_LED_ON_L = 0xFA,
        ALL_LED_ON_H = 0xFB,
        ALL_LED_OFF_L = 0xFC,
        ALL_LED_OFF_H = 0xFD,
        PRE_SCALE = 0xFE,       // Writes to PRE_SCALE register are blocked when SLEEP bit is logic 0 (MODE 1)
    };

    /**
     * @enum HardwareRestart
     *
     * Bit 7 of MODE1 register
     */
    enum class HardwareRestart : uint8_t {
        DISABLED = 0x00,
        ENABLED = 0x80,
    };

    /**
     * @enum Oscillator
     *
     * Determine whether PCA9685 uses either the internal or an external clock
     */
    enum class Oscillator : uint8_t {
        INTERNAL = 0x00,
        EXTERNAL = 0x40,
    };

    /**
     * @enum RegisterAutoIncrement
     *
     * Determine whether register auto increment (AI) is either enabled or disabled
     */
    enum class RegisterAutoIncrement : uint8_t {
        DISABLED = 0x00,
        ENABLED  = 0x20,
    };

    /**
     * @enum SleepMode
     *
     * Determine whether PCA9685 operates either in normal mode or in low-power mode
     */
    enum class SleepMode : uint8_t {
        NORMAL = 0x00,
        SLEEP  = 0x10,
    };

    /**
     * @enum RespondToI2CBusAddresses
     *
     * Determine whether PCA9685 operates either in normal mode or in low-power mode
     */
    enum class RespondToI2CBusAddresses : uint8_t {
        SUB1_NO_RESPOND = 0x0,
        SUB1_RESPOND = 0x8,
        SUB2_NO_RESPOND = 0x0,
        SUB2_RESPOND = 0x4,
        SUB3_NO_RESPOND = 0x0,
        SUB3_RESPOND = 0x2,
        ALLCALL_NO_RESPOND = 0x0,
        ALLCALL_RESPOND = 0x1,
    };

    /**
     * @enum OutputInvert
     *
     * Output logic state can be inverted (1h) or not (oh)
     *
     * @brief Setting the INVRT bit of MODE1 register allows you to control whether the output is active high
     * or active low when the PWM reaches its specified value.
     */
    enum class OutputInvert : uint8_t {
        NOT_INVERTED = 0x00,
        INVERTED = 0x10,
    };

    /**
     * @enum OutputChange
     *
     * Determine whether outputs change on STOP or ACK command
     */
    enum class OutputChangesOn : uint8_t {
        STOP = 0x0,
        ACK = 0x8,
    };

    /**
     * @enum OutputConfiguration
     *
     * Output can be configured either as open-drain (0h) or totem-pole (1h) structure
     *
     * @brief This can be configured through the OUTDRV bit of the MODE1 register
     */
    enum class OutputConfiguration : uint8_t {
        OPEN_DRAIN_STRUCTURE = 0x0,
        TOTEM_POLE_STRUCTURE = 0x4,
    };

    /**
     * @enum OEPinHighStates
     *
     * Possible states of the PCA9685 IC when OE pin is set HIGH (datasheet p. 7.4)
     */
    enum class OEPinHighStates: uint8_t {
        LOW = 0x0,              // OUTNE1: 0, OUTNE0: 0
        HIGH = 0x1,             // OUTNE1: 0, OUTNE0: 1
        HIGH_IMPEDANCE = 0x2,   // OUTNE1: 1, OUTNE0: 0 or OUTNE1: 1, OUTNE0: 1
    };

    /**
     * The address for the I2C protocol of the PCA9685 device.
     */
    I2CAddress i2cAddress;

    /**
     * Slave address to select READ operation
     */
    uint8_t slaveAddressRead;

    /**
     * Slave address to select WRITE operation
     */
    uint8_t slaveAddressWrite;

    void setMode1Register();

    void setMode2Register();

    /**
     * Function that reads from a specified register of the PCA9685 device.
     *
     * @param registerAddress The address of the register.
     * @param rData  The response of the device as an array of bytes.
     * @param numberOfBytesToRead The number of bytes that are read from the register.
     */
    void readRegister(RegisterAddresses registerAddress, uint8_t* rData, uint8_t numberOfBytesToRead);

    /**
     * Function that writes to a specified register of the PCA9685 device.
     *
     * @param tData The data sent to the specified register as an array of bytes.
     * @param numberOfBytesToWrite The number of bytes of the data sent to the register.
     */
    void writeRegister(uint8_t* tData, uint8_t numberOfBytesToWrite);

};
