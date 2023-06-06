#include "TLC5940.hpp"

TLC5940::TLC5940(PIO_PIN latchPin, PIO_PIN blankPin, PIO_Pin gsclkPin) : latchPin(latchPin),
                                                                         blankPin(blankPin),
                                                                         gsclkPin(gsclkPin) {
    PIO_PinWrite(latchPin, false);
    PIO_PinWrite(blankPin, true);
    setAllPWM(static_cast<uint16_t>(0))
}

void TLC5940::begin() {
    writeData();
    latchData();
    setBlank(false);
}

void TLC5940::setPWM(uint8_t channel, uint16_t value) {
    // Check for invalid channel number
    if (channel >= MaxChannels)
        return;

    // Set the PWM value for the specified channel
    pwmData.at(channel) = value;
}

void TLC5940::setAllPWM(uint16_t value) {
    // Set the same PWM value for all channels
    for (size_t i = 0; i < MaxChannels; i++) {
        pwmData.at(i) = value;
    }
}

void TLC5940::update() {
    setBlank(true);
    begin();
}

void TLC5940::sendSPI(uint16_t data) {
    // Convert the 16-bit data into an array of bytes
    uint8_t txData[2];
    txData[0] = static_cast<uint8_t>(data >> 8); // MSB 8-15
    txData[1] = static_cast<uint8_t>(data & 0xFF); // LSB 0-7

    // Send the data and ignore any received data
    SPI1_WriteRead(txData, sizeof(txData), nullptr, 0);
}

void TLC5940::writeData() {
    // Iterate over the stored PWM data and send it to the TLC5940 chip
    for (uint16_t data : pwmData) {
        sendSPI(data);
    }
}

void TLC5940::setBlank(bool enableBlank) {
    PIO_PinWrite(blankPin, enableBlank);
}

void TLC5940::latchData() {
    PIO_PinWrite(latchPin, true);
    PIO_PinWrite(latchPin, false);
}
