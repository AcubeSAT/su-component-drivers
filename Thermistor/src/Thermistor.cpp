#include "Thermistor.hpp"
#include <cmath>

uint16_t Thermistor::getADCResult() {
    // Initialize AFEC0
    AFEC_Initialize();

    // Enable the required ADC channels
    AFEC_ChannelsEnable(AdcChannelMask);

    // Register the callback function
    AFEC_CallbackRegister(ADCResultCallback, reinterpret_cast<uintptr_t>(this));

    // Start the conversion
    AFEC_ConversionStart();
    return adcResult;
}

void Thermistor::ADCResultCallback(uint32_t status, uintptr_t context) {
    auto thermistor = reinterpret_cast<Thermistor *>(context);

    if (AFEC_ChannelResultIsReady(thermistor->AdcChannelNumber)) {
        // Assuming 'context' is a pointer to the Thermistor instance
        thermistor->adcResult = AFEC_ChannelResultGet(thermistor->AdcChannelNumber);
        LOG_DEBUG << "ADC Result: " << thermistor->adcResult;
    } else {
        LOG_ERROR << "AFEC0 channel result not ready";
    }
}

void Thermistor::calculateOutputVoltage() {
    outputVoltage = static_cast<float>(getADCResult(ADCResultCallback, 0)) / MaxADCValue * VrefAfec;
    LOG_DEBUG << "OutputVoltage is : " << outputVoltage;
}

void Thermistor::voltage2Resistance() {
    outputVoltageCalculation();
    resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * outputVoltage + R1 * PowerSupply) - R3;
    LOG_DEBUG << "Resistor value is :" << resistorValue;
}

void Thermistor::resistance2Temperature() {
    voltage2Resistance();
    if (resistorValue < 166.71) {
        temperature =
                -8.47506357770908 * pow(10, -6) * pow(resistorValue, 3) + 0.00386892064896403 * pow(resistorValue, 2) -
                0.720748414692382 * resistorValue + 66.7732219851856;
    } else if (resistorValue < 402.32 && resistorValue > 166.71) {
        temperature = 38.4859 - 0.1705 * resistorValue + (1.8468 * pow(10, -4)) * resistorValue * resistorValue;
    } else {
        temperature = 15.1909 - 0.0476 * resistorValue + (1.5773 * pow(10, -5)) * resistorValue * resistorValue;
    }
}

double Thermistor::getTemperature() {
    resistance2Temperature();
    return temperature;
}