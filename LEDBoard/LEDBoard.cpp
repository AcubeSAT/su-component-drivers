#include "LEDBoard.hpp"

LEDBoard::LEDBoard(PIO_PIN *pins) {
    for(uint8_t i=0; i<numberOfLeds; i++){
        ledPins[i] = pins[i];
        PIO_PinWrite(ledPins[i], true);
    }
}

void LEDBoard::setLedIntensity(uint8_t ledIndex, bool ledIntensity) {
    PIO_PIN ledPin;
    if(ledIndex <= 4 && ledIndex > 0){
        PIO_PIN = ledPins[ledIndex];
    }
    else return;

    PIO_PinWrite(ledPin, !ledIntensity);
}

