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
            return AdcResult;
        } else {
            LOG_ERROR << "AFEC0 channel result not ready";
        }
    } else if (AdcChannelMask == AFEC_CH1_MASK) {
        AFEC1_Initialize();
        AFEC1_ChannelsEnable(AFEC_CH0);
        AFEC1_ConversionStart();
        status = AFEC1_ChannelResultIsReady(AdcChannelNumber);
        if (status) {
            AdcResult = AFEC1_ChannelResultGet(AdcChannelNumber);
            return AdcResult;
        } else {
            LOG_ERROR << "AFEC1 channel result not ready";
        }
    }
}

float VoltageValueCalculation() {
    getADCResult();
    VoltageValue = static_cast<float>(AdcResult) / MaxADCValue * PowerSupply;
    return VoltageValue
}

const void Thermistor::Voltage2Resistance() {
    double r3 = 1;
    double r4 = 3.57;
    double r2 = 301;
    ResistorValue = r2 * PowerSupply * (r4 + r3) / ((r4 + r3) * VoltageValue + r4 * PowerSupply) - r2;
}

double Thermistor::Resistance2Temperature() {
    Voltage2Resistance();
    if (ResistorValue < 166.71) {
        Temperature = 0.0001 * pow(ResistorValue, 2) +
                      -0.0743 * ResistorValue + 21.5320;
        return Temperature;
    } else if (ResistorValue < 402.32 && ResistorValue > 166.71) {
        Temperature = 0.0004 * pow(ResistorValue, 2) +
                      -0.2277 * ResistorValue + 42.2494;
        return Temperature;
    } else {
        Temperature = 0.0039 * pow(ResistorValue, 2) +
                      -0.7207 * ResistorValue + 66.7732;
        return Temperature;
    }
}

float Thermistor::getTemperature() const {
    Resistance2Temperature();
    return Temperature;
}