#pragma once

#include <cstdint>
#include <algorithm>
#include <etl/array.h>
#include <etl/vector.h>
#include <etl/span.h>
#include "Peripheral_Definitions.hpp"
#include "Task.hpp"
#include "Logger.hpp"
#include "definitions.h"

#if FGDOS_SPI_PORT == 0

#define FGDOS_Initialize SPI0_Initialize
#define FGDOS_WriteRead SPI0_WriteRead
#define FGDOS_Write SPI0_Write
#define FGDOS_Read SPI0_Read

#else

#define FGDOS_Initialize SPI1_Initialize
#define FGDOS_WriteRead SPI1_WriteRead
#define FGDOS_Write SPI1_Write
#define FGDOS_Read SPI1_Read

#endif

/**
 * @class FGDOS
 * @brief Interface for communicating with the Floating Gate Dosimeter (FGDOS) radiation sensors.
 */
class FGDOS {
public:
    /**
     * Constructor that initializes the FGDOS instance and sets the chip select pin to high by default.
     *
     * @param chipSelectPin The pin used for chip select (CS) in SPI communication.
     */
    explicit FGDOS(PIO_PIN chipSelectPin) : ChipSelectPin(chipSelectPin) {
        PIO_PinWrite(ChipSelectPin, true);
    };

    /**
     * @brief Retrieves the Chip ID from the FGDOS device.
     *
     * @return The Chip ID as a byte.
     */
    [[nodiscard]] uint8_t getChipID();

private:
    /**
     *The chip select pin for SPI communication.
     */
    PIO_PIN ChipSelectPin = PIO_PIN_NONE;

    /**
     *  Underlying type of the DeviceRegister enum class.
     */
    using DeviceRegisterAddressType_t = uint8_t;

    /**
     * @enum DeviceRegister
     * @brief Defines device register addresses for the FGDOS device.
     */
    enum DeviceRegister : DeviceRegisterAddressType_t {
        CHIPID = 0x13 /**< Register address for the Chip ID. */
    };

    /**
     * @brief Reads a value from a specified register.
     *
     * @param targetRegister The register to read from.
     * @return The value read from the register.
     */
    [[nodiscard]] uint8_t readRegister(DeviceRegister targetRegister) const;

    /**
     * @brief Writes data to a specified register.
     *
     * @param targetRegister The register to write to.
     * @param dataBuffer A span containing the data to be written.
     */
    void writeRegister(DeviceRegister targetRegister, etl::span<uint8_t> dataBuffer) const;
};
