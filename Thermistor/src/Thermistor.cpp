#include "Thermistor.hpp"

uint16_t Thermistor::getADCResult() {
    // Initialize AFEC
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
        LOG_ERROR << "AFEC channel result not ready";
    }
}

float Thermistor::getOutputVoltage() {
    outputVoltage = static_cast<float>(getADCResult(ADCResultCallback, 0)) / MaxADCValue * VrefAfec;
    LOG_DEBUG << "OutputVoltage is : " << outputVoltage;
    return outputVoltage;
}

double Thermistor::getResistance() {
    resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * getOutputVoltage() + R1 * PowerSupply) - R3;
    LOG_DEBUG << "Resistor value is :" << resistorValue;
    return resistorValue;
}

double Thermistor::getTemperature() {
    getResistance();
    if (getResistance() < 166.71) {
        temperature =
                -8.47506357770908 * pow(10, -6) * pow(getResistance(), 3) + 0.00386892064896403 * pow(getResistance(), 2) -
                0.720748414692382 * getResistance() + 66.7732219851856;
    } else if (getResistance() < 402.32 && getResistance() > 166.71) {
        temperature = 38.4859 - 0.1705 * getResistance() + (1.8468 * pow(10, -4)) * getResistance() * getResistance();
    } else {
        temperature = 15.1909 - 0.0476 * getResistance() + (1.5773 * pow(10, -5)) * getResistance() * getResistance();
    }
    return temperature;
}