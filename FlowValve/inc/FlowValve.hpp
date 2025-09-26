#pragma once

#include <cstdint>
#include "peripheral/pio/plib_pio.h"
#include "FreeRTOS.h"
#include "Task.hpp"
#include "semphr.h"
#include "InternalFlash.hpp"

/**
 * @class FlowValve
 *
 * Class for controlling a latching flow valve using GPIO pins.
 */
class FlowValve {
public:
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
     * Class constructor.
     *
     * @param openPin The GPIO pin number for opening the valve.
     * @param closePin The GPIO pin number for closing the valve.
     * @param index Index of this valve, from 0 to 7. Used to save its state
     */
    explicit FlowValve(PIO_PIN openPin, PIO_PIN closePin, uint8_t index) : OpenPin(openPin), ClosePin(closePin), valveIndex{index} {
        PIO_PinWrite(OpenPin, false);
        PIO_PinWrite(ClosePin, false);
        if (not flash.has_value()) {
            flash.emplace();
        }
        if (index >= valveStates.size()) {
            LOG_ERROR << "Invalid valve index";
            return;
        }
        if (not valveSemaphore) {
            valveSemaphore.emplace(xSemaphoreCreateBinaryStatic(&valveSemaphoreBuffer));
            xSemaphoreGive(valveSemaphore.value());
        }
        valveStates[index] = getValveStateFromFlash(index);

    };

    /**
     * Function that opens the valve by sending a latching pulse to the open pin.
     * A delay is used to ensure smooth transition to the next latching state.
     * @return false if the valve was already open or invalid
     */
    [[nodiscard]] bool openValve() const;

    /**
     * Function that closes the valve by sending a latching pulse to the close pin.
     * A delay is used to ensure smooth transition to the next latching state.
     * @return false if the valve was already closed or invalid
     */
    [[nodiscard]] bool closeValve() const;

    /**
     * Gets the cached state of the valve
     * @return nullopt if the valve is invalid, opened or closed otherwise
     */
    [[nodiscard]] etl::optional<ValveState> getValveState() const {
        if (valveIndex >= valveStates.size()) {
            return etl::nullopt;
        }
        return valveStates[valveIndex];
    }

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
     * Flash driver instanced. Initialized with the Singleton Pattern
     */
    inline static etl::optional<FlashDriver> flash{};

    /**
     * Semaphore used to protect valve operations
     */
    inline static etl::optional<SemaphoreHandle_t> valveSemaphore;

    /**
     * Buffer for valve semaphore
     */
    inline static StaticSemaphore_t valveSemaphoreBuffer;

    /**
     * Index of this valve. Used to save its state
     */
    const uint8_t valveIndex;

    /**
     * Array of all valve states. Populated by reading from internal flash and
     */
    inline static etl::array<ValveState, 8> valveStates{ValveState::OPEN, ValveState::OPEN, ValveState::OPEN, ValveState::OPEN, ValveState::OPEN, ValveState::OPEN, ValveState::OPEN, ValveState::OPEN};

    /**
     * Retreives the valve state from internal flash
     * @param index valve index from 0 to 7
     * @return The valve state, either open or closed. As a failsafe, in case of error the valve is considered open
     */
    static ValveState getValveStateFromFlash(uint8_t index);
    /**
     * Stores the valve state to internal flash. In case of invalid index ore error the state is not saved
     * @param index the valve index from 0 to 7
     * @param state the state, either open or closed
     */
    static void setValveStateToFlash(uint8_t index, ValveState state);

    /**
     * Pulse's time duration for latching operation in milliseconds.
     */
    static constexpr uint16_t LatchPulseDuration = 500;
    /**
     * Address to store valve state in internal flash
     */
    static constexpr uint32_t InternalFlashStorageAddress = 0x5F0000;
    /**
     * Arbitrary value to store if a valve is closed
     * @note this value was chosen to make it difficult for bitflips to change it
     */
    static constexpr uint8_t ValveClosedMagicNumber = 0b10101010;
    /**
     * Arbitrary value to store if a valve is closed
     * @note this value was chosen to make it difficult for bitflips to change it
     */
    static constexpr uint8_t ValveOpenMagicNumber = 0b11001100;
};
