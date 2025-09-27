#include "FlowValve.hpp"

bool FlowValve::openValve() const {
    if (valveIndex >= valveStates.size()) {
        LOG_ERROR << "Valve index invalid!";
        return false;
    }
    xSemaphoreTake(valveSemaphore.value(), portMAX_DELAY);
    if (valveStates[valveIndex] == ValveState::OPEN) {
        xSemaphoreGive(valveSemaphore.value());
        LOG_ERROR << "Valve is already open!";
        return false;
    }
    PIO_PinWrite(OpenPin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(OpenPin, false);
    setValveStateToFlash(valveIndex, ValveState::OPEN);
    valveStates[valveIndex] = ValveState::OPEN;
    xSemaphoreGive(valveSemaphore.value());

    return true;
}

bool FlowValve::closeValve() const {
    if (valveIndex >= valveStates.size()) {
        LOG_ERROR << "Valve index invalid!";
        return false;
    }
    xSemaphoreTake(valveSemaphore.value(), portMAX_DELAY);
    if (valveStates[valveIndex] == ValveState::CLOSED) {
        xSemaphoreGive(valveSemaphore.value());
        LOG_ERROR << "Valve is already closed!";
        return false;
    }
    PIO_PinWrite(ClosePin, true);
    vTaskDelay(pdMS_TO_TICKS(LatchPulseDuration));
    PIO_PinWrite(ClosePin, false);
    setValveStateToFlash(valveIndex, ValveState::CLOSED);
    valveStates[valveIndex] = ValveState::CLOSED;
    xSemaphoreGive(valveSemaphore.value());
    return true;
}

etl::optional<FlowValve::ValveState> FlowValve::getValveStateFromFlash(uint8_t index) {
    if (index >= valveStates.size()) {
        LOG_ERROR << "Valve index invalid!";
        return etl::nullopt;
    }
    etl::array<uint32_t, 2> savedValveStates{};
    auto result = flash->readFromMemory(savedValveStates, sizeof(uint32_t) * savedValveStates.size(), InternalFlashStorageAddress);
    if (result != FlashDriver::EFCError::NONE) {
        LOG_ERROR << "Flash Read Error:" << static_cast<int>(result);
        return etl::nullopt;
    }
    uint8_t state = reinterpret_cast<uint8_t*>(savedValveStates.data())[index];
    if (state == ValveClosedMagicNumber) {
        return ValveState::CLOSED;
    } else if (state != ValveOpenMagicNumber) {
        LOG_ERROR << "Read Invalid valve state. Assuming Open!";
    }
    return ValveState::OPEN;
}

void FlowValve::setValveStateToFlash(uint8_t index, ValveState state) {
    if (index >= valveStates.size()) {
        LOG_ERROR << "Invalid Valve Index";
        return;
    }
    etl::array<uint32_t, 4> savedValveStates{};
    auto readResult = flash->readFromMemory(savedValveStates, sizeof(uint32_t) * savedValveStates.size(), InternalFlashStorageAddress);
    if (readResult != FlashDriver::EFCError::NONE) {
        LOG_ERROR << "Flash Read Error before Write:" << static_cast<int>(readResult);
    }
    auto& savedState = reinterpret_cast<uint8_t*>(savedValveStates.data())[index];
    savedState = ((state == ValveState::OPEN) ? ValveOpenMagicNumber : ValveClosedMagicNumber);
    auto writeResult = flash->writeQuadWord(savedValveStates, InternalFlashStorageAddress);
    if (writeResult != FlashDriver::EFCError::NONE) {
        LOG_ERROR << "Flash write Error:" << static_cast<int>(writeResult);
    }
}
