#pragma once

#include <cstdint>

/**
 * @brief C++ class for interfacing with the INA228 Current Monitor.
 *
 * This class provides functions to read various electrical parameters from the INA228 device,
 * such as voltage, temperature, current, power, energy, and charge.
 */
class INA228 {
public:

    /**
     * @brief Constructor for the INA228 class.
     * @param something The I2C master port number used for communication.
     */
    constexpr INA228();

    /**
     * @brief Reads the current value from the INA228 device.
     * @return The current value in amperes.
    */
    float getCurrent();

    /**
     * @brief Reads the power value from the INA228 device.
     * @return The power value in watts.
     */
    float getPower();


private:

    /**
     * @brief The maximum expected current, used to calculate CurrentLSB
     */
    static constexpr float MaximumExpectedCurrent = 500;

    /**
     * The LSB step size for the CURRENT register where the current in Amperes is stored
     *
     * @brief While the smallest CurrentLSB value yields highest resolution,
     * it is common to select a higher round-number (no higher than 8x) value for the CurrentLSB
     * in order to simplify the conversion of the CURRENT
     */
    static constexpr float CurrentLSB = MaximumExpectedCurrent / static_cast<float>(1 << 19);

    /**
     * Value of current-sensing resistor
     */
    static constexpr float RShuntResistor = 100;

    /**
     * Value that is going to be written in SHUNT_CAL register
     *
     * @brief The current is calculated following a shunt voltage measurement based on the value set
     * in the SHUNT_CAL register. If the value loaded into the SHUNT_CAL register is zero, the current
     * value reported through the CURRENT register is also zero
     */
    static constexpr float SHUNT_CAL = 13107.2 * 1000000 * CurrentLSB * RShuntResistor;

	/**
     * @enum Holds the addresses of the INA228 registers
     */
    enum class RegisterAddress : uint8_t {
        CONFIG = 0x00,
        ADC_CONFIG = 0x01,
        SHUNT_CAL = 0x02,
        SHUNT_TEMPCO = 0x03,
        VSHUNT = 0x04,
        VBUS = 0x05,
        DIETEMP = 0x06,
        CURRENT = 0x07,
        POWER = 0x08,
        ENERGY = 0x09,
        CHARGE = 0x0A,
        DIAG_ALRT = 0x0B,
        SOVL = 0x0C,
        SUVL = 0x0D,
        BOVL = 0x0E,
        BUVL = 0x0F,
        TEMP_LIMIT = 0x10,
        PWR_LIMIT = 0x11,
        MANUFACTURER_ID = 0x3E,
        DEVICE_ID = 0x3F
    };

};
