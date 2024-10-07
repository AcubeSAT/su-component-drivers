#include "Thermistor.hpp"

template<AFECPeripheral AfecPeripheral>
float Thermistor<AfecPeripheral>::getTemperature() {
    float outputVoltage = static_cast<float>(AFECGeneral<AfecPeripheral>::adcResult) / MaxADCValue * VrefAfec;
    double resistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * outputVoltage + R1 * PowerSupply) - R3;
    if (resistorValue < 166.71) {
        Temperature =
                -8.47506357770908 * pow(10, -6) * pow(resistorValue, 3) +
                0.00386892064896403 * pow(resistorValue, 2) -
                0.720748414692382 * resistorValue + 66.7732219851856;
    } else if (resistorValue < 402.32) {
        Temperature = 38.4859 - 0.1705 * resistorValue +
                      (1.8468 * pow(10, -4)) * resistorValue * resistorValue;
    } else {
        Temperature = 15.1909 - 0.0476 * resistorValue +
                      (1.5773 * pow(10, -5)) * resistorValue * resistorValue;
    }
    return Temperature;
}

template
class Thermistor<AFECPeripheral::AFEC0>;

template
class Thermistor<AFECPeripheral::AFEC1>;
