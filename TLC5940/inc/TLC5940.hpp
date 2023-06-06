#include <SPI.h>
#include <stdint.h>
#include <etl/array.h>

class TLC5940 {
public:
  static constexpr size_t MaxChannels = 16;
  static constexpr size_t MaxDrivers = 4;

  TLC5940(uint8_t numDrivers, uint8_t latchPin, uint8_t blankPin, uint8_t dcPin, uint8_t spiClock, uint8_t spiData);

  void begin();
  void setPWM(uint16_t channel, uint16_t value);
  void setAllPWM(uint16_t value);
  void update();

private:
  uint8_t numDrivers;
  uint16_t numChannels;
  uint16_t numPWM;
  uint8_t latchPin;
  uint8_t blankPin;
  uint8_t dcPin;
  uint8_t spiClock;
  uint8_t spiData;
  etl::array<uint16_t, MaxChannels * MaxDrivers> pwmData;

  void spiSend(uint16_t data);
  void writeData();
  void setLatch();
  void setBlank();

};
