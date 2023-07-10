#include "FlowValveDriver.hpp"

FlowValve::FlowValve(PIO_Pin openPin, PIO_Pin closePin) : openPin_(openPin), closePin_(closePin) {
    PIO_PinWrite(openPin_, false);
    close();
}

void FlowValve::open() {
    PIO_PinWrite(openPin_, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(openPin_, false);
}

void FlowValve::close() {
    PIO_PinWrite(closePin_, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(closePin_, false);
}