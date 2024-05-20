#pragma once

#include <cstdint>
#include "peripheral/pwm/plib_pwm0.h"
#include "peripheral/pwm/plib_pwm1.h"
#include "HAL_PWM.hpp"

template<uint8_t PeripheralNumber>
class Heater : public HAL_PWM {
public:
    /**
     * @param frequency: The frequency of PWM measured in kHz
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
    Heater(uint16_t frequency, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM pwmChannel);

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
    void setDutyCyclePercentage(uint8_t dutyCyclePercentage);

    /**
     * @param period: the period of the PWM measured in ticks
     *
     * @brief Sets the period of PWM channel
     *  of the instance of the class we are each time
     *  working with
     */
    void setPeriod(uint16_t period);

    /**
     * @param frequency: the frequency of the PWM measured in kHz
     *
     * @brief Sets the frequency of PWM channel
     *  of the instance of the class we are each time
     *  working with
     *
     */
    void setFrequency(uint16_t frequency);

    /**
     * @return The period of the PWM in ticks
     */
    uint16_t getPeriod() const;

    /**
    * @return The frequency of the PWM in kHz
    */
    uint16_t getFrequency() const;

    /**
     * @return The Duty Cycle Percentage of the PWM
     *
     * @note Duty percentage indicates the percentage of
     *  the OFF-time of the waveform
     */
    uint8_t getDutyCyclePercentage() const;

private:
    /**
     * Indicates whether or not the heater
     * has been started
     */
    bool heaterHasStarted = false;

    /**
     * The period of the waveform in ticks
     */
    uint16_t period = PWM_ChannelPeriodGet<PeripheralNumber>(pwmChannel);

    /**
     * The frequency of the waveform in kHz
     */
    uint16_t frequency = convertHarmonyPeriodToKHzFrequency();

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

    /**
     * @return dutyValue in ticks
     *
     * Converts the duty cycle percentage of the waveform
     * to the OFF-time of the waveform in ticks.
     */
    inline uint16_t convertDutyCyclePercentageToTicks() const { return (period * dutyCyclePercentage) / 100; }

    /**
     * @return Period in ticks
     *
     * Converts the frequency of the waveform measured in kHz
     * to Period measured in Harmony ticks.
     *
     * @warning If in the future we change the MCU frequency,
     * we must change this function accordingly.
     */
    inline uint16_t convertKHzFrequencyToHarmonyPeriod() const { return 1500 * (10 ^ 3 / frequency); }

    /**
     * @return frequency in kHz
     *
     * Converts the period of the waveform measured in Ticks
     * to frequency measured in kHz.
     *
     * @warning If in the future we change the MCU frequency,
     * we must change this function accordingly.
     */
    inline uint16_t convertHarmonyPeriodToKHzFrequency() const { return 1500 * (10 ^ 3 / period); }
};