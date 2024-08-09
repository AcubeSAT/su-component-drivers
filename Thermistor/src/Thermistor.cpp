#include "Thermistor.hpp"


uint16_t Thermistor::getADCResult() {
    bool status;
    if (AdcChannelMask == AFEC_CH0_MASK) {
        AFEC0_Initialize();
        AFEC0_ChannelsEnable(AFEC_CH0);
        AFEC0_ConversionStart();
        status = AFEC0_ChannelResultIsReady(AdcChannelNumber);
        if (status) {
            AdcResult = AFEC0_ChannelResultGet(AdcChannelNumber);
        }
        return AdcResult;
    }
    else if(AdcChannelMask == AFEC_CH1_MASK) {
        AFEC1_Initialize();
        AFEC1_ChannelsEnable(AFEC_CH0);
        AFEC1_ConversionStart();
        status = AFEC1_ChannelResultIsReady(AdcChannelNumber);
        if (status) {
            AdcResult = AFEC1_ChannelResultGet(AdcChannelNumber);
        }
        return AdcResult;
    }
}

const void Thermistor::Voltage2Resistance() {
    double r3 = 1;
    double r4 = 3.57;
    double r2 = 301;
    ResistorValue = r2 * 5 * (r4 + r3) / ((r4 + r3) * VoltageValue + PowerSupply) - r1;
}

etl::expected<float, bool> Thermistor::getTemperature() const {
    static_assert(MaxADCValue > 0 && VoltageValue > 0, " MaxADCValue and VoltageValue andValue must be above zero");
    if (ResistorValue <= 0.0f) {
        return etl::unexpected(true);
    }
    const float voltageConversion = static_cast<float>(AdcResult) / MaxADCValue * VoltageValue;
    Voltage2Resistance();
    const float currentConversion = voltageConversion / ResistorValue;
    return currentConversion - OffsetCurrent + ReferenceTemperature;
}