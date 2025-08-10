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
     * @param averagePower percentage of duty cycle from 0 to 100
     */
    explicit Heater(PWM_PeripheralChannel channel, float frequency,float averagePower) : pwm(channel) {
        pwm.setFrequency(frequency);
        setAveragePower(averagePower);
        close();

    }


    void open() {
        pwm.start();
    }

    void close() {
        pwm.stop();
    }

    void setAveragePower(float averagePower) {
        pwm.setDutyCycle(averagePower);
    }

private:
    /**
     * Templated PWM instance for the given peripheral.
     */
    PWM<PeripheralID> pwm;
};

constexpr float DEFAULT_HEATER_FREQUENCY=1500.0f;
constexpr float DEFAULT_HEATER_DUTY_CYCLE=28.0f;

static_assert(Heaters_PWM_Config.size() == NumberOfHeaters, "PWM channel allocations for heaters are too many");
inline Heater<Heaters_PWM_Config[0].first> heater1 {Heaters_PWM_Config[0].second,DEFAULT_HEATER_FREQUENCY,DEFAULT_HEATER_DUTY_CYCLE};
inline Heater<Heaters_PWM_Config[1].first> heater2 {Heaters_PWM_Config[1].second,DEFAULT_HEATER_FREQUENCY,DEFAULT_HEATER_DUTY_CYCLE};
inline Heater<Heaters_PWM_Config[2].first> heater3 {Heaters_PWM_Config[2].second,DEFAULT_HEATER_FREQUENCY,DEFAULT_HEATER_DUTY_CYCLE};
inline Heater<Heaters_PWM_Config[3].first> heater4 {Heaters_PWM_Config[3].second,DEFAULT_HEATER_FREQUENCY,DEFAULT_HEATER_DUTY_CYCLE};

} // namespace PayloadDrivers
