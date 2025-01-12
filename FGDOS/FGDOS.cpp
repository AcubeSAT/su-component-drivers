#include "FGDOS.hpp"

[[nodiscard]] uint8_t FGDOS::getChipID() const {
    return readRegister(DeviceRegister::CHIPID);
}

[[nodiscard]] uint8_t FGDOS::readRegister(DeviceRegister targetRegister) const {
    constexpr uint8_t ReadOperationMask = 0b1000'0000;
    const auto targetRegisterAddress = static_cast<DeviceRegisterAddressType_t>(targetRegister);
    etl::array<uint8_t, 1> txData {static_cast<uint8_t>(targetRegisterAddress | ReadOperationMask)};
    etl::array<uint8_t, 1> rxData {0};

    PIO_PinWrite(ChipSelectPin, false);
    if (not FGDOS_WriteRead(txData.data(), txData.size(), rxData.data(), rxData.size())) {
        LOG_ERROR << "FGDOS SPI transaction failed. Suspending task...";
        vTaskSuspend(nullptr);
    }
    PIO_PinWrite(ChipSelectPin, true);

    return rxData[0];
}

void FGDOS::writeRegister(DeviceRegister targetRegister, etl::span<uint8_t> dataBuffer) const {
    constexpr uint8_t WriteOperationMask = 0b0100'0000;
    constexpr size_t MaximumDataBufferSize = 5;
    const auto targetRegisterAddress = static_cast<DeviceRegisterAddressType_t>(targetRegister);

    etl::vector<uint8_t, MaximumDataBufferSize> txData {static_cast<uint8_t>(targetRegisterAddress | WriteOperationMask)};
    std::copy(dataBuffer.begin(), dataBuffer.end(), txData.begin() + 1);

    PIO_PinWrite(ChipSelectPin, false);
    if (not FGDOS_Write(dataBuffer.data(), dataBuffer.size())) {
        LOG_ERROR << "FGDOS SPI transaction failed. Suspending task...";
        vTaskSuspend(nullptr);
    }
    PIO_PinWrite(ChipSelectPin, true);
}
