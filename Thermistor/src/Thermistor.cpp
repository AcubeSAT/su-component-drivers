#include "Thermistor.hpp"

Thermistor::Thermistor(AFEC_CHANNEL_MASK AdcChannelMask) : AdcChannelMask(AdcChannelMask), AdcChannelNumber(
        THERMISTOR_PORT == 0 ? AFEC_CH0 : AFEC_CH1) {}

uint16_t Thermistor::getADCResult() {
    AFEC_Initialize();
    AFEC_ChannelsEnable(AdcChannelMask);
    AFEC_CallbackRegister(ADCResultCallback, reinterpret_cast<uintptr_t>(this));
    AFEC_ConversionStart();
    return adcResult;
}

void Thermistor::ADCResultCallback(uint32_t status, uintptr_t context) {
    auto thermistor = reinterpret_cast<Thermistor *>(context);

    if (AFEC_ChannelResultIsReady(thermistor->AdcChannelNumber)) {
        thermistor->adcResult = AFEC_ChannelResultGet(thermistor->AdcChannelNumber);
        LOG_DEBUG << "ADC Result: " << thermistor->adcResult;
    } else {
        LOG_ERROR << "AFEC channel result not ready";
    }
}

float Thermistor::getOutputVoltage() {
    float outputVoltage = static_cast<float>(getADCResult()) / MaxADCValue * VrefAfec;
    LOG_DEBUG << "OutputVoltage is : " << outputVoltage;
    return outputVoltage;
}

double Thermistor::getResistance() {
    double resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * getOutputVoltage() + R1 * PowerSupply) - R3;
    LOG_DEBUG << "Resistor value is :" << resistorValue;
    return resistorValue;
}

double Thermistor::getTemperature() {
    const double EquivalentResistance = getResistance();
    double temperature;
    if (EquivalentResistance < 166.71) {
        temperature =
                -8.47506357770908 * pow(10, -6) * pow(EquivalentResistance, 3) +
                0.00386892064896403 * pow(EquivalentResistance, 2) -
                0.720748414692382 * EquivalentResistance + 66.7732219851856;
    } else if (EquivalentResistance < 402.32 && EquivalentResistance > 166.71) {
        temperature = 38.4859 - 0.1705 * EquivalentResistance +
                      (1.8468 * pow(10, -4)) * EquivalentResistance * EquivalentResistance;
    } else {
        temperature = 15.1909 - 0.0476 * EquivalentResistance +
                      (1.5773 * pow(10, -5)) * EquivalentResistance * EquivalentResistance;
    }
    return temperature;
}