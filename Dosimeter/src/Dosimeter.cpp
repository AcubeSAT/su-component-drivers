#include "Dosimeter.hpp"

uint8_t Dosimeter::readRegister(Dosimeter::RegisterAddress readRegister) {
    uint8_t commandAddressByte = static_cast<uint8_t >(readRegister) | SPI_READ_COMMAND;
    uint8_t registerValue = 0;

    DOSIMETER_SPI_WriteRead(&commandAddressByte, RegisterAddressSizeInBytes, &registerValue, RegisterSizeInBytes);

    return registerValue;
}

void Dosimeter::writeRegister(Dosimeter::RegisterAddress writeRegister, Dosimeter::RegisterSpecifiedValue registerSpecifiedValue, uint8_t data) {
    const uint8_t commandAddressByte = static_cast<uint8_t >(writeRegister) | SPI_WRITE_COMMAND;
    etl::array<uint8_t , RegisterSizeInBytes+RegisterAddressSizeInBytes> dataArray = {commandAddressByte, data};
    DOSIMETER_SPI_Write(dataArray.data(), RegisterSizeInBytes+RegisterAddressSizeInBytes);
}

void Dosimeter::setTargetRegister(uint8_t value) {

}

void Dosimeter::setThresholdRegister(uint8_t value) {

}