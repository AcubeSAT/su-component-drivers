#pragma once

#include <cstdint>

class Heater {

public:
    /**constructor
     *
     */
    Heater();

    /**This function disables the 2nd channel of the PWM0
     *
     */
    static void startHeater();

    /**This function enables the 2nd channel of the PWM0
     *
     */
    static void stopHeater();

    /**sets the Duty of PWM
     *
     * @param dutyCyclePercentage
     */
    void setDutyPercentage(uint8_t dutyCyclePercentage);

    /** dutyValue() takes the duty cycle percentage of the waveform as an argument
     * and returns the on-time of the waveform in ticks.
     */
    uint16_t convertDutyCyclePercentageToTicks(uint8_t dutyCyclePercentage);


private:

    /** the period of the waveform in ticks
     *
     */
    float period;

};