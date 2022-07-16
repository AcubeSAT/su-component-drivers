#include <etl/array.h>
#include <cstdint>
#include "definitions.h"

#define LPS22HH_SPI_PORT 0

#if LPS22HH_SPI_PORT == 0
#define SPI_WriteRead SPI0_WriteRead
#else
#define SPI_WriteRead SPI1_WriteRead
#endif

/**
 * This is a generic driver implementation for LPS22HH, based on its datasheet.
 * @ingroup drivers
 * @see https://gr.mouser.com/datasheet/2/389/lps22hh-1395924.pdf
 * @author Efthymia Amarantidou <efthimia145@gmail.com>
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
    };

    /**
     * The pressure sensitivity value according to the datasheet. It is used to calculate the pressure.
     */
    static const uint16_t pressureSensitivity = 4096;

    /**
     * The temperature sensitivity value according to the datasheet. It is used to calculate the temperature.
     */
    static const uint8_t temperatureSensitivity = 100;

    /**
     * The temperature measurement
     */
    float temperatureValue;

    /**
     * The pressure measurement
     */
    float pressureValue;

    /**
     * Chip select pin of the SPI peripheral.
     */
    PIO_PIN ssn;

    /**
     * Reads from a specific register of the ADXRS453 device.
     * @param registerAddress is the value of a register address.
     */
    uint8_t readFromRegister(RegisterAddress registerAddress);

    /**
     * Writes a byte to a specific address.
     * @param registerAddress is the specific address in which a byte is going to be written.
     * @param data is the byte which will be written to the specific register.
     */
    void writeToRegister(uint8_t registerAddress, uint8_t data);

    /**
     * Get the STATUS of the sensor.
     * @return byte in the STATUS register of the sensor.
     */
    uint8_t getStatus();

public:
    /**
     * @param ssn is the chip select pin of the SPI peripheral.
     */
    LPS22HH(PIO_PIN ssn);

    /**
     * Checks if a new temperature data is generated.
     */
    bool temperatureDataAvailableCheck();

    /**
     * Checks if a new pressure data is generated.
     */
    bool pressureDataAvailableCheck();

    /**
     * Read the current pressure measurement.
     * @return the calculated pressure value.
     */
    void readPressure();

    /**
     * Read the current temperature measurement.
     * @return the calculated temperature value.
     */
    void readTemperature();

    /**
     * Get the current pressure measurement.
     * @return the calculated pressure value.
     */
    float getPressure(){
        return pressureValue;
    }

    /**
     * Get the current temperature measurement.
     * @return the calculated temperature value.
     */
    float getTemperature(){
        return temperatureValue;
    }

};
