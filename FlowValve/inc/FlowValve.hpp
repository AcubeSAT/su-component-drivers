#pragma once

#include <cstdint>
#include "peripheral/pio/plib_pio.h"
#include "FreeRTOS.h"
#include "Task.hpp"

/**
 * @class FlowValve
 * @brief Class for controlling a latching flow valve using GPIO pins.
 */
class FlowValve {
private:

    /**
     * Pulse's time duration for latching operation
     */
    static constexpr uint16_t LatchPulseDuration = 500;

    /**
     * The GPIO pin number for opening the valve.
     */
    const PIO_PIN openPin;

    /**
     * The GPIO pin number for closing the valve.
     */
    const PIO_PIN closePin;

    /**
     * A variable to select whether to close the valve, thus changing the state of the valve if it was open, each time the MCU boots.
     */
    static inline constexpr bool CloseValveAtMCUStartup = true;

public:

    /**
     * @brief Constructs a FlowValve object.
     * @param openPin The GPIO pin number for opening the valve.
     * @param closePin The GPIO pin number for closing the valve.
     */
    explicit FlowValve(PIO_PIN openPin, PIO_PIN closePin);

    /**
     * @brief Opens the valve by sending a latching pulse to the open pin. A delay is used to ensure smooth transition
     * to the next latching state.
     */
    void openValve();

    /**
     * @brief Closes the valve by sending a latching pulse to the close pin. A delay is used to ensure smooth transition
     * to the next latching state.
     */
    void closeValve();
};

