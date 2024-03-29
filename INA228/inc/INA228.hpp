#pragma once

#include <cstdint>
#include <type_traits>
#include <etl/utility.h>
#include <etl/array.h>
#include <etl/expected.h>
#include <etl/span.h>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

/**
 * The INA228_TWI_PORT definition is used to select which TWI peripheral of the ATSAMV71 MCU will be used.
 * By giving the corresponding value to INA228_TWI_PORT, the user can choose between TWI0, TWI1 or TWI2 respectively.
 */
#if INA228_TWI_PORT == 0

#include "plib_twihs0_master.h"
#define INA228_TWIHS_WriteRead TWIHS0_WriteRead
#define INA228_TWIHS_Write TWIHS0_Write
#define INA228_TWIHS_ErrorGet TWIHS0_ErrorGet
#define INA228_TWIHS_Read TWIHS0_Read
#define INA228_TWIHS_Initialize TWIHS0_Initialize
#define INA228_TWIHS_IsBusy TWIHS0_IsBusy

#elif INA228_TWI_PORT == 1

#include "plib_twihs1_master.h"
#define INA228_TWIHS_WriteRead TWIHS1_WriteRead
#define INA228_TWIHS_Write TWIHS1_Write
#define INA228_TWIHS_ErrorGet TWIHS1_ErrorGet
#define INA228_TWIHS_Read TWIHS1_Read
#define INA228_TWIHS_Initialize TWIHS1_Initialize
#define INA228_TWIHS_IsBusy TWIHS1_IsBusy

#elif INA228_TWI_PORT == 2

#include "plib_twihs2_master.h"

#define INA228_TWIHS_WriteRead TWIHS2_WriteRead
#define INA228_TWIHS_Write TWIHS2_Write
#define INA228_TWIHS_ErrorGet TWIHS2_ErrorGet
#define INA228_TWIHS_Read TWIHS2_Read
#define INA228_TWIHS_Initialize TWIHS2_Initialize
#define INA228_TWIHS_IsBusy TWIHS2_IsBusy
#endif

/**
 * Class for interfacing with the INA228 Current Monitor.
 *
 * @brief This class provides functions to read various electrical parameters from the INA228 device,
 * such as voltage, temperature, current, power, energy, and charge.
 */
class INA228 {
public:
    /**
     * @enum I2CAddress
     *
     * Contains all the possible I2C addresses, that can be configured by the two address pins, A0 and A1.
     */
    enum class I2CAddress : uint16_t {
        Address_1000000 = 0b100'0000, // A0 -> GND,  A1 -> GND
        Address_1000001 = 0b100'0001, // A0 -> Vs,   A1 -> GND
        Address_1000010 = 0b100'0010, // A0 -> SDA,  A1 -> GND
        Address_1000011 = 0b100'0011, // A0 -> SCL,  A1 -> GND

        Address_1000100 = 0b100'0100, // A0 -> GND,  A1 -> Vs
        Address_1000101 = 0b100'0101, // A0 -> Vs,   A1 -> Vs
        Address_1000110 = 0b100'0110, // A0 -> SDA,  A1 -> Vs
        Address_1000111 = 0b100'0111, // A0 -> SCL,  A1 -> Vs

        Address_1001000 = 0b100'1000, // A0 -> GND,  A1 -> SDA
        Address_1001001 = 0b100'1001, // A0 -> Vs,   A1 -> SDA
        Address_1001010 = 0b100'1010, // A0 -> SDA,  A1 -> SDA
        Address_1001011 = 0b100'1011, // A0 -> SCL,  A1 -> SDA

        Address_1001100 = 0b100'1100, // A0 -> GND,  A1 -> SCL
        Address_1001101 = 0b100'1101, // A0 -> Vs,   A1 -> SCL
        Address_1001110 = 0b100'1110, // A0 -> SDA,  A1 -> SCL
        Address_1001111 = 0b100'1111, // A0 -> SCL,  A1 -> SCL
    };

    /**
     * @enum Configuration
     *
     * Contains the various configurations of the INA228 device after power up.
     */
    enum class Configuration : uint16_t {
        Configuration1 = 0x00,    /// Default configuration (ADCRANGE = 0)
        Configuration2 = 0x10,    /// ADCRANGE = 1
    };

