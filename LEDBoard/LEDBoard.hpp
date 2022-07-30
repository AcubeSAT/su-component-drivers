#ifndef COMPONENT_DRIVERS_LEDBOARD_HPP
#define COMPONENT_DRIVERS_LEDBOARD_HPP

#include "definitions.h"


class LEDBoard{
private:

    enum LED_PIN : PIO_PIN {
        LED_1_PIN = PD3,
        LED_2_PIN = PD5,
        LED_3_PIN = PD7,
        LED_4_PIN = PC19
    };


public:

    LEDBoard();

    void setLedIntensity(LED_PIN pin, uint8_t letIntensity);

};


#endif //COMPONENT_DRIVERS_LEDBOARD_HPP
