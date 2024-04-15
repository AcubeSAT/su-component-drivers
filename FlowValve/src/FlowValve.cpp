#include "FlowValve.hpp"

void FlowValve::openValve() const {
    PIO_PinWrite(OpenPin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(OpenPin, false);
}

void FlowValve::closeValve() const {
    PIO_PinWrite(ClosePin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(ClosePin, false);
}
