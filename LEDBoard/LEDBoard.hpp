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

    const uint8_t numberOfLeds = 4;

    /**
     * Array of the pins connected to the LEDs of the Board
     */
    etl::array<PIO_PIN, numberOfLeds> ledPins;

public:

    /**
     * Constructor to set the initial LED intensities to zero
     * @param ledPins Pointer to an array with the pins connected to the LEDs
     */
    LEDBoard(PIO_PIN *ledPins);

    /**
     * Set the LED intensities
     * @param ledIndex The index of the LED (1,2,3,4)
     * @param letIntensity The value of the intensity
     */
    void setLedIntensity(uint8_t ledIndex, uint8_t ledIntensity);

};


#endif //COMPONENT_DRIVERS_LEDBOARD_HPP
