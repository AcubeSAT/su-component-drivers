#include "inc/FreeRTOSTasks/Heater.hpp"
#include "peripheral/pwm/plib_pwm0.h"

void Heater::startHeater() {
    PWM0_ChannelsStart(PWM_CHANNEL_2_MASK);
}

void Heater::stopHeater() {
    PWM0_ChannelsStop(PWM_CHANNEL_2_MASK);
}

void Heater::setDutyPercentage(float dutyValuePercentage) {
    PWM0_ChannelDutySet(PWM_CHANNEL_2, convertDutyCycleToMilliseconds(dutyValuePercentage));
}

float Heater::convertDutyCycleToMilliseconds(float dutyCyclePercentage) {
    return period * (dutyCyclePercentage / 100);
}

float Heater::period = PWM0_ChannelPeriodGet(PWM_CHANNEL_2);