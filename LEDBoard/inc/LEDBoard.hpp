#ifndef COMPONENT_DRIVERS_LEDBOARD_HPP
#define COMPONENT_DRIVERS_LEDBOARD_HPP

#include "definitions.h"
#include <etl/array.h>

/**
 * Excitation LED Board driver
 *
 * This is a simple driver for the in-house Excitation LED PCB of AcubeSAT
 * More details about the PCB can be found at:
 * https://gitlab.com/acubesat/su/optics/excitation-led-pcb
 *
 * @author Miltiadis Bozatzis <mpozan20@gmail.com>
 */
class LEDBoard {
private:
    /**
     * The number of the LEDs on the LEDBoard
     */
    constexpr uint8_t NumberOfLeds = 4;

    /**
     * Array of the pins connected to the LEDs of the Board
     */
    etl::array <PIO_PIN, NumberOfLeds> ledPins;

    /**
     * Array of the PWM Channels connected to each pin
     * each pin ledPins[i] should correspond to PWM Channel pinPwmChannels[i]
     */
    etl::array <PWM_CHANNEL_NUM, NumberOfLeds> pinPwmChannels;

    /**
     * Array of the PWM Channel Masks for each channel
     */
    etl::array <PWM_CHANNEL_MASK, NumberOfLeds> pinPwmChannelMasks;

    /**
     * Array of the PWM Handler of each pin
     * each pin ledPins[i] should correspond to PWM Handler pinPwmChannels[i]
     */
    etl::array<bool, NumberOfLeds> pwmHandlers;

public:
    /**
     * The Indexes of the LEDs on the LEDBoard
     */
    enum LedIndex : uint8_t {
        FIRST_LED = 0,
        SECOND_LED = 1,
        THIRD_LED = 2,
        FOURTH_LED = 3
    };

    /**
     * Constructor to set the initial LED intensities to zero and define the LED pins
     * @param pins Pointer to an array with the pins connected to the LEDs
     * @param channels Pointer to an array with the PWM channel of each LED Pin
     * @param handlers Pointer to an array with the PWM handler of each LED Pin
     */
    LEDBoard(PIO_PIN *pins, PWM_CHANNEL_NUM *channels, bool *handlers);

    /**
     * Set the LED intensities
     * @param ledIndex The index of the LED
     * @param ledValue The value of the led, boolean (on-true, off-false)
     */
    void ledWrite(LedIndex ledIndex, bool ledValue);

    /**
     * Set the LED intensities
     * @param ledIndex The index of the LED
     * @param letIntensity The value of the intensity (percentage from 0 to 100)
     */
    void setLedIntensity(LedIndex ledIndex, uint16_t intensity);

    /**
     * Stops the PWM on the selected LED pin
     * @param ledIndex The index of the LED
     */
    void closeLed(LedIndex ledIndex);

};


#endif //COMPONENT_DRIVERS_LEDBOARD_HPP
