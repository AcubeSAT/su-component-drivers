#pragma once

#include "Peripheral_Definitions.hpp"

namespace PayloadDrivers {

/**
 * The number of the LED instances as declared by the user in the keyValuePairs array.
 */
inline constexpr size_t NumberOfLEDStrings = 2;

/**
 * @class LED
 * @brief A class to manage a PWM-controlled LED.
 *
 * This class provides an interface to control an LED using PWM.
 * The PWM class is templated on the peripheral ID, meaning each LED
 * can control a specific peripheral with a distinct channel.
 */
template<PWM_PeripheralID PeripheralID>
class LED {
public:
    /**
     * @brief Constructor for LED.
     * @param channel The specific channel of the peripheral to control the LED.
     *
     * This constructor initializes the PWM instance with the given
     * channel, allowing control over the LED's brightness and frequency.
     */
    explicit LED(PWM_PeripheralChannel channel) : pwm(channel) {
        pwm.setFrequency(DefaultDimmingFrequency);
    }

    enum class LED_ControlModes {
        MODE1,
        MODE2,
        MODE3,
        MODE4,
    };

    /**
     * Default dimming frequency for PWM.
     */
    static constexpr uint32_t DefaultDimmingFrequency = 1000;

    /**
     * @brief Opens the LED (starts PWM signal).
     *
     * This function activates the PWM signal, causing the LED to start
     * emitting light at the specified duty cycle and frequency.
     */
    void open() {
        pwm.start();
    }

    /**
     * @brief Closes the LED (stops PWM signal).
     *
     * This function deactivates the PWM signal, effectively turning off
     * the LED.
     */
    void close() {
        pwm.stop();
    }

    /**
     * @brief Adjusts the relative brightness of the LED.
     * @param brightness Brightness level (0-100%).
     *
     * This function sets the duty cycle of the PWM signal, controlling
     * the brightness of the LED. A value of 100 results in full brightness,
     * while 0 turns off the LED.
     */
    void setRelativeBrightness(float brightness) {
        if (brightness > 100.0f) {
            brightness = 100.0f;
        }
        pwm.setDutyCycle(brightness);
    }

    /**
     * @brief Sets the dimming frequency for the LED.
     * @param frequency Frequency value in Hz.
     *
     * This function sets the PWM frequency for dimming the LED. A higher frequency
     * can lead to smoother dimming effects, while a lower frequency might cause flicker.
     */
    void setDimmingFrequency(float frequency) {
        pwm.setFrequency(frequency);
    }

private:
    /**
     * Templated PWM instance for the given peripheral.
     */
    PWM<PeripheralID> pwm;
};

static_assert(LED_PWM_Config.size() == NumberOfLEDStrings, "PWM channel allocations for LEDs are too many");
// inline LED<LED_PWM_Config[0].first> LED_String1 {LED_PWM_Config[0].second};
// inline LED<LED_PWM_Config[1].first> LED_String2 {LED_PWM_Config[1].second};

} // namespace PayloadDrivers
