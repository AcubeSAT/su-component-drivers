#include "INA228.hpp"

constexpr INA228::INA228() {

}

float INA228::getCurrent() {
    uint32_t current = static_cast<uint32_t>(readRegister());

    current = (current >> 4) & 0xFFFFF;

    uint8_t sign = current & 0x80000;

    if (sign == 1)
        current = (!current & 0xFFFFF) + 1;

    return static_cast<float>(current) * CurrentLSB;
}

float INA228::getPower() {
    uint32_t power = static_cast<uint32_t>(readRegister());

    return 3.2f * CurrentLSB * static_cast<float>(power);
}