    /**
     * @enum ADCConfiguration
     *
     * Contains the various ADC configurations of the INA228 device after power up.
     */
    enum class ADCConfiguration : uint16_t {
        Configuration1 = 0b1111'1011'0110'1000,    /// Default configuration, Continuous measurements
        Configuration2 = 0b0111'1011'0110'1000,    /// Single-shot measurements of voltages and temperature
    };

    /**
     * Constructor for the INA228 class.
     *
     * @param i2cAddress The hardware configured I2C chip address.
     * @param configuration The configuration.
     * @param adcConfiguration The ADC configuration.
     * @param shuntResistor The hardware configured Rshunt.
     */
    INA228(I2CAddress i2cAddress, Configuration configuration, ADCConfiguration adcConfiguration, float shuntResistor)
            : I2CChipAddress(i2cAddress), ConfigurationSelected(configuration),
              ADCConfigurationSelected(adcConfiguration),
              ShuntResistor(shuntResistor) {
        setupConfigurationRegisters();
    }

    /**
     * Constructor for the INA228 class.
     *
     * @param i2cAddress The hardware configured I2C chip address.
     */
    [[maybe_unused]] explicit INA228(I2CAddress i2cAddress) : I2CChipAddress(i2cAddress) {
        setupConfigurationRegisters();
    }

    /**
     * Constructor for the INA228 class.
     *
     * @param i2cAddress The hardware configured I2C chip address.
     * @param shuntResistor The hardware configured Rshunt.
     */
    [[maybe_unused]] INA228(I2CAddress i2cAddress, float shuntResistor) : I2CChipAddress(i2cAddress),
                                                                          ShuntResistor(shuntResistor) {
        setupConfigurationRegisters();
    }

    /**
     * Constructor for the INA228 class.
     */
    INA228() {
        setupConfigurationRegisters();
    }

    /**
     * Function that reads the current measurements from the INA228 device.
     *
     * @return The current measurement (in Amperes).
    */
    [[nodiscard]] float getCurrent() const;

    /**
     * Function that reads the power measurements from the INA228 device.
     *
     * @return The power measurement (in Watts).
     */
    [[nodiscard]] float getPower() const;

    /**
     * Function that reads the bus voltage measurements from the INA228 device.
     *
     * @brief Resolution size 195.3125 μV/LSB.
     *
     * @return The bus voltage measurement (in Volts).
     */
    [[nodiscard]] float getVoltage() const;

    /**
     * Function that reads the internal die temperature from the INA228 device.
     *
     * @brief The temperature is calculated using a conversion factor 7.8125 m°C/LSB.
     *
     * @return The die temperature (in Celsius).
     */
    [[nodiscard]] float getDieTemperature() const;

    /**
     * Function that reads the energy measurements from the INA228 device.
     *
     * @return The energy measurement (in Joules).
     */
    [[nodiscard]] double getEnergy() const;

    /**
     * Function that reads the shunt voltage measurements from the INA228 device.
     *
     * @return The shunt voltage measurement (in milliVolts).
     */
    [[nodiscard]] float getShuntVoltage() const;

private:
    /**
     * The hardware configured I2C chip address of the INA228 device.
     */
    const I2CAddress I2CChipAddress = I2CAddress::Address_1000000;

    /**
     * The user selected configuration option for CONFIG register.
     */
    const Configuration ConfigurationSelected = Configuration::Configuration1;

    /**
     * The user selected configuration option for ADC_CONFIG register.
     */
    const ADCConfiguration ADCConfigurationSelected = ADCConfiguration::Configuration1;

    /**
     * The maximum expected current, used to calculate CurrentLSB.
     */
    const float MaximumExpectedCurrent = 1.0f;

    /**
     * The LSB step size for the CURRENT register where the current in Amperes is stored.
     *
     * @brief While the smallest CurrentLSB value yields highest resolution,
     * it is common to select a higher round-number (no higher than 8x) value for the CurrentLSB
     * in order to simplify the conversion of the CURRENT.
     */
    const float CurrentLSB = MaximumExpectedCurrent / (static_cast<float>(uint32_t{1} << 19));

    /**
     * Value of current-sensing resistor (in Ohms).
     */
    const float ShuntResistor = 0.05f;

