#pragma once

#include <cstdint>
#include "peripheral/pwm/plib_pwm0.h"
#include "peripheral/pwm/plib_pwm1.h"
#include "HAL_PWM.hpp"

template<uint8_t PeripheralNumber>
class Heater : public HAL_PWM {

public:
    /**
     * @param period: The period of PWM (measured in ticks)
     *
     * @param channelMask: The mask indicating which channel to start
     *
     * @param pwmChannel: The Channel of PWM
     *
     * @brief An instance of the constructor that initializes the period value .
     *
     * @note The period can be changed after the construction of the Heater instance .
     *
     */
    Heater(uint16_t period, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel);

    /**
     * @param channelMask: The mask indicating which channel to start
     *
     * @param pwmChannel: The Channel of PWM
     *
     * @brief An instance of the constructor that doesn't initialize the period value.
     *
     * @note In this case, period takes its initial value from Harmony .
     *
     * @note The period can be changed after the construction of the Heater instance .
     *
     */
    Heater(PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel);

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
     * @param dutyCyclePercentage should be between 0 and 100
     *
     * @brief Sets the Duty percentage of PWM channel
     *  of the instance of the class we are each time
     *  working with.
     *
     * @note Duty percentage indicates the percentage of
     *  the OFF-time of the waveform
     */
    void setDutyPercentage(uint8_t dutyCyclePercentage) ;

    /**
     * @return dutyValue() in ticks
     *
     * Takes the duty cycle percentage of the waveform as an argument
     * and returns the OFF-time of the waveform in ticks.
     */
    uint16_t convertDutyCyclePercentageToTicks();

    /**
     * @param period: the period of the PWM
     *
     * @brief Sets the period of PWM channel
     *  of the instance of the class we are each time
     *  working with
     *
     *  @note The setPeriod function can only be used when
     *  the PWM channel is already started
     */
    void setPeriod(uint16_t period);

    /**
     * @return The period of the PWM
     */
    uint16_t getPeriod();

    /**
     * @return The Duty Cycle Percentage of the PWM
     */
    uint8_t getDutyCyclePercentage();
private:

    /**
     * The period of the waveform in ticks
     */
    uint16_t period = 15000;

    /**
     * The mask indicating which channel to start
     */
    const PWM_CHANNEL_MASK channelMask;

    /**
     * The PWM channel
     */
    const PWM_CHANNEL_NUM pwmChannel;

    /**
     * The Duty Cycle Percentage
     * (== The percentage of the period that the PWM is OFF)
     *
     * @note dutyCyclePercentage rvalue is between 0 and 100
     */
    uint8_t dutyCyclePercentage = 100;
};