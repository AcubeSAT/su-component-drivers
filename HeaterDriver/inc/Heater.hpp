#pragma once

#include <cstdint>
#include "peripheral/pwm/plib_pwm0.h"
#include "peripheral/pwm/plib_pwm1.h"
#include "lib/atsam-component-drivers/AcubeSAT_HAL/PWM/inc/HAL_PWM.hpp"

template<uint8_t PWMPeripheral>
class Heater : public HAL_PWM {

public:


    /**
     * Different instances of the constructor
     * we can construct a heater either initializing a period value
     * or taking the default period value,
     * but we have to specify the PWM channel
     */
    Heater(uint16_t period, PWM_CHANNEL_MASK channelMask);

    Heater(PWM_CHANNEL_MASK channelMask);


    /**
     * This function enables the PWM channel
     * of the instance of the class we are each time
     * working with
     */
    void startHeater();

    /**
     * This function disables the PWM channel
     * of the instance of the class we are each time
     * working with
     */
    void stopHeater();

    /**
     * sets the Duty percentage of PWM channel
     * of the instance of the class we are each time
     * working with
     */
    void setDutyPercentage();

    /**
     *
     * @return dutyValue() takes the duty cycle percentage of the waveform as an argument
     * and returns the on-time of the waveform in ticks.
     * Only allows for values between 0 and 100
     */
    uint16_t convertDutyCyclePercentageToTicks();

private:

    /**
     * the period of the waveform in ticks
     */
    uint16_t period;

    /**
     * the duty Cycle Percentage (== heater On Time / period)
     */
    uint8_t dutyCyclePercentage;

    /**
     * The mask indicating which channels to start
     */
    PWM_CHANNEL_MASK channelMask;
};