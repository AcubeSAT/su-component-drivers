#include "Dosimeter.hpp"

template<typename F, typename... Arguments>
void Dosimeter::executeSPITransaction(F spiFunction, Arguments... arguments) {
    pullDownChipSelect();

    spiFunction(arguments...);
    waitForTransfer();

    pullUpChipSelect();
}

uint8_t Dosimeter::readRegister(Dosimeter::RegisterAddress readRegister) {
    uint8_t commandAddressByte = static_cast<uint8_t>(readRegister) | SPI_READ_COMMAND;
    uint8_t registerValue = 0;

    executeSPITransaction(DOSIMETER_SPI_WriteRead,&commandAddressByte, RegisterAddressSizeInBytes, &registerValue, RegisterSizeInBytes);

    // or try this if the sensor is slow
//    executeSPITransaction(DOSIMETER_SPI_Write, &commandAddressByte, RegisterAddressSizeInBytes);
//
//    executeSPITransaction(DOSIMETER_SPI_Read, &registerValue, RegisterSizeInBytes);

    return registerValue;
}

void Dosimeter::writeRegister(Dosimeter::RegisterAddress writeRegister, uint8_t data) {
    const uint8_t commandAddressByte = static_cast<uint8_t >(writeRegister) | SPI_WRITE_COMMAND;
    etl::array<uint8_t , RegisterSizeInBytes+RegisterAddressSizeInBytes> dataArray = {commandAddressByte, data};

    executeSPITransaction(DOSIMETER_SPI_Write, dataArray.data(), RegisterSizeInBytes+RegisterAddressSizeInBytes);
}

void Dosimeter::setTargetRegister(uint8_t value) {
    writeRegister(TARGET, value & TARGET_DEFAULT);
}

void Dosimeter::setThresholdRegister(uint8_t value) {
    writeRegister(THRESHOLD, value & THRESHOLD_DEFAULT);
}

void Dosimeter::waitForTransfer() {
    auto start = xTaskGetTickCount();

    while(DOSIMETER_SPI_IsBusy) {
        if (xTaskGetTickCount() - start > TimeoutTicks) {
            LOG_ERROR << "Dosimeter communication has timed out";
            break;
        }
    }
}