    /**
     * Value that is going to be written in SHUNT_CAL register.
     *
     * @brief The current is calculated following a shunt voltage measurement based on the value set
     * in the SHUNT_CAL register. If the value loaded into the SHUNT_CAL register is zero, the current
     * value reported through the CURRENT register is also zero.
     */
    const uint16_t ShuntCalValue = [=]() -> uint16_t {
        float ShuntCalFloat = round(13107.2f * 1000000.0f * CurrentLSB * ShuntResistor);

        if (ConfigurationSelected == Configuration::Configuration2) {
            ShuntCalFloat = ShuntCalFloat * 4.0f;
        }

        return static_cast<uint16_t>(ShuntCalFloat);
    }();

    /**
     * Underlying type of the RegisterAddress enum.
     */
    using RegisterAddress_t = uint8_t;

    /**
    * @enum RegisterAddress
    *
    * Contains the addresses of all the INA228 registers.
    */
    enum class RegisterAddress : RegisterAddress_t {
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
        DEVICE_ID = 0x3F,
    };

    /**
     * Underlying type of the RegisterAddress enum.
     */
    using RegisterBytesNumber_t = uint8_t;

    /**
    * @enum RegisterBytesNumber
    *
    * Contains the number of bytes that each of the INA228 registers has.
    */
    enum class RegisterBytesNumber : RegisterBytesNumber_t {
        CONFIG = 2,
        ADC_CONFIG = 2,
        SHUNT_CAL = 2,
        SHUNT_TEMPCO = 0x03,
        VSHUNT = 3,
        VBUS = 3,
        DIETEMP = 2,
        CURRENT = 3,
        POWER = 3,
        ENERGY = 5,
        CHARGE = 5,
        DIAG_ALRT = 0,
        SOVL = 0,
        SUVL = 0,
        BOVL = 0,
        BUVL = 0,
        TEMP_LIMIT = 0,
        PWR_LIMIT = 0,
        MANUFACTURER_ID = 0,
        DEVICE_ID = 0,
    };

    /**
     * Type alias for representing the current register data (3 bytes).
     */
    using Current_t = uint32_t;

    /**
     * Type alias for representing the bus voltage register data (3 bytes).
     */
    using BusVoltage_t = uint32_t;

    /**
     * Type alias for representing the shunt voltage register data (3 bytes).
     */
    using ShuntVoltage_t = uint32_t;

    /**
     * Type alias for representing the power register data (3 bytes).
     */
    using Power_t = uint32_t;

    /**
     * Type alias for representing the shunt energy register data (5 bytes).
     */
    using Energy_t = uint64_t;

    /**
     * Type alias for representing the shunt die temp register data (2 bytes).
     */
    using DieTemp_t = uint16_t;

    /**
     * Function that sets up the device's registers CONFIG, ADC_CONFIG, and SHUNT_CAL on power-up.
     *
     * @brief The SHUNT_CAL register provides the device with a conversion constant value that represents shunt
     * resistance used to calculate current value (in Amperes).
     */
    void setupConfigurationRegisters() const;

    /**
     * Function that decodes the returned data from an array of bytes to a binary number.
     *
     * @tparam NUMBER_OF_BYTES The number of bytes in the input array. Must be in the range [2, 5].
     * @tparam T The type of the returned numeric value. Defaults to uint64_t.
     * @param returnedData The array of bytes containing the data to be decoded.
     * @return The decoded binary number.
     */
    template<uint8_t NUMBER_OF_BYTES, typename T = uint64_t>
    T decodeReturnedData(const etl::array<uint8_t, NUMBER_OF_BYTES> &returnedData) const;

    /**
     * Function that reads from a specified register of the INA228 device.
     *
     * @param registerAddress The address of the register.
     * @return True if the I2C transaction was completed successfully.
     */
    template<uint8_t RETURNED_BYTES>
    etl::array<uint8_t, RETURNED_BYTES> readRegister(RegisterAddress registerAddress) const;

    /**
     * Function that writes to a specified register of the INA228 device.
     *
     * @param data The data sent to the specified register as an array of bytes.
     * @return True if the I2C transaction was completed successfully.
     */
    [[nodiscard]] bool writeRegister(etl::span<uint8_t> data) const;
};
