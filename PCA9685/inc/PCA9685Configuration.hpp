#pragma once

#include <cstdint>

namespace PCA9685Configuration {

    /**
     * @enum DeviceClock
     *
     * Configure device to use either the internal (25MHz) or the external (EXTCLK pin) clock.
     */
    enum class DeviceClock : uint8_t {
        INTERNAL_CLOCK = 0,
        EXTERNAL_CLOCK = 1,
    };

    /**
     * @enum GeneralCall
     *
     * Configure all PCA9685 devices either to respond to the ALL_CALL address or not.
     */
    enum class GeneralCall : uint8_t {
        DISABLED = 0,
        ENABLED = 1,
    };

    /**
     * Default General Call (ALL_CALL) I2C bus address.
     */
    static constexpr uint8_t GeneralCallAddressDefault = 0b1110'000;

    /**
     * @enum OutputLogicState
     *
     * Configure the output signal to be either inverted or not.
     */
    enum class OutputLogicState : uint8_t {
        OUTPUT_NOT_INVERTED = 0,
        OUTPUT_INVERTED = 1,
    };

    /**
     * @enum OutputChangeCondition
     *
     * Configure the PWM outputs to refresh either on STOP command or after ACK.
     */
    enum class OutputChangeCondition : uint8_t {
        STOP = 0,
        ACK = 0,
    };

    /**
     * @enum OutputDriveType
     *
     * Configure the structure of the outputs to either totem-pole or open-drain.
     */
    enum class OutputDriveType : uint8_t {
        OPEN_DRAIN = 0,
        TOTEM_POLE = 1,
    };

    /**
     * @enum OutputNotEnabled
     *
     * Enum representing the different states of output behavior when output drivers are not enabled (OUTNE).
     */
    enum class OutputNotEnabled : uint8_t {
        LEDn_0 = 0b00, // OE = 1, LEDn = 0
        LEDn_1 = 0b01, // OE = 1, OUTDRV = 1 => LEDn = 1 or OUTDRV = 0 => LEDn = HiZ
        LEDn_HI_Z = 0b10, // OE = 1, LEDn = high-impedance
    };

    /**
     * @struct Configuration
     *
     * Configure the PCA9685 device before it starts operating.
     */
    struct Configuration {
        // MODE1
        DeviceClock deviceClock = DeviceClock::INTERNAL_CLOCK;
        GeneralCall generalCall = GeneralCall::DISABLED;
        uint8_t generalCallAddress = GeneralCallAddressDefault;
        
        // MODE2
        OutputLogicState outputLogicState = OutputLogicState::OUTPUT_NOT_INVERTED;
        OutputChangeCondition outputChangeCondition = OutputChangeCondition::STOP;
        OutputDriveType outputDriveType = OutputDriveType::OPEN_DRAIN;
        OutputNotEnabled outputNotEnabled = OutputNotEnabled::LEDn_0;
    };

}
