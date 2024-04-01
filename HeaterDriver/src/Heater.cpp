#include "inc/FreeRTOSTasks/heater.hpp"
#include "peripheral/pwm/plib_pwm0.h"

void heater::initializeHeater() {
    PWM0_Initialize();
}

//This function disables the 2nd channel of the PWM0
void heater::startHeater() {
    PWM0_ChannelsStart(PWM_CHANNEL_2_MASK);
}

//This function enables the 2nd channel of the PWM0
void heater::stopHeater() {
    PWM0_ChannelsStop(PWM_CHANNEL_2_MASK);
}

//sets the Duty of PWM
void heater::setDutyPercentage(float dutyValuePercentage) {
    PWM0_ChannelDutySet(PWM_CHANNEL_2, convertDutyCycleToMilliseconds(dutyValuePercentage));
}

/* dutyValue() takes the duty cycle percentage of the waveform as an argument
 * and returns the on-time of the waveform in ticks.
 */
float heater::convertDutyCycleToMilliseconds(float dutyCyclePercentage) {
    return period * (dutyCyclePercentage / 100);
}

float heater::period = PWM0_ChannelPeriodGet(PWM_CHANNEL_2);   // equals to the period of the waveform in ticks