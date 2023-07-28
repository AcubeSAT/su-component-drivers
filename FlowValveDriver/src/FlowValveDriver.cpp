#include "FlowValveDriver.hpp"

FlowValve::FlowValve(PIO_PIN openPin, PIO_PIN closePin) : openPin_(openPin), closePin_(closePin) {
    PIO_PinWrite(openPin_, false);
    PIO_PinWrite(closePin_, false);
//    close();
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