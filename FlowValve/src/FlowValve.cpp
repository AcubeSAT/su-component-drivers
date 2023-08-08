#include "FlowValve.hpp"

FlowValve::FlowValve(PIO_PIN openPin, PIO_PIN closePin) : openPin(openPin), closePin(closePin) {
    PIO_PinWrite(openPin, false);
    PIO_PinWrite(closePin, false);

    if constexpr (CloseValveAtMCUStartup) {
        closeValve();
    }
}

void FlowValve::openValve() {
    PIO_PinWrite(openPin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(openPin, false);
}

void FlowValve::closeValve() {
    PIO_PinWrite(closePin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(closePin, false);
}

