#include "Thermistor.hpp"
#include <cmath>


uint16_t Thermistor::getADCResult(AFEC_CALLBACK callback, uintptr_t context) {
    // Initialize AFEC0
    AFEC0_Initialize();

    // Enable the required ADC channels
    AFEC0_ChannelsEnable(AdcChannelMask);

    // Register the callback function
    AFEC0_CallbackRegister(ADCResultCallback,  reinterpret_cast<uintptr_t>(this));

    // Start the conversion
    AFEC0_ConversionStart();
    return AdcResult;
}


void Thermistor::ADCResultCallback(uint32_t status, uintptr_t context) {
    auto thermistor = reinterpret_cast<Thermistor*>(context);

    if (AFEC0_ChannelResultIsReady(thermistor->AdcChannelNumber)) {
        // Assuming 'context' is a pointer to the Thermistor instance
        thermistor->AdcResult = AFEC0_ChannelResultGet(thermistor->AdcChannelNumber);
        LOG_INFO << "ADC Result: " << thermistor->AdcResult;
    } else {
        LOG_ERROR << "AFEC0 channel result not ready";
    }
}

void Thermistor::OutputVoltageCalculation() {

    OutputVoltage = static_cast<float>(getADCResult(ADCResultCallback, 0))/ MaxADCValue * PowerSupply;
    LOG_DEBUG<<"OutputVoltage is : "<<OutputVoltage;
}

void Thermistor::Voltage2Resistance() {
    OutputVoltageCalculation();
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
