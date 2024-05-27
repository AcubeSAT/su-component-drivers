#pragma once

#include "Logger.hpp"
#include "HAL_PWM.hpp"

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK channelMask, PWM_CHANNEL_NUM channelNum>
class Heater : public HAL_PWM {
public:
    /**
     * @param frequency: The frequency of PWM measured in kHz
     *
     * @brief An instance of the constructor that initializes the frequency value .
     *
     * @note The frequency can be changed after the construction of the Heater instance .
     */
    Heater(uint32_t frequency);

    /**
     * @brief An instance of the constructor that doesn't initialize the frequency value.
     *
     * @note In this case, frequency takes its default value (=10000Hz).
     *
     * @note The frequency can be changed after the construction of the Heater instance .
     *
     */
    Heater();

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
     *  the ON-time of the waveform
     */
    void setDutyCyclePercentage(uint8_t dutyCyclePercentage);

    /**
     * @param period: the period of the PWM measured in ticks
     *
     * @brief Sets the period of PWM channel
     *  of the instance of the class we are each time
     *  working with
     */
    void setPeriodTicks(uint16_t periodTicks);

    /**
     * @param frequency: the frequency of the PWM measured in kHz
     *
     * @brief Sets the frequency of PWM channel
     *  of the instance of the class we are each time
     *  working with
     *
     */
    void setFrequency(uint32_t frequency);

    /**
     * @return The period of the PWM in ticks
     */
    uint16_t getPeriodTicks() const;

    /**
    * @return The frequency of the PWM in kHz
    */
    uint16_t getFrequency() const;

    /**
     * @return The Duty Cycle Percentage of the PWM
     *
     * @note Duty percentage indicates the percentage of
     *  the ON-time of the waveform
     */
    uint8_t getDutyCyclePercentage() const;

private:
    /**
     * The MCU clock frequency
     *
     * @warning: if in the future we change the MCU frequency,
     * we must change the value of this variable accordingly
     */
    static constexpr uint32_t clockFrequency = 150e6;

    /**
     * Indicates whether or not the heater
     * has been started
     */
    bool heaterHasStarted = false;

    /**
     * The period of the waveform in ticks
     */
    uint16_t periodTicks = 15e3;

    /**
     * The frequency of the waveform in kHz
     */
    uint32_t frequency = 10e3;

    /**
     * The Duty Cycle Percentage
     * (== The percentage of the period that the PWM is ON)
     *
     * @note dutyCyclePercentage rvalue is between 0 and 100
     */
    uint8_t dutyCyclePercentage = 0;

    /**
     * @return dutyValue in ticks
     *
     * Converts the duty cycle percentage of the waveform
     * to the ON-time of the waveform in ticks.
     */
    inline uint16_t convertDutyCyclePercentageToTicks() const {
        return (periodTicks * (100 - dutyCyclePercentage)) / 100;
    }

    /**
     * @return Period in ticks
     *
     * Converts the frequency of the waveform measured in Hz
     * to Period measured in Harmony ticks.
     */
    inline uint16_t convertHzFrequencyToHarmonyPeriod() const {
        return uint16_t(clockFrequency / frequency);
    }
};