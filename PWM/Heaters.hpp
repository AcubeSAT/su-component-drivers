#pragma once

#include "Peripheral_Definitions.hpp"

namespace PayloadDrivers {

inline constexpr size_t NumberOfHeaters = 4;

template<PWM_PeripheralID PeripheralID>
class Heater {
public:
    /**
     *@brief Initializes the Heater and underlying PWM object, making it closed by default and setting the appropriate PWM frequency and duty cycle
     * @param channel the PWM channel used by the Heater
     * @param frequency the PWM frequency in Hz
     * @param dutyCycle percentage of duty cycle from 0 to 100
     */
    explicit Heater(PWM_PeripheralChannel channel, float frequency, float dutyCycle) : pwm(channel) {
        close();
        pwm.setFrequency(frequency);
        setDutyCycle(dutyCycle);

    }

    /**
     * Enables the Heater
     */
    void open() {
        pwm.start();
    }

    /**
     * Disables the Heater
     */
    void close() {
        pwm.stop();
    }

    /**
     * Sets the Heater's Duty Cycle
     * @param dutyCycle percentage of duty cycle from 0 to 100
     */
    void setDutyCycle(float dutyCycle) {
        pwm.setDutyCycle(dutyCycle);
    }

private:
    /**
     * Templated PWM instance for the given peripheral.
     */
    PWM<PeripheralID> pwm;

};

static_assert(Heaters_PWM_Config.size() == NumberOfHeaters, "PWM channel allocations for heaters are too many");
inline etl::optional<Heater<Heaters_PWM_Config[0].first>> heater1;
inline etl::optional<Heater<Heaters_PWM_Config[1].first>> heater2;
inline etl::optional<Heater<Heaters_PWM_Config[2].first>> heater3;
inline etl::optional<Heater<Heaters_PWM_Config[3].first>> heater4;

} // namespace PayloadDrivers
