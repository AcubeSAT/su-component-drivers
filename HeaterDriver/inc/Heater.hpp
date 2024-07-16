#pragma once

#include "Logger.hpp"
#include "HAL_PWM.hpp"

template<uint8_t PeripheralNumber, PWM_CHANNEL_MASK ChannelMask, PWM_CHANNEL_NUM ChannelNum>
class Heater {
public:
    /**
     * @param frequency: The frequency of PWM measured in Hz
     *
     * @param dutyCyclePercentage: The percentage of ON time of the PWM
     *
     * @brief An instance of the constructor that initializes the frequency value .
     *
     * @note The frequency can be changed after the construction of the Heater instance .
     */
    Heater(uint32_t frequency, uint8_t dutyCyclePercentage);

    /**
     * @param frequency: The frequency of PWM measured in Hz
     *
     * @brief An instance of the constructor that initializes the frequency value .
     *
     * @note The frequency can be changed after the construction of the Heater instance .
     */
    Heater(uint32_t frequency);

    /**
     * @brief An instance of the constructor that doesn't initialize the frequency value.
     *
     * @note In this case, frequency takes its default value (1250Hz).
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
     * @param frequency: the frequency of the PWM measured in Hz
     *
     * @brief Sets the frequency of PWM channel
     *  of the instance of the class we are each time
     *  working with
     */
    void setFrequency(uint32_t frequency);

    /**
     * @return The period of the PWM in ticks
     */
    uint16_t getPeriodTicks() const;

    /**
     * @return The frequency of the PWM in Hz
     */
    uint32_t getFrequency() const;

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
     * The divider of the clock used for PWM
     *
     * @warning : if we change clockDivider from the harmony settings,
     * we must change the value of this variable accordingly.
     *
     * @note: In harmony: channel clock = clock A divider
     */
    static constexpr uint16_t clockDivider = 8;

    /**
     * Indicates whether or not the heater
     * has been started
     */
    bool heaterHasStarted = false;

    /**
     * The period of the waveform in ticks
     */
    uint16_t periodTicks = 15000;

    /**
     * The frequency of the waveform in mHz
     *
     * @warning: The default value of  frequency is dependent on the
     * default value of periodTicks , clockFrequency and clockDivider.
     *
     * @note: frequency is of range [287 , 18750000]
     */

    uint32_t frequency = 9765;

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
    inline uint16_t convertmHzFrequencyToHarmonyPeriod() const {
        return (clockFrequency) / (frequency * clockDivider);
    }

    /**
     * @return Frequency in Hz
     *
     * Converts the Period of the waveform measured in Ticks
     * to Frequency measured in Hz.
     */
    inline uint32_t convertHarmonyPeriodTomHzFrequency() const {
        return (clockFrequency) / (periodTicks * clockDivider);
    }

    /**
     * @param period: the period of the PWM measured in ticks
     *
     * @brief Sets the period of PWM channel
     *  of the instance of the class we are each time
     *  working with
     */
    void setPeriodTicks(uint16_t periodTicks);
};