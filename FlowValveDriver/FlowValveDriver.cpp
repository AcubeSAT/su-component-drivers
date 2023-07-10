#include "FlowValveDriver.hpp"

FlowValve::FlowValve(PIO_Pin openPin, PIO_Pin closePin) : openPin_(openPin), closePin_(closePin) {
    PIO_PinWrite(openPin_, false);
    PIO_PinWrite(closePin_, true);
}

void FlowValve::open() {
    PIO_PinWrite(closePin_, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    PIO_PinWrite(openPin_, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    PIO_PinWrite(openPin_, false);
}

void FlowValve::close() {
    PIO_PinWrite(openPin_, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    PIO_PinWrite(closePin_, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulating valve operation
    PIO_PinWrite(closePin_, false);
}