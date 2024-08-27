#pragma once

#include "SMC.hpp"
#include "definitions.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Logger.hpp"
#include "nand_MT29F_lld.hpp"
#include "etl/span.h"

/**
 * This is a driver for MT29F NAND Flash.
 * @ingroup drivers
 * @see http://ww1.microchip.com/downloads/en/DeviceDoc/NAND-Flash-Interface-with-EBI-on-Cortex-M-Based-MCUs-DS90003184A.pdf
 *  and https://gr.mouser.com/datasheet/2/671/micron_technology_micts05995-1-1759202.pdf
 */
class MT29F : public SMC {
private:
    const uint32_t triggerNANDALEAddress = moduleBaseAddress | 0x200000;
    const uint32_t triggerNANDCLEAddress = moduleBaseAddress | 0x400000;

    const PIO_PIN nandReadyBusyPin = PIO_PIN_NONE; //TODO: if PIO_PIN == NONE => throw log message
    const PIO_PIN nandWriteProtect = PIO_PIN_NONE;

    inline static constexpr uint8_t
    enableNandConfiguration = 1;

    const static inline uint8_t TimeoutCycles = 20;

    /**
     * @param chipSelect Number of the Chip Select used for enabling the Nand Flash Die.
     */
    inline constexpr void selectNandConfiguration(ChipSelect chipSelect) {
        switch (chipSelect) {
            case NCS0:
                MATRIX_REGS->CCFG_SMCNFCS |= CCFG_SMCNFCS_SMC_NFCS0(enableNandConfiguration);
                return;

            case NCS1:
                MATRIX_REGS->CCFG_SMCNFCS |= CCFG_SMCNFCS_SMC_NFCS1(enableNandConfiguration);
                return;

            case NCS2:
                MATRIX_REGS->CCFG_SMCNFCS |= CCFG_SMCNFCS_SMC_NFCS2(enableNandConfiguration);
                return;

            case NCS3:
                MATRIX_REGS->CCFG_SMCNFCS |= CCFG_SMCNFCS_SMC_NFCS3(enableNandConfiguration);
                return;

            default:
                return;
        }
    }

public:
    constexpr MT29F(ChipSelect chipSelect, PIO_PIN nandReadyBusyPin, PIO_PIN nandWriteProtect) : SMC(chipSelect),
                                                                                                 nandReadyBusyPin(
                                                                                                         nandReadyBusyPin),
                                                                                                 nandWriteProtect(
                                                                                                         nandWriteProtect) {
        selectNandConfiguration(chipSelect);
    }

    inline void PLATFORM_SendData(uint8_t data) {
        smcWriteByte(moduleBaseAddress, data);
    }

    inline void PLATFORM_SendAddr(uint8_t address) {
        smcWriteByte(triggerNANDALEAddress, address);
    }

    inline void PLATFORM_SendCmd(uint8_t command) {
        smcWriteByte(triggerNANDCLEAddress, command);
    }

    inline uint8_t PLATFORM_ReadData() {
        return smcReadByte(moduleBaseAddress);
    }

    inline void PLATFORM_Open(void){

    }

    inline void PLATFORM_Close(void){

    }

    uint8_t resetNAND(); // TODO: use etl::expected

    bool readNANDID(etl::array<uint8_t, 8> &id);

    uint8_t eraseBlock(uint8_t LUN, uint16_t block); // TODO: use etl::expected

    uint8_t detectArrayError(); // TODO: use etl::expected

    bool isNANDAlive();

    uint8_t PLATFORM_Wait(int nanoseconds); // TODO: use etl::expected

    uint8_t errorHandler(); // TODO: use etl::expected

    bool writeNAND(uint8_t LUN, uint32_t page, uint32_t column, etl::array<uint8_t, 20> &data);

    bool readNAND(uint8_t LUN, uint32_t page, uint32_t column, etl::array<uint8_t, 20> &data);

};