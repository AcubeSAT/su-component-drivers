#include "Thermistor.hpp"

uint16_t Thermistor::getADCResult() {
    bool status;
    AFEC0_Initialize();
    AFEC0_ChannelsEnable(AdcChannelMask);
    AFEC0_ConversionStart();
    status = AFEC0_ChannelResultIsReady(AdcChannelNumber);
    if (status) {
        AdcResult = AFEC0_ChannelResultGet(AdcChannelNumber);
        return AdcResult;
    } else {
        LOG_ERROR << "AFEC0 channel result not ready";
        return 0;
    }
}

void Thermistor::VoltageValueCalculation() {
    getADCResult();
    OutputVoltage = static_cast<float>(AdcResult) / MaxADCValue * PowerSupply;
}

void Thermistor::Voltage2Resistance() {
    ResistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * OutputVoltage + R2 * PowerSupply) - R3;
}

void Thermistor::Resistance2Temperature() {
    Voltage2Resistance();
    if (ResistorValue < 166.71) {
        Temperature = 0.0001 * pow(ResistorValue, 2) +
                      -0.0743 * ResistorValue + 21.5320;
    } else if (ResistorValue < 402.32 && ResistorValue > 166.71) {
        Temperature = 0.0004 * pow(ResistorValue, 2) +
                      -0.2277 * ResistorValue + 42.2494;
    } else {
        Temperature = 0.0039 * pow(ResistorValue, 2) +
                      -0.7207 * ResistorValue + 66.7732;
    }
}

double Thermistor::getTemperature() {
    Resistance2Temperature();
    return Temperature;
}