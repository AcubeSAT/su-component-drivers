#include "../inc/LEDBoard.hpp"

LEDBoard::LEDBoard(PIO_PIN *pins, PWM_CHANNEL_NUM *channels, bool *handlers) {
    for(uint8_t i=0; i<numberOfLeds; i++){
        ledPins[i] = pins[i];
        pinPwmChannels[i] = channels[i];
        pwmHandlers[i] = handlers[i];
        PIO_PinWrite(ledPins[i], true);

        if(pinPwmChannels[i] == PWM_CHANNEL_0)
            pinPwmChannelMasks[i] = PWM_CHANNEL_0_MASK;
        else if(pinPwmChannels[i] == PWM_CHANNEL_1)
            pinPwmChannelMasks[i] = PWM_CHANNEL_1_MASK;
        else if(pinPwmChannels[i] == PWM_CHANNEL_2)
            pinPwmChannelMasks[i] = PWM_CHANNEL_2_MASK;
        else
            pinPwmChannelMasks[i] = PWM_CHANNEL_3_MASK;
    }

    PWM0_Initialize();
    PWM1_Initialize();
}

void LEDBoard::ledWrite(uint8_t ledIndex, bool ledValue) {
    PIO_PIN ledPin;
    if(ledIndex <= 4 && ledIndex > 0){
        ledPin = ledPins[ledIndex];
    }
    else return;

    PIO_PinWrite(ledPin, !ledValue);
}

void LEDBoard::setLedIntensity(uint8_t ledIndex, uint16_t intensity) {
    if(ledIndex <= 4 && ledIndex > 0) {
        if (pwmHandlers[ledIndex]) {
            uint16_t period = PWM1_ChannelPeriodGet(pinPwmChannels[ledIndex]);
            uint16_t dutyCycle = period * intensity / 100;
            PWM1_ChannelDutySet(pinPwmChannels[ledIndex], uint16_t
            dutyCycle);
            PWM1_ChannelsStart(pinPwmChannelMasks[ledIndex]);
        }
        else {
            uint16_t period = PWM0_ChannelPeriodGet(pinPwmChannels[ledIndex]);
            uint16_t dutyCycle = period * intensity / 100;
            PWM0_ChannelDutySet(pinPwmChannels[ledIndex], uint16_t
            dutyCycle);
            PWM0_ChannelsStart(pinPwmChannelMasks[ledIndex]);
        }
    }
    else return;
}

void LEDBoard::closeLed(uint8_t ledIndex) {
    if(ledIndex <= 4 && ledIndex > 0) {
        if (pwmHandlers[ledIndex]) {
            PWM1_ChannelsStop(pinPwmChannelMasks[ledIndex]);
        }
        else {
            PWM0_ChannelsStop(pinPwmChannelMasks[ledIndex]);
        }
    }
    else return;
}



