#pragma once

#include "PWM.hpp"
#include "Peripheral_Definitions.hpp"
#include "etl/utility.h"
#include "etl/vector.h"
#include "etl/array.h"

namespace PayloadDrivers {

constexpr etl::array<etl::pair<PWM_PeripheralID, PWM_PeripheralChannel>, 3> keyValuePairs = {{
    {PWM_PeripheralID::PERIPHERAL_0, PWM_PeripheralChannel::CHANNEL0},
    {PWM_PeripheralID::PERIPHERAL_1, PWM_PeripheralChannel::CHANNEL1},
    {PWM_PeripheralID::PERIPHERAL_1, PWM_PeripheralChannel::CHANNEL2},
}};

inline constexpr size_t NumberOfLEDStrings = keyValuePairs.size();

/**
 * @class LED
 * @brief A class to manage a PWM-controlled LED.
 *
 * This class provides an interface to control an LED using PWM.
 * The PWM class is templated on the peripheral ID, meaning each LED
 * can control a specific peripheral with a distinct channel.
 */
template<PWM_PeripheralID peripheralID>
class LED {
public:
    /**
     * @brief Constructor for LED.
     * @param channel The specific channel of the peripheral to control the LED.
     *
     * This constructor initializes the PWM instance with the given
     * channel, allowing control over the LED's brightness and frequency.
     */
    explicit LED(PWM_PeripheralChannel channel) : pwm(channel) {}

    /**
     * Default dimming frequency for PWM.
     */
    uint32_t dimmingFrequency = 1000;

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
    void setRelativeBrightness(uint32_t brightness) {
        if (brightness > 100) {
            brightness = 100;
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
    void setDimmingFrequency(uint32_t frequency) {
        pwm.setFrequency(frequency);
    }

private:
    /**
     * Templated PWM instance for the given peripheral.
     */
    PWM<peripheralID> pwm;
};

/**
 * @class LED_Init
 * @brief Initializes and manages a collection of LED objects.
 *
 * This class provides functionality to initialize LEDs based on the peripheral map
 * and create an array or vector of LED objects. It supports initialization of LEDs
 * for multiple peripherals and channels, allowing the user to ignore peripheral details.
 */
class LED_Init {
public:
    /**
     * @brief Initializes LEDs for all peripherals and channels.
     * @return A vector of LEDs for all peripherals and channels, using etl::variant to handle different types.
     */
    static auto initLEDs() {
        etl::vector<etl::variant<LED<PWM_PeripheralID::PERIPHERAL_0>, LED<PWM_PeripheralID::PERIPHERAL_1>>, NumberOfLEDStrings> ledVector;

        for (const auto& kv : keyValuePairs) {
            if (kv.first == PWM_PeripheralID::PERIPHERAL_0) {
                ledVector.push_back(LED<PWM_PeripheralID::PERIPHERAL_0>(kv.second));
            }
            else if (kv.first == PWM_PeripheralID::PERIPHERAL_1) {
                ledVector.push_back(LED<PWM_PeripheralID::PERIPHERAL_1>(kv.second));
            }
        }

        return ledVector;
    }
};

inline auto ledCollection = LED_Init::initLEDs();

} // namespace PayloadDrivers
