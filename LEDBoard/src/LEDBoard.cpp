#include "LEDBoard.hpp"

LEDBoard::LEDBoard(PIO_PIN *pins, PWM_CHANNEL_NUM *channels, bool *handlers) {
    for (uint8_t i = 0; i < NumberOfLeds; i++) {
        ledPins[i] = pins[i];
        pinPwmChannels[i] = channels[i];
        pwmHandlers[i] = handlers[i];
        PIO_PinWrite(ledPins[i], true);

        switch (pinPwmChannels[i]) {
            case PWM_CHANNEL_0:
                pinPwmChannelMasks[i] = PWM_CHANNEL_0_MASK;
                break;
            case PWM_CHANNEL_1:
                pinPwmChannelMasks[i] = PWM_CHANNEL_1_MASK;
                break;
            case PWM_CHANNEL_2:
                pinPwmChannelMasks[i] = PWM_CHANNEL_2_MASK;
                break;
            case PWM_CHANNEL_3:
                pinPwmChannelMasks[i] = PWM_CHANNEL_3_MASK;
                break;
        }
    }

    PWM0_Initialize();
    PWM1_Initialize();
}

void LEDBoard::ledWrite(LedIndex ledIndex, bool ledValue) {
    PIO_PIN ledPin = ledPins[ledIndex];
    PIO_PinWrite(ledPin, !ledValue);
}

void LEDBoard::setLedIntensity(LedIndex ledIndex, uint16_t intensity) {
    if (pwmHandlers[ledIndex]) {
        uint16_t period = PWM1_ChannelPeriodGet(pinPwmChannels[ledIndex]);
        uint16_t dutyCycle = period * intensity / 100.0;
        PWM1_ChannelDutySet(pinPwmChannels[ledIndex], uint16_t
        dutyCycle);
        PWM1_ChannelsStart(pinPwmChannelMasks[ledIndex]);
    } else {
        uint16_t period = PWM0_ChannelPeriodGet(pinPwmChannels[ledIndex]);
        uint16_t dutyCycle = period * intensity / 100.0;
        PWM0_ChannelDutySet(pinPwmChannels[ledIndex], uint16_t
        dutyCycle);
        PWM0_ChannelsStart(pinPwmChannelMasks[ledIndex]);
    }
}

void LEDBoard::closeLed(LedIndex ledIndex) {
    if (pwmHandlers[ledIndex]) {
        PWM1_ChannelsStop(pinPwmChannelMasks[ledIndex]);
    } else {
        PWM0_ChannelsStop(pinPwmChannelMasks[ledIndex]);
    }
}



