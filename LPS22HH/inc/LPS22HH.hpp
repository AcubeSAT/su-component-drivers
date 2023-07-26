#pragma once

#include <etl/array.h>
#include <cstdint>
#include "definitions.h"
#include "Task.hpp"

#define LPS22HH_SPI_PORT 0

#if LPS22HH_SPI_PORT == 0
#define LPS22HH_SPI_WriteRead SPI0_WriteRead
#define LPS22HH_SPI_Write   SPI0_Write
#define LPS22HH_SPI_Read    SPI0_Read
#define LPS22HH_SPI_IsBusy  SPI0_IsBusy

#elif LPS22HH_SPI_PORT == 1
#define LPS22HH_SPI_WriteRead SPI1_WriteRead
#define LPS22HH_SPI_Write   SPI1_Write
#define LPS22HH_SPI_Read    SPI1_Read
#define LPS22HH_SPI_IsBusy  SPI1_IsBusy

#elif LPS22HH_SPI_PORT == 2
#define LPS22HH_SPI_WriteRead SPI2_WriteRead
#define LPS22HH_SPI_Write   SPI2_Write
#define LPS22HH_SPI_Read    SPI2_Read
#define LPS22HH_SPI_IsBusy  SPI2_IsBusy
#endif

/**
 * This is a generic driver implementation for LPS22HH based on the SPI protocol.
 * @ingroup drivers
 * @see https://gr.mouser.com/datasheet/2/389/lps22hh-1395924.pdf
 */
class LPS22HH {
private:
    enum RegisterAddress : uint8_t {
        CTRL_REG1 = 0x10,
        CTRL_REG2 = 0x11,
        CTRL_REG3 = 0x12,
        FIFO_CTRL = 0x13,
        FIFO_STATUS1 = 0x25,
        FIFO_STATUS2 = 0x26,
        STATUS = 0x27,
        PRESSURE_OUT_XL = 0x28,
        PRESSURE_OUT_L = 0x29,
        PRESSURE_OUT_H = 0x2A,
        TEMP_OUT_L = 0x2B,
        TEMP_OUT_H = 0x2C,
        FIFO_DATA_OUT_PRESS_XL = 0x78,
        FIFO_DATA_OUT_PRESS_L = 0x79,
        FIFO_DATA_OUT_PRESS_H = 0x7A,
        FIFO_DATA_OUT_TEMP_L = 0x7B,
        FIFO_DATA_OUT_TEMP_H = 0x7C,
        WHO_AM_I = 0x0F
    };

    /**
     * SPI Command Type (Write/Read)
     */
    enum SPICommandType : uint8_t {
        SPI_WRITE_COMMAND = 0x0,
        SPI_READ_COMMAND = 0x80,
    };

    /**
     *
     */
    inline static constexpr uint8_t whoAmIRegisterDefaultValue = 0b10110011;

    /**
     * The output data rate (Hz) set in Control Register 1
     */
    enum OutputDataRate : uint8_t {
        ONE_SHOT = 0x0,
        HZ1 = 0x1,
        HZ10 = 0x2,
        HZ25 = 0x3,
        HZ50 = 0x4,
        HZ75 = 0x5,
        HZ100 = 0x6,
        HZ200 = 0x7,
    };

    enum FIFOModes : uint8_t {
        BYPASS = 0x0,
        FIFO = 0x1,
        CONTINUOUS = 0x2,
        BYPASS_TO_FIFO = 0x1,
        BYPASS_TO_CONTINUOUS = 0x2,
        CONTINUOUS_TO_FIFO = 0x3,
    };

    /**
     * Wait period before for abandoning an SPI transfer because the send/receive buffer does not get unloaded/gets loaded.
     */
    static inline constexpr uint16_t TimeoutTicks = 1000;


    /**
     * The pressure sensitivity value according to the datasheet in LSB/hPa. It is used to calculate the pressure.
     */
    static const uint16_t PressureSensitivity = 4096;

    /**
     * The temperature sensitivity value according to the datasheet in LSB/°C. It is used to calculate the temperature.
     */
    static const uint8_t TemperatureSensitivity = 100;

    /**
     * The temperature measurement in °C.
     */
    float temperatureValue;

    /**
     * The pressure measurement in hPa.
     */
    float pressureValue;

    /**
     * Chip select pin of the SPI peripheral.
     */
    PIO_PIN ssn;

    /**
     * Maximum number of bytes that are written in registers.
     */
    static const uint8_t MaxWriteBytes = 3;

    /**
     * Reads from a specific register of the LPS22HH device.
     * @param registerAddress is the value of a register address.
     */
    [[nodiscard]] uint8_t readFromRegister(RegisterAddress registerAddress) const;

    /**
     * Writes a byte to a specific address.
     * @param registerAddress is the specific address in which a byte is going to be written.
     * @param data is the byte which will be written to the specific register.
     */
    void writeToRegister(RegisterAddress registerAddress, uint8_t data) const;

    /**
     * Get the STATUS of the sensor.
     * @return byte in the STATUS register of the sensor.
     */
    uint8_t getStatus();

public:
    /**
     * @param ssn the chip select pin for the SPI protocol
     */
    LPS22HH(PIO_PIN ssn) : ssn(ssn) {
        PIO_PinWrite(ssn, true);
    };

    /**
     * Read the current pressure measurement in hPa.
     * @return the calculated pressure value.
     */
    float readPressure();

    /**
     * Read the current temperature measurement in °C.
     * @return the calculated temperature value.
     */
    float readTemperature();

    /**
     * Sets the Output Data Rate bits for the Control Register 1 (CTRL_REG1(10h))
     * @param rate ODR bits
     */
    void setODRBits(OutputDataRate rate);

    /**
     * Sets the bit that defines the use of the FIFO watermark level in the FIFO Control Register (FIFO_CTRL(13h))
     * @param stopOnWTM flag to enable the mode or not
     */
    void setStopOnWTM(bool stopOnWTM);

    /**
     * Sets the bit that enables triggered FIFO modes in the FIFO Control Register (FIFO_CTRL(13h))
     * @param trigMode flag to enable trigger mode or not
     */
    void setTrigModes(bool trigMode);

    /**
     * Sets the FIFO mode in the FIFO Control Register (FIFO_CTRL(13h))
     */
    void setFIFOMode(FIFOModes mode);

    /**
     * Activates ONE_SHOT bit of CTRL_REG2 in order for the sensor to acquire a measuremnt
     */
    void triggerOneShotMode();

    /**
     *
     */
    void performAreYouAliveCheck();

    /**
     *
     */
    void waitForTransfer() const;
};
