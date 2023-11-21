#pragma once

#include <cstdint>
#include <type_traits>
#include <etl/utility.h>
#include <etl/array.h>
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
 * @brief This class provides functions to operate the PCA9685 LED driver.
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
    enum class I2CAddress : uint16_t {
        I2CAddress_000000 = 0b000000,
        I2CAddress_101001 = 0b101001,
        I2CAddress_101010 = 0b101010,
        I2CAddress_101011 = 0b101011,
        I2CAddress_101100 = 0b101100,
        I2CAddress_100011 = 0b100011,
    };

    /**
     * @enum PWMChannelsNumber
     *
     * Lists all of the available PCA9685 PWM outputs
     */
    enum class PWMChannels : uint8_t {
        CHANNEL0 = 0,
        CHANNEL1 = 1,
        CHANNEL2 = 2,
        CHANNEL3 = 3,
        CHANNEL4 = 4,
        CHANNEL5 = 5,
        CHANNEL6 = 6,
        CHANNEL7 = 7,
        CHANNEL8 = 8,
        CHANNEL9 = 9,
        CHANNEL10 = 10,
        CHANNEL11 = 11,
        CHANNEL12 = 12,
        CHANNEL13 = 13,
        CHANNEL14 = 14,
        CHANNEL15 = 15,
    };

    /**
     * Constructor for the PCA9685 class.
     *
     * @param something The I2C master port number used for communication.
     */
    explicit PCA9685(I2CAddress i2cAddress);

    /**
     * Function that sets the PWM duty cycle of the specified channel given a starting delayPercent
     *
     * @param channel The PWM channel that is to be modified (0-15)
     * @param dutyCycle The duty cycle of the PWM given as a percentage
     * @param delayPercent The delayPercent time given as a percentage
     */
    void setPWMChannel(PWMChannels channel, uint8_t dutyCyclePercent, uint8_t delayPercent = 0);

    /**
     * Function that sets all the PWM channels to the same values
     *
     * @param dutyCyclePercent
     * @param delayPercent
     */
    void setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent = 0);

    /**
     * Set device to operate at a new frequency. The PRE_SCALE register defines that frequency.
     *
     * @param frequency
     */
    void setDeviceFrequency(uint16_t frequency);

    /**
     * Function that enables the auto-increment (AI) feature
     */
    void enableAutoIncrement();

    /**
     * Function that disables the auto-increment (AI) feature.
     */
    void disableAutoIncrement();

    /**
     * Function that enables the PCA9695's internal (EXTCLK) clock.
     */
    void enableExternalClock();

    /**
     * Function that enables the PCA9695's external (EXTCLK) clock.
     */
    void disableExternalClock();

    /**
     * Set device to low-power operation (no PWMs are generated).
     */
    void setDeviceToSleepMode();

    /**
     * Set device to normal operation.
     */
    void stopDeviceFromSleepMode();

    /**
     * Enable the I2C sub-addresses and/or All-Call address.
     *
     * @param sub1 Sub-address 1: Bit 3 of MODE1 register, true if enabled.
     * @param sub2 Sub-address 2: Bit 2 of MODE1 register, true if enabled.
     * @param sub3 Sub-address 3: Bit 1 of MODE1 register, true if enabled.
     * @param allCall All-Call address: Bit 0 of MODE1 register, true if enabled.
     */
    void
    enableDeviceResponseToSubAddresses(bool sub1 = false, bool sub2 = false, bool sub3 = false, bool allCall = false);

    /**
     * Disable the I2C sub-addresses and/or All-Call address
     *
     * @param sub1 Sub-address 1: Bit 3 of MODE1 register, true if disabled.
     * @param sub2 Sub-address 2: Bit 2 of MODE1 register, true if disabled.
     * @param sub3 Sub-address 3: Bit 1 of MODE1 register, true if disabled.
     * @param allCall All-Call address: Bit 0 of MODE1 register, true if enabled.
     */
    void
    disableDeviceResponseToSubAddresses(bool sub1 = false, bool sub2 = false, bool sub3 = false, bool allCall = false);

    /**
     * Configure the output logic state (MODE2 register, INVRT bit).
     *
     * @param invert true if output logic state inverted.
     */
    void invertOutputs(bool invert);

    /**
     * Configure the outputs to change on STOP or ACK command (MODE2 register, OCH bit).
     *
     * @param stop true if outputs change on STOP command.
     */
    void setOutputChangeOn(bool stop);

    /**
     * Configure the outputs to totem-pole structure.
     */
    void setTotemPoleOutputs();

    /**
     * Configure the outputs to open-drain structure.
     */
    void setOpenDrainOutputs();

    /**
     * Configure OE pin function when OE=1.
     *
     * @param outne00 true if bits of OUTNE[1:0] are '00'.
     * @param outne01 true if bits of OUTNE[1:0] are '01'.
     * @param outne1x true if bits of OUTNE[1:0] are '1x'.
     */
    void setOutputEnableState(bool outne00 = true, bool outne01 = false, bool outne1x = false);

    /**
     * Function that allows all the devices in the I2C-bus to be reset to the power-up state value
     * through a specific formatted I2C-bus command (SWRST register).
     */
    void softwareResetOfAllDevices();

