#pragma once

#include <cstdint>
#include <etl/utility.h>
#include <etl/array.h>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

#define INA228_TWI_PORT 2

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
 * @brief C++ class for interfacing with the INA228 Current Monitor.
 *
 * This class provides functions to read various electrical parameters from the INA228 device,
 * such as voltage, temperature, current, power, energy, and charge.
 */
class INA228 {
public:
    uint8_t i2cAddress = 0b1000000;

    /**
     * @brief Constructor for the INA228 class.
     * @param something The I2C master port number used for communication.
     */
    INA228();

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

    /**
     * @brief Reads the bus voltage from the INA228 device. Resolution size 195.3125 μV/LSB
     * @return The bus voltage in volts.
     */
    float getVoltage();

    /**
     * Reads the internal die temperature from the INA228 device.
     *
     * @brief The temperature is calculated using a conversion factor 7.8125 m°C/LSB
     * @return The die temperature in degrees Celsius.
     */
    float getDieTemperature();

    /**
     * @brief Reads the energy value from the INA228 device.
     * @return The energy value in joules.
     */
    float getEnergy();

    /**
     * @brief Reads the shunt voltage from the INA228 device.
     * @return The shunt voltage in millivolts.
     */
    float getShuntVoltage();

private:

    /**
     * @brief The maximum expected current, used to calculate CurrentLSB
     */
    static constexpr float MaximumExpectedCurrent = 1;

    /**
     * The LSB step size for the CURRENT register where the current in Amperes is stored
     *
     * @brief While the smallest CurrentLSB value yields highest resolution,
     * it is common to select a higher round-number (no higher than 8x) value for the CurrentLSB
     * in order to simplify the conversion of the CURRENT
     */
    static constexpr float CurrentLSB = MaximumExpectedCurrent / (static_cast<float>(1 << 19));

    /**
     * Value of current-sensing resistor
     */
    static constexpr float RShuntResistor = 0.05;

    /**
     * Value that is going to be written in SHUNT_CAL register
     *
     * @brief The current is calculated following a shunt voltage measurement based on the value set
     * in the SHUNT_CAL register. If the value loaded into the SHUNT_CAL register is zero, the current
     * value reported through the CURRENT register is also zero
     */
    static constexpr uint16_t ShuntCalValue = 13107.2 * 1000000 * CurrentLSB * RShuntResistor;

    /**
     * Bit 4 of the CONFIG register
     *
     * @brief Configure this value in order to acquire shunt full scale range selection across IN+ and IN–.
     */
    static constexpr uint8_t ADCRange = 0;

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

    void readRegister(RegisterAddress registerAddress, uint8_t* returnedData, uint8_t numberOfBytesToRead);
};
