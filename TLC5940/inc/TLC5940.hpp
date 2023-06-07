#include <cstdint>
#include <etl/array.h>

/**
 * TLC5940 class for controlling TLC5940
 */
class TLC5940 {
public:

    /**
     * @brief TLC5940 constructor
     * @param latchPin Pin connected to the LATCH (LAT) pin of the TLC5940
     * @param blankPin Pin connected to the BLANK (BLK) pin of the TLC5940
     */
    TLC5940(PIO_Pin latchPin, PIO_Pin blankPin, PIO_Pin gsclkPin);

    /**
     * @brief Initialize the TLC5940
     */
    void begin();

    /**
     * @brief Set the PWM value for a specific channel
     * @param channel Channel index (0 to MaxChannels - 1)
     * @param value PWM value (0 to 4095)
     */
    void setPWM(uint8_t channel, uint16_t value);

    /**
     * @brief Set the PWM value for all channels
     * @param value PWM value (0 to 4095)
     */
    void setAllPWM(uint16_t value);

    /**
     * @brief Update the TLC5940 to apply the changes made to the PWM values
     */
    void update();

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

private:

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
     */
    void setBlank(bool enableBlank);

    /**
     * XLAT pin
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
     * PWM data array
     */
    etl::array<uint16_t, MaxChannels * MaxDrivers> pwmData;

};
