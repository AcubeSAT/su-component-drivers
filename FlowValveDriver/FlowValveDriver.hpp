#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

/**
 * @class FlowValve
 * @brief Class for controlling a latching flow valve using GPIO pins.
 */
class FlowValve {
public:

    /**
     * @brief Constructs a FlowValve object.
     * @param openPin The GPIO pin number for opening the valve.
     * @param closePin The GPIO pin number for closing the valve.
     */
    constexpr FlowValve(PIO_Pin openPin, PIO_Pin closePin) : openPin_(openPin), closePin_(closePin);

    /**
     * @brief Opens the valve by sending a latching pulse to the open pin. A delay is used to ensure smooth transition
     * to the next latching state.
     */
    void open();

    /**
     * @brief Closes the valve by sending a latching pulse to the close pin. A delay is used to ensure smooth transition
     * to the next latching state.
     */
    void close();

private:

    /**
     * The GPIO pin number for opening the valve.
     */
    PIO_Pin openPin_;

    /**
     * The GPIO pin number for closing the valve.
     */
    PIO_Pin closePin_;

};
