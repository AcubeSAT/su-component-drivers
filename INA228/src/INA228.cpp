#include "INA228.hpp"

constexpr INA228::INA228() {

}

float INA228::getCurrent() {
    int32_t current = readFromRegister();

    uint8_t sign = current & (1 << 12);

    if(sign == 1)
        current = !current;

    return current * CurrentLSB;
}

float INA228::getPower() {
    uint32_t power = static_cast<uint32_t>(readRegister());

    return 3.2f * CurrentLSB * static_cast<float>(power) / 256.0f;
}

