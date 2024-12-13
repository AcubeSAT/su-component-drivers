#pragma once

#include <cstdint>
#include "peripheral/pio/plib_pio.h"
#include "FreeRTOS.h"
#include "Task.hpp"

/**
 * @class FlowValve
 *
 * Class for controlling a latching flow valve using GPIO pins.
 */
class FlowValve {
public:
    /**
     * Class constructor.
     *
     * @param openPin The GPIO pin number for opening the valve.
     * @param closePin The GPIO pin number for closing the valve.
     */
    explicit FlowValve(PIO_PIN openPin, PIO_PIN closePin) : OpenPin(openPin), ClosePin(closePin) {
        PIO_PinWrite(OpenPin, false);
        PIO_PinWrite(ClosePin, false);
    };

    /**
     * Function that opens the valve by sending a latching pulse to the open pin.
     * A delay is used to ensure smooth transition to the next latching state.
     */
    void openValve() const;

    /**
     * Function that closes the valve by sending a latching pulse to the close pin.
     * A delay is used to ensure smooth transition to the next latching state.
     */
    void closeValve() const;

    /**
     * The GPIO pin number for opening the valve.
     */
    const PIO_PIN OpenPin = PIO_PIN_NONE;

    /**
     * The GPIO pin number for closing the valve.
     */
    const PIO_PIN ClosePin = PIO_PIN_NONE;

private:
    /**
     * @enum ValveState
     *
     * Enum that contains the possible on/off states.
     */
    enum class ValveState : bool {
        OPEN = true,
        CLOSED = false,
    };

    /**
     * Pulse's time duration for latching operation in milliseconds.
     */
    static constexpr uint16_t LatchPulseDuration = 500;

};
