#pragma once

#include "Peripheral_Definitions.hpp"
#include "etl/array.h"

namespace PayloadDrivers {

inline constexpr size_t NumberOfHeaters = 4;

template<PWM_PeripheralID PeripheralID>
class Heater {
public:
    explicit Heater(PWM_PeripheralChannel channel) : pwm(channel) {}

    static constexpr uint32_t PWM_Frequency = 800;

    void open() {
        pwm.start();
    }

    void close() {
        pwm.stop();
    }

    void setAveragePower(uint32_t averagePower) {
        pwm.setDutyCycle(averagePower);
    }

private:
    /**
     * Templated PWM instance for the given peripheral.
     */
    PWM<PeripheralID> pwm;
};

static_assert(Heaters_PWM_Config.size() == NumberOfHeaters, "PWM channel allocations for heaters are too many");
inline Heater<Heaters_PWM_Config[0].first> heater1 {Heaters_PWM_Config[0].second};
inline Heater<Heaters_PWM_Config[1].first> heater2 {Heaters_PWM_Config[1].second};
inline Heater<Heaters_PWM_Config[2].first> heater3 {Heaters_PWM_Config[2].second};
inline Heater<Heaters_PWM_Config[3].first> heater4 {Heaters_PWM_Config[3].second};

} // namespace PayloadDrivers
