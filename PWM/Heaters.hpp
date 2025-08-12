#pragma once

#include "Peripheral_Definitions.hpp"

namespace PayloadDrivers {

inline constexpr size_t NumberOfHeaters = 4;

template<PWM_PeripheralID PeripheralID>
class Heater {
public:
    /**
     *
     * @param channel the PWM channel used by the Heater
     * @param frequency the PWM frequency in Hz
     * @param dutyCycle percentage of duty cycle from 0 to 100
     */
    explicit Heater(PWM_PeripheralChannel channel, float frequency, float dutyCycle) : pwm(channel) {
        pwm.setFrequency(frequency);
        setDutyCycle(dutyCycle);
        close();

    }


    void open() {
        pwm.start();
    }

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

constexpr float DefaultHeaterFrequency = 1500.0f;
constexpr float DefaultHeaterDutyCycle = 28.0f;

static_assert(Heaters_PWM_Config.size() == NumberOfHeaters, "PWM channel allocations for heaters are too many");
inline Heater<Heaters_PWM_Config[0].first> heater1 {Heaters_PWM_Config[0].second, DefaultHeaterFrequency, DefaultHeaterDutyCycle};
inline Heater<Heaters_PWM_Config[1].first> heater2 {Heaters_PWM_Config[1].second, DefaultHeaterFrequency, DefaultHeaterDutyCycle};
inline Heater<Heaters_PWM_Config[2].first> heater3 {Heaters_PWM_Config[2].second, DefaultHeaterFrequency, DefaultHeaterDutyCycle};
inline Heater<Heaters_PWM_Config[3].first> heater4 {Heaters_PWM_Config[3].second, DefaultHeaterFrequency, DefaultHeaterDutyCycle};

} // namespace PayloadDrivers
