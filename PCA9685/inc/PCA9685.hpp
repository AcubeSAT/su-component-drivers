#pragma once

#include <cstdint>
#include <cmath>
#include <type_traits>
#include <etl/utility.h>
#include <etl/array.h>
#include <etl/span.h>
#include <etl/expected.h>
#include "PCA9685Configuration.hpp"
#include "Logger.hpp"
#include "FreeRTOS.h"
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
        I2CAddress_000000 = 0b100'0000,
        I2CAddress_101001 = 0b110'1001,
        I2CAddress_101010 = 0b110'1010,
        I2CAddress_101011 = 0b110'1011,
        I2CAddress_101100 = 0b110'1100,
        I2CAddress_100011 = 0b110'0011,
    };

    /**
     * Underlying type of I2CAddress enum class.
     */
    using I2CAddress_t = std::underlying_type_t<I2CAddress>;

    /**
     * @enum PWMChannel
     *
     * Lists all of the available PCA9685 PWM outputs
     */
    enum class PWMChannel : uint8_t {
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
     * Underlying type of PWMChannel enum class.
     */
    using PWMChannel_t = std::underlying_type_t<PWMChannel>;

    /**
     * Constructor for the PCA9685 class.
     *
     * @param i2cAddress The I2C address of the device.
     */
    explicit PCA9685(I2CAddress i2cAddress) : i2cAddress(i2cAddress) {};

    /**
     * Function that configures the device and starts the PWM cycle bu exiting low-power mode.
     *
     * @param config The configurations of the device.
     */
    void start(const PCA9685Configuration::Configuration& config);

    /**
     * Function that allows all the devices in the I2C-bus to be reset to the power-up state value
     * through a specific formatted I2C-bus command (SWRST register).
     */
    void reset();

    /**
     * Function that sets the PWM duty cycle of the specified channel given a starting delayPercent
     *
     * @param channel The PWM channel that is to be modified (0-15)
     * @param dutyCycle The duty cycle of the PWM given as a percentage
     * @param delayPercent The delayPercent time given as a percentage
     */
    void setPWMChannel(PWMChannel channel, uint8_t dutyCyclePercent, uint8_t delayPercent = 0);

    /**
     * Set the specified channel to always off
     *
     * @param channel
     */
    void setPWMChannelAlwaysOff(PWMChannel channel);

    /**
     * Set the specified channel to always on
     *
     * @param channel
     * @param delayPercent
     */
    void setPWMChannelAlwaysOn(PWMChannel channel, uint8_t delayPercent);

    /**
     * Function that sets all the PWM channels to the same values
     *
     * @param dutyCyclePercent
     * @param delayPercent
     */
    void setAllPWMChannels(uint8_t dutyCyclePercent, uint8_t delayPercent = 0);

    /**
     * Function that turns all the PWM channels off
     */
    void setAllPWMChannelsOff();

    /**
     * Function that turns all the PWM channels on
     */
    void setAllPWMChannelsOn(uint8_t delayPercent = 0);

    /**
     * Set device to operate at a new frequency (in Hz).
     *
     * @param frequency
     */
    void setPWMFrequency(float frequency);

    /**
     * Set device to low-power operation (no PWMs are generated).
     */
    void enterSleepMode();

    /**
     * Set device to normal operation.
     */
    void exitSleepMode();

    /**
     * Function that configures the device clock.
     * @param deviceClock
     */
    void setDeviceClock(PCA9685Configuration::DeviceClock deviceClock);

    /**
     * Function that configures the general-call (ALLCALL) address.
     */
    void configureGeneralCallAddress(PCA9685Configuration::GeneralCall generalCall);

    /**
     * Configure the output logic state (MODE2 register, INVRT bit).
     */
    void setOutputLogicState(PCA9685Configuration::OutputLogicState outputLogicState);

    /**
     *
     * @param outputChangeCondition
     */
    void setOutputChangeCondition(PCA9685Configuration::OutputChangeCondition outputChangeCondition);

    /**
     * Function that sets PCA9685 outputs to operate as either totem-pole or open-drain structure.
     */
    void setOutputDriveType(PCA9685Configuration::OutputDriveType outputDriveType);

private:

    /**
     * The slave address for the I2C protocol of the PCA9685 device.
     */
    const I2CAddress i2cAddress = I2CAddress::I2CAddress_101100;

    /**
     * The total number of readable registers.
     */
    static constexpr uint8_t NumberOfRegisters = 68;

    /**
     * The maximum step count (4096)
     */
    static constexpr uint16_t GrayscaleMaximumSteps = 0x1000;

    /**
     * The number of bytes/registers that define the form of the PWM
     */
    static constexpr uint8_t BytesPerPWM = 0x04;

    /**
     * The delay for exiting sleep mode (in ms).
     */
    static constexpr float SleepModeDelayMS = 0.6;

    /**
     * @enum SoftwareReset
     *
     * An enum that contains the reserved slave address (General Call) and the data byte that are used to reset
     * all the PCA9685 devices of the I2C bus to the power-up state.
     */
    enum class SoftwareReset : uint8_t {
        SLAVE_ADDRESS = 0x00,
        DATA_BYTE_1 = 0x06,
    };

    /**
     * Underlying type of the SoftwareReset enum class.
     */
    using SoftwareReset_t = std::underlying_type_t<SoftwareReset>;

    /**
     * Mask for the 8 LSB of the 12-bit PWM
     */
    static constexpr uint16_t MaskLSB = 0xFF;

    /**
     * Mask for the 4 MSB of the 12-bit PWM
     */
    static constexpr uint16_t MaskMSB = 0xF00;

    /**
     * Duty cycle of an always off PWM.
     */
    static constexpr uint8_t FullOffPWMDutyCycle = 0;

    /**
     * Duty cycle of an always on PWM.
     */
    static constexpr uint8_t FullOnPWMDutyCycle = 100;

    /**
     * The frequency (MHz) of the internal oscillator.
     */
    static constexpr float InternalOscillatorFrequency = 25;

    /**
     * The frequency (MHz) of the external oscillator (hardware configured).
     */
    const float ExternalOscillatorFrequency = 48;

    /**
     * Determine whether the clock being used is the internal or the external.
     */
    PCA9685Configuration::DeviceClock deviceClock = PCA9685Configuration::DeviceClock::INTERNAL_CLOCK;

    /**
     * The minimum pre-scale value for setting frequency.
     */
    static constexpr uint8_t MinimumPreScaleValue = 3;

    /**
     * The maximum pre-scale value for setting frequency.
     */
    static constexpr uint8_t MaximumPreScaleValue = 255;

    /**
     * @enum RegisterAddress
     *
     * An enum that contains the various register addresses that PCA9685 acknowledges
     */
    enum class RegisterAddress : uint8_t {
        MODE1 = 0x00,
        MODE2 = 0x01,
        ALLCALLADR = 0x05,
        LED0_ON_L = 0x06,
        ALL_LED_ON_L = 0xFA,
        ALL_LED_ON_H = 0xFB,
        ALL_LED_OFF_L = 0xFC,
        ALL_LED_OFF_H = 0xFD,
        PRE_SCALE = 0xFE,       // Writes to PRE_SCALE register are blocked when SLEEP bit is logic 0 (MODE 1)
    };

    /**
     * Underlying type of the RegisterAddress enum class.
     */
    using RegisterAddress_t = std::underlying_type_t<RegisterAddress>;

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
     * Underlying type of the Mode1RegisterMasks enum class.
     */
    using Mode1RegisterMasks_t = std::underlying_type_t<Mode1RegisterMasks>;

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
     * Underlying type of Mode2RegisterMasks enum class.
     */
    using Mode2RegisterMasks_t = std::underlying_type_t<Mode2RegisterMasks>;

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
     * Function that enables the auto-increment (AI) feature
     */
    void enableAutoIncrement();

    /**
     * Function that disables the auto-increment (AI) feature.
     */
    void disableAutoIncrement();

    /**
     * Functions that calculates the data that need to be stored at each one of the 4 PWM channel registers.
     *
     * @param channel The PCA9685 PWM channel.
     * @param dutyCyclePercent The PWM signal duty cycle, as a percentage.
     * @param delayPercent The PWM signal delay, as a percentage.
     *
     * @returns An array filled with the registers values, in an ascending (per channel) order.
     */
    static auto calculatePWMRegisterValues(uint8_t dutyCyclePercent, uint8_t delayPercent);

    /**
     * Function that writes a byte to a specific register
     *
     * @param registerAddress The address of the register
     * @param transmittedByte The transmitted byte
     */
    void i2cWriteValueToRegister(RegisterAddress registerAddress, uint8_t transmittedByte);

    /**
     * Function that reads from a specified register of the PCA9685 device.
     *
     * @param registerAddress The address of the register.
     * @param rData  The response of the device as an array of bytes.
     * @param returnedBytesNumber The number of bytes that are read from the register.
     *
     * @returns True if I2C transaction was successful.
     */
    template<size_t SIZE, typename T = uint8_t>
    etl::expected<etl::array<T, SIZE>, bool> i2cReadData();

    /**
     * Function that writes to a specified register of the PCA9685 device.
     *
     * @param tData The data sent to the specified register as an array of bytes.
     * @param numberOfBytesToWrite The number of bytes of the data sent to the register.
     *
     * @returns True if I2C transaction was successful.
     */
    template<typename T=I2CAddress>
    bool i2cWriteData(etl::span<uint8_t> buffer);

};

