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
class LEDBoard{
private:

    /**
     * The number of the LEDs on the LEDBoard
     */
    const uint8_t numberOfLeds = 4;

    /**
     * Array of the pins connected to the LEDs of the Board
     */
    etl::array<PIO_PIN, numberOfLeds> ledPins;

public:

    /**
     * Constructor to set the initial LED intensities to zero and define the LED pins
     * @param pins Pointer to an array with the pins connected to the LEDs
     */
    LEDBoard(PIO_PIN *pins);

    /**
     * Set the LED intensities
     * @param ledIndex The index of the LED (1,2,3,4)
     * @param letIntensity The value of the intensity (boolean)
     * @todo Modify this function to accept various ledIntensity values and change the LED voltage accordingly with PWM
     */
    void setLedIntensity(uint8_t ledIndex, bool ledIntensity);

};


#endif //COMPONENT_DRIVERS_LEDBOARD_HPP
