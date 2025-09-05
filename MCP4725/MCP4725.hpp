#pragma once
#include <cstdint>
#include <etl/expected.h>
#include <FreeRTOS.h>
#include <etl/optional.h>
#include "task.h"
#include "plib_twihs0_master.h"

/**
 * @class Driver for the MCP4725 DAC
 * @note Used to control the reference voltage for the pump
 */
class MCP4725 {
    public:
    enum class Error {
            NO_ERROR,
            INVALID_ARGUMENT,
            WRITE_FAIL,
            EEPROM_BUSY
            };

    /**
     * Constructs the driver and reads voltage from the DAC's EEPROM
     * @param i2cAddress The hardware I2C address of the DAC
     * @param maxVoltage Maximum Voltage the DAC can output, equal to the supply voltage
     */
    MCP4725(uint8_t i2cAddress,float maxVoltage);

    /**
     * Sets the output voltage, also programming the EEPROM to preserver the value after power down
     * @param voltage voltage in volts (must be between 0 - supply voltage)
     * @return Returns WRITE_FAIL if I2C write failed, EEPROM_BUSY if a previous write to EEPROM is still in progress, or INVALID_ARGUMENT
     */
    inline Error setVoltagePersistent(float voltage) {
        return setVoltage<false>(voltage);
    }
    /**
        * Sets the output voltage but does not program EEPROM. The value is nto preserved after power down
        * @param voltage voltage in volts (must be between 0 - supply voltage)
        * @return Returns WRITE_FAIL if I2C write failed, EEPROM_BUSY if the command fails due to EEPROM being updated, or INVALID_ARGUMENT
        */
    Error setVoltageRegister(float voltage) {
        return setVoltage<true>(voltage);
    }

    /**
     * Gets the cached voltage value that the DAC is configured at
     * @return Returns the cached voltage in Volts read from EEPROM or writen to the register. If the EEPROM was never read from successfully, zero is returned
     */
    inline float getVoltage() const {
        return rawToVolts(voltageRaw);
    }
private:
    /**
     * 12 bit value read from the DAC Register
     */
    uint16_t voltageRaw;

    /**
     * Supply voltage for the DAC
     */
    float maxVoltage;

    /**
     * I2C address of the DAC
     */
    uint8_t address;

    /**
     * Command byte for register-only write command
     */
    constexpr static uint8_t WriteRegisterCommandByte = 0b01000000;

    /**
     * Command byte for register and EEPROM write command
     */
    constexpr static uint8_t WriteAllCommandByte = 0b01100000;

    /**
     * Raw voltage value for max voltage
     */
    constexpr static uint16_t MaxVoltageRaw = 0b111111111111;

    /**
     * Converts the raw data from the DAC register to Voltage
     * @param raw 12 bit raw register value
     * @return The converted voltage in Volts
     */
    inline float rawToVolts(uint16_t raw) const {
        return (maxVoltage * raw) / MaxVoltageRaw;
    }

    /**
     * Function that sends the command to update the Voltage register (and possibly the EEPROM)
     * @tparam WriteToEEPROM When true the data is also written to EEPROM, otherwise is is written only to the DAC register
     * @param voltage the voltage in Volts
     * @return (see setVoltagePersistent and setVoltageRegister for possible errors)
     */
    template<bool WriteToEEPROM>
    Error setVoltage(float voltage);
    /**
     * Reads all data from the DAC register and EEPROM
     * @return the read data or nothing on read failure
     */
    etl::optional<etl::array<uint8_t, 5>> readRegisters() const;
};

