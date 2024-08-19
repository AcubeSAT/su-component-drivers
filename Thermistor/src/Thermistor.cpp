#include "Thermistor.hpp"
#include <cmath>

uint16_t Thermistor::getADCResult(AFEC_CALLBACK callback, uintptr_t context) {
    // Initialize AFEC0
    AFEC0_Initialize();

    // Enable the required ADC channels
    AFEC0_ChannelsEnable(AdcChannelMask);

    // Register the callback function
    AFEC0_CallbackRegister(ADCResultCallback, reinterpret_cast<uintptr_t>(this));

    // Start the conversion
    AFEC0_ConversionStart();
    return AdcResult;
}

void Thermistor::ADCResultCallback(uint32_t status, uintptr_t context) {
    auto thermistor = reinterpret_cast<Thermistor *>(context);

    if (AFEC0_ChannelResultIsReady(thermistor->AdcChannelNumber)) {
        // Assuming 'context' is a pointer to the Thermistor instance
        thermistor->AdcResult = AFEC0_ChannelResultGet(thermistor->AdcChannelNumber);
        LOG_INFO << "ADC Result: " << thermistor->AdcResult;
    } else {
        LOG_ERROR << "AFEC0 channel result not ready";
    }
}

void Thermistor::OutputVoltageCalculation() {
    OutputVoltage = static_cast<float>(getADCResult(ADCResultCallback, 0)) / MaxADCValue * VrefAfec;
    LOG_DEBUG << "OutputVoltage is : " << OutputVoltage;
}

void Thermistor::Voltage2Resistance() {
    OutputVoltageCalculation();
    ResistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * OutputVoltage + R1 * PowerSupply) - R3;
    LOG_DEBUG << "Resistor value is :" << ResistorValue;
}

void Thermistor::Resistance2Temperature() {
    Voltage2Resistance();
    if (ResistorValue < 166.71) {
        Temperature =
                -8.47506357770908 * pow(10, -6) * pow(ResistorValue, 3) + 0.00386892064896403 * pow(ResistorValue, 2) -
                0.720748414692382 * ResistorValue + 66.7732219851856;
    } else if (ResistorValue < 402.32 && ResistorValue > 166.71) {
        Temperature = 38.4859 - 0.1705 * ResistorValue + (1.8468 * pow(10, -4)) * ResistorValue * ResistorValue;
    } else {
        Temperature = 15.1909 - 0.0476 * ResistorValue + (1.5773 * pow(10, -5)) * ResistorValue * ResistorValue;
    }
}

double Thermistor::getTemperature() {
    Resistance2Temperature();
    return Temperature;
}