#include "Thermistor.hpp"

etl::expected<float, bool> Thermistor::getTemperature() const {
    static_assert(MaxADCValue > 0 && VoltageValue > 0, "Value must be above zero");
    if (ResistorValue <= 0.0f) {
        return etl::unexpected(true);
    }
    const float voltageConversion = static_cast<float>(adcResult) / MaxADCValue * VoltageValue;
    const float currentConversion = voltageConversion / ResistorValue;
    return currentConversion - OffsetCurrent + ReferenceTemperature;
}