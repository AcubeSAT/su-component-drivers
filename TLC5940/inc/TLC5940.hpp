#include <cstdint>
#include <etl/array.h>

/**
 * A class for controlling TLC5940 using the ATSAMV71Q21B MCU.
 *
 * @class TLC5940
 * @brief Provides methods to set the PWM values for individual channels,
 * update the TLC5940 to apply the changes, and control the timing
 * and behavior of the LED driver.
 * @author [ggkogkou](https://gitlab.com/ggkogkou)
 */
class TLC5940 {
public:

    /**
     * Maximum number of channels per TLC5940 driver
     */
    static constexpr size_t MaxChannels = 16;

    /**
     * Maximum number of TLC5940 drivers
     */
    static constexpr size_t MaxDrivers = 1;

    /**
     * Frequency of GSCLK clock signal
     */
    static constexpr uint16_t GSCLKFrequency = 0x4E20;

    /**
     * @brief TLC5940 constructor
     * @param latchPin Pin connected to the LATCH (LAT) pin of the TLC5940
     * @param blankPin Pin connected to the BLANK (BLK) pin of the TLC5940
     */
    constexpr TLC5940(PIO_Pin latchPin, PIO_Pin blankPin, PIO_Pin gsclkPin);

    /**
     * @brief Initialize the PWM data
     * @param initData An array containing the initial PWM data
     */
    void initialize(etl::array<uint16_t, MaxChannels * MaxDrivers> initData);

    /**
     * @brief Start the Grayscale cycle
     */
    void begin();

    /**
     * @brief Set the PWM value for a specific channel
     * @param channel Channel index (0 to MaxChannels - 1)
     * @param value PWM value (0 to 4095)
     */
    inline void setPWM(uint8_t channel, uint16_t value);

    /**
     * @brief Set the PWM value for all channels
     * @param value PWM value (0 to 4095)
     */
    void setAllPWM(uint16_t value);

    /**
     * @brief Update the TLC5940 to apply the changes made to the PWM values
     */
    void update();

private:

    /**
     * Latch (XLAT) pin
     */
    PIO_Pin latchPin;

    /**
     * BLANK pin
     */
    PIO_Pin blankPin;

    /**
     * GSCLK pin
     */
    PIO_Pin gsclkPin;

    /**
     * Number of GSCLK pulses
     */
    static constexpr uint16_t numberOfPulses = 0x1000;

    /**
     * PWM data array
     */
    etl::array<uint16_t, MaxChannels * MaxDrivers> pwmData;

    /**
     * @brief Send data to the TLC5940 via SPI
     * @param data Data to be sent
     */
    void sendSPI(uint16_t data);

    /**
     * @brief Write the PWM data to the TLC5940
     */
    void writePWMData();

    /**
     * @brief Set the XLAT (LATCH) pin to latch the PWM data
     */
    void latchData();

    /**
     * @brief Set the BLANK pin to apply the PWM data to the outputs
     * @param enableBlank Set the pin HIGH or LOW
     */
    void setBlank(bool enableBlank);

    /**
     * @brief Generate GSCLK pulses through GPIO
     * @param numberOfPulses The number of pulses, usually 4096
     */
    void generateGSCLKPulses();

};
