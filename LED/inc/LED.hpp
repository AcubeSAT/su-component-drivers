#pragma once

#include "HAL_PWM.hpp"
#include "Logger.hpp"

/**
 * @class LED
 * @brief A class to manage a PWM-controlled LED.
 *
 * This class provides an interface to control an LED using PWM (Pulse Width Modulation).
 * It allows the user to open and close the LED, adjust its brightness, and change the
 * dimming frequency.
 *
 * @tparam PeripheralNumber The peripheral number for the PWM hardware.
 * @tparam ChannelNum The specific PWM channel number.
 * @tparam ChannelMask The bitmask for the PWM channel.
 */
template<HAL_PWM::Peripheral PeripheralNumber, PWM_CHANNEL_NUM ChannelNum, PWM_CHANNEL_MASK ChannelMask>
class LED {
public:
    /**
     * @enum OperationalState
     * @brief Enumeration representing the operational state of the LED.
     */
    enum class OperationalState {
        LED_OPEN,   /// The LED is open (on).
        LED_CLOSED, /// The LED is closed (off).
    };

    /**
     * @brief The MCU clock frequency in Hz.
     *
     * This constant represents the clock frequency of the microcontroller unit.
     * If the MCU clock frequency changes, this value must be updated accordingly.
     */
    static constexpr uint32_t MCUClockFrequency = 150e6;

    /**
     * @brief The default PWM duty cycle.
     *
     * This constant represents the default duty cycle for the PWM signal.
     * It is expressed as a percentage (0-100).
     */
    static constexpr uint8_t DefaultDutyCycle = 50;

    /**
     * @brief The default PWM frequency in Hz.
     *
     * This constant represents the default frequency for the PWM signal.
     */
    static constexpr uint32_t DefaultDimmingFrequency = 1e3;

    /**
     * @struct Configuration
     * @brief Structure representing the configuration for the LED.
     *
     * Contains the LED's brightness (PWM duty cycle) and dimming frequency.
     */
    struct Configuration {
        uint8_t ledBrightness = DefaultDutyCycle;
        uint32_t ledDimmingFrequency = DefaultDimmingFrequency;
    };

    /**
     * @brief Default constructor for the LED class.
     *
     * Initializes the LED object with default values.
     */
    LED() = default;

    /**
     * @brief Opens (turns on) the LED.
     *
     * This method enables the PWM channel to start the LED with the current
     * brightness and frequency settings.
     */
    void openLED();

    /**
     * Opens (turns on) the LED with specific configuration.
     *
     * This method enables the PWM channel to start the LED with the provided
     * brightness and frequency settings.
     *
     * @param ledConfig The configuration for the LED.
     */
    void openLED(const Configuration& ledConfig);

    /**
     * @brief Closes (turns off) the LED.
     *
     * This method disables the PWM channel, effectively turning off the LED.
     */
    void closeLED() {
        HAL_PWM::PWM_ChannelsStop<PeripheralNumber>(ChannelMask);
        ledStatus = OperationalState::LED_CLOSED;
    }

    /**
     * @brief Sets the relative brightness of the LED.
     *
     * This method adjusts the duty cycle of the PWM signal to change the brightness
     * of the LED. The duty cycle is given as a percentage (0-100).
     *
     * @param dutyCycle The desired duty cycle as a percentage (0-100).
     */
    inline void setRelativeBrightness(uint8_t dutyCycle) {
        config.ledBrightness = dutyCycle;
    };

    /**
     * @brief Sets the dimming frequency of the LED.
     *
     * This method adjusts the frequency of the PWM signal to change the dimming rate
     * of the LED.
     *
     * @param frequency The desired frequency in Hz.
     */
    inline void setDimmingFrequency(uint32_t frequency) {
        config.ledDimmingFrequency = frequency;
    };

    /**
     * @brief Gets the current PWM frequency.
     *
     * This method returns the current frequency of the PWM signal used for the LED.
     *
     * @return The current PWM frequency in Hz.
     */
    uint16_t getFrequency() const {
        return HAL_PWM::PWM_ChannelPeriodGet<PeripheralNumber>(ChannelNum);
    };

    /**
     * @brief Gets the current PWM duty cycle.
     *
     * This method returns the current duty cycle of the PWM signal used for the LED.
     *
     * @return The current duty cycle as a percentage (0-100).
     */
    uint8_t getDutyCycle() const {
        return config.ledBrightness;
    };

    /**
     * @brief Gets the operational status of the LED.
     *
     * This method returns the current operational status of the LED.
     *
     * @return The operational status of the LED.
     */
    OperationalState getLEDStatus() const {
        return ledStatus;
    };

private:
    /**
     * The current operational state of the LED (initially closed).
     */
    mutable OperationalState ledStatus = OperationalState::LED_CLOSED;

    /**
     * The configuration for the LED.
     */
    mutable Configuration config;
};
