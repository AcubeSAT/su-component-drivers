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

    executeSPITransaction(DOSIMETER_SPI_WriteRead, &commandAddressByte, RegisterAddressSizeInBytes, &registerValue, RegisterSizeInBytes);

    // or try this if the sensor is slow
//    executeSPITransaction(DOSIMETER_SPI_Write, &commandAddressByte, RegisterAddressSizeInBytes);
//
//    executeSPITransaction(DOSIMETER_SPI_Read, &registerValue, RegisterSizeInBytes);

    return registerValue;
}

void Dosimeter::writeRegister(Dosimeter::RegisterAddress writeRegister, uint8_t data) {
    const uint8_t commandAddressByte = static_cast<uint8_t >(writeRegister) | SPI_WRITE_COMMAND;
    etl::array<uint8_t, RegisterSizeInBytes + RegisterAddressSizeInBytes> dataArray =
            {commandAddressByte, prepareRegisterValue(writeRegister, data)};

    executeSPITransaction(DOSIMETER_SPI_Write, dataArray.data(), RegisterSizeInBytes + RegisterAddressSizeInBytes);
}

void Dosimeter::setTargetRegister(uint8_t value) {
    writeRegister(TARGET, value);
}

void Dosimeter::setThresholdRegister(uint8_t value) {
    writeRegister(THRESHOLD, value);
}

void Dosimeter::waitForTransfer() {
    auto start = xTaskGetTickCount();

    while (DOSIMETER_SPI_IsBusy) {
        if (xTaskGetTickCount() - start > TimeoutTicks) {
            LOG_ERROR << "Dosimeter communication has timed out";
            break;
        }
    }
}

uint8_t Dosimeter::prepareRegisterValue(Dosimeter::RegisterAddress registerAddress, uint8_t data) {
    switch (registerAddress) {
        case TARGET:
            return data & TARGET_DEFAULT;
        case THRESHOLD:
            return data & THRESHOLD_DEFAULT;
        case MISC1:
            return data & MISC1_DEFAULT;
        case MISC2:
            return data | MISC2_DEFAULT;
        case MISC4:
            return data & MISC4_DEFAULT;
        default:
            return data;
    }
}

void Dosimeter::readSerialNumber() {

}

uint8_t Dosimeter::readChipID() {
    return readRegister(CHIPID);
}

bool Dosimeter::sensorIsAlive() {
    const uint8_t readChipIdResult = readChipID();
    return (readChipIdResult == ChipID::VERSION_1) or (readChipIdResult == ChipID::VERSION_Z_Z1);
}
