#include "Thermistor.hpp"

template<AFECPeripheral AfecPeripheral>
etl::expected<float, bool> Thermistor<AfecPeripheral>::getTemperature() const {
    float outputVoltage = static_cast<float>(AfecGeneral<AfecPeripheral>::ADCResult) / MaxADCValue * VrefAfec;
    double resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * outputVoltage + R1 * PowerSupply) - R3;

    const double EquivalentResistance = resistorValue;
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

template
class Thermistor<AFECPeripheral::AFEC0>;

template
class Thermistor<AFECPeripheral::AFEC1>;
