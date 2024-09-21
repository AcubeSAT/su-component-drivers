#include "Thermistor.hpp"

template<AFECPeripheral AfecPeripheral>
uint16_t Thermistor<AfecPeripheral>::getADCResult() {
    if (AfecPeripheral == AFECPeripheral::AFEC0) {
        thermistorAdcResult = AFEC0_ChannelResultGet(afecChannelNum);
    }
    else {
        thermistorAdcResult = AFEC1_ChannelResultGet(afecChannelNum);
    }
    return thermistorAdcResult;
}

template<AFECPeripheral AfecPeripheral>
float Thermistor<AfecPeripheral>::getOutputVoltage() {
    float outputVoltage = static_cast<float>(getADCResult()) / MaxADCValue * VrefAfec;
    LOG_DEBUG << "OutputVoltage is : " << outputVoltage;
    return outputVoltage;
}

template<AFECPeripheral AfecPeripheral>
double Thermistor<AfecPeripheral>::getResistance() {
    double resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * getOutputVoltage() + R1 * PowerSupply) - R3;
    LOG_DEBUG << "Resistor value is :" << resistorValue;
    return resistorValue;
}

template<AFECPeripheral AfecPeripheral>
double Thermistor<AfecPeripheral>::getTemperature() {
    const double EquivalentResistance = getResistance();
    double temperature;
    if (EquivalentResistance < 166.71) {
        temperature =
                -8.47506357770908 * pow(10, -6) * pow(EquivalentResistance, 3) +
                0.00386892064896403 * pow(EquivalentResistance, 2) -
                0.720748414692382 * EquivalentResistance + 66.7732219851856;
    } else if (EquivalentResistance < 402.32) {
        temperature = 38.4859 - 0.1705 * EquivalentResistance +
                      (1.8468 * pow(10, -4)) * EquivalentResistance * EquivalentResistance;
    } else {
        temperature = 15.1909 - 0.0476 * EquivalentResistance +
                      (1.5773 * pow(10, -5)) * EquivalentResistance * EquivalentResistance;
    }
    return temperature;
}