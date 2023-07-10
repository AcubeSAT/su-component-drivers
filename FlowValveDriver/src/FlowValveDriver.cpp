#include "FlowValveDriver.hpp"

FlowValve::FlowValve(PIO_Pin openPin, PIO_Pin closePin) : openPin_(openPin), closePin_(closePin) {
    PIO_PinWrite(openPin_, false);
    PIO_PinWrite(closePin_, true);
    vTaskDelay(pdMS_TO_TICKS(50));
}

void FlowValve::open() {
    PIO_PinWrite(openPin_, true);
    vTaskDelay(pdMS_TO_TICKS(500));
    PIO_PinWrite(openPin_, false);
}

void FlowValve::close() {
    PIO_PinWrite(closePin_, true);
    vTaskDelay(pdMS_TO_TICKS(500));
    PIO_PinWrite(closePin_, false);
}