private:

    /**
     * Number of PWM channel of the PCA9685
     */
    static constexpr uint8_t PWMChannelsNumber = 16;

    /**
     * The maximum step count (4096)
     */
    static constexpr uint16_t GrayscaleMaximumSteps = 0x1000;

    /**
     * The number of bytes/registers that define the form of the PWM
     */
    static constexpr uint8_t NumberOfBytesPerPWMChannelRegisters = 0x04;

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
     * Mask for the 8 LSB of the 12-bit PWM
     */
    static constexpr uint16_t MaskLSB = 0xFF;

    /**
     * Mask for the 4 MSB of the 12-bit PWM
     */
    static constexpr uint16_t MaskMSB = 0xF00;

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
     * @enum Mode1RegisterMasks
     *
     * A collection of OR and AND masks that are used to PCA9685 configurations through MODE1 register.
     */
    enum class Mode1RegisterMasks : uint8_t {
        RESTART_DEVICE_ENABLE = 0x80,
        RESTART_DEVICE_DISABLE = 0x7F,
        EXTERNAL_CLOCK_ENABLE = 0x40,
        EXTERNAL_CLOCK_DISABLE = 0xBF,
        AUTO_INCREMENT_ENABLE = 0x20,
        AUTO_INCREMENT_DISABLE = 0xDF,
        SLEEP_ENABLE = 0x10,
        SLEEP_DISABLE = 0xEF,
        SUB1_RESPOND_ENABLE = 0x08,
        SUB1_RESPOND_DISABLE = 0xF7,
        SUB2_RESPOND_ENABLE = 0x04,
        SUB2_RESPOND_DISABLE = 0xFB,
        SUB3_RESPOND_ENABLE = 0x02,
        SUB3_RESPOND_DISABLE = 0xFD,
        ALLCALL_RESPOND_ENABLE = 0x01,
        ALLCALL_RESPOND_DISABLE = 0xFE,
    };

    /**
     * @enum Mode2RegisterMasks
     *
     * A collection of OR and AND masks that are used to PCA9685 configurations through MODE2 register.
     */
    enum class Mode2RegisterMasks : uint8_t {
        OUTPUT_INVERT_ENABLE = 0X10,
        OUTPUT_INVERT_DISABLE = 0xEF,
        OUTPUT_CHANGES_ON_ACK = 0X08,
        OUTPUT_CHANGES_ON_STOP = 0xF7,
        OUTPUT_CONFIGURATION_TOTEM_POLE = 0X04,
        OUTPUT_CONFIGURATION_OPEN_DRAIN = 0xFB,
        OUTPUT_ENABLE_STATE_HIGH = 0X01,
        OUTPUT_ENABLE_STATE_HIGH_IMPEDANCE = 0X02,
    };

    /**
     * MODE1 register byte, initial value 0h.
     *
     * Initial Configuration:
     * ----------------------
     * Bit 7: RESTART   -> 0
     * Bit 6: EXTCLK    -> 0
     * Bit 5: AI        -> 0
     * Bit 4: SLEEP     -> 0
     * Bit 3: SUB1      -> 0
     * Bit 2: SUB2      -> 0
     * Bit 1: SUB3      -> 0
     * Bit 0: ALLCALL   -> 0
     */
    uint8_t mode1RegisterByte = 0;

    /**
     * MODE2 register byte, initial value 0h.
     *
     * Initial Configuration:
     * ----------------------
     * Bit 7: (RESERVED)
     * Bit 6: (RESERVED)
     * Bit 5: (RESERVED)
     * Bit 4: INVRT     -> 0
     * Bit 3: OCH       -> 0
     * Bit 2: OUTDRV    -> 0
     * Bit 1: OUTNE[1]  -> 0
     * Bit 0: OUTNE[0]  -> 0
     */
    uint8_t mode2RegisterByte = 0;

    /**
     * The address for the I2C protocol of the PCA9685 device.
     */
    I2CAddress i2cAddress = I2CAddress::I2CAddress_101100;

    /**
     * Set the specified channel to always off
     *
     * @param channel
     */
    void setPWMChannelAlwaysOff(PWMChannels channel);

    /**
     * Set the specified channel to always on
     *
     * @param channel
     * @param delayPercent
     */
    void setPWMChannelAlwaysOn(PWMChannels channel, uint8_t delayPercent = 0);

    /**
     * Function that turns all the PWM channels off
     */
    void setAllPWMChannelsOff();

    /**
     * Function that turns all the PWM channels on
     */
    void setAllPWMChannelsOn(uint8_t delayPercent = 0);

    /**
     * Function that writes a byte to a specific register
     *
     * @param registerAddress The address of the register
     * @param transmittedByte The transmitted byte
     */
    void i2cWriteToSpecificRegister(uint8_t registerAddress, uint8_t transmittedByte);

    /**
     * Function that reads from a specified register of the PCA9685 device.
     *
     * @param registerAddress The address of the register.
     * @param rData  The response of the device as an array of bytes.
     * @param numberOfBytesToRead The number of bytes that are read from the register.
     *
     * @returns True if I2C transaction was successful.
     */
    bool i2cReadData(RegisterAddresses registerAddress, uint8_t *rData, uint8_t numberOfBytesToRead);

    /**
     * Function that writes to a specified register of the PCA9685 device.
     *
     * @param tData The data sent to the specified register as an array of bytes.
     * @param numberOfBytesToWrite The number of bytes of the data sent to the register.
     *
     * @returns True if I2C transaction was successful.
     */
    bool i2cWriteData(uint8_t *tData, uint8_t numberOfBytesToWrite);

};
