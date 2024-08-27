#include "etl/array.h"
#include "etl/bit.h"
#include "etl/bitset.h"
#include "NANDFlash.hpp"


uint8_t MT29F::resetNAND() {
    NAND_Reset();
}

bool MT29F::readNANDID(etl::array<uint8_t, 8> &id) {
    PLATFORM_SendCmd(CMD_READID);
    PLATFORM_SendAddr(ADDR_READ_ID);

    for (uint8_t i = 0; i < 8; i++) {
        id[i] = PLATFORM_ReadData();
    }
}

uint8_t MT29F::eraseBlock(uint8_t LUN, uint16_t block) {
    const uint8_t row1 = (block & 0x01) << 7;
    const uint8_t row2 = (block >> 1) & 0xff;
    const uint8_t row3 = ((block >> 9) & 0x07) | ((LUN & 0x01) << 3);

    nand_addr_t addr;
    addr.column = 0;
    addr.page = 0;
    addr.block = block;
    addr.lun = LUN;

    uint8_t result = NAND_Block_Erase(addr);

    return result;
}

bool MT29F::isNANDAlive() {
    etl::array<uint8_t, 8> id = {};
    const uint8_t valid_id[8] = {0x2C, 0x68, 0x00, 0x27, 0xA9, 0x00, 0x00, 0x00};
    readNANDID(id);
    (etl::equal(id.begin(), id.end(), valid_id));
}

uint8_t MT29F::PLATFORM_Wait(int start) {
    start = xTaskGetTickCount();
    while ((PIO_PinRead(nandReadyBusyPin) == 0)) {
        if ((xTaskGetTickCount() - start) > TimeoutCycles) {
            return NAND_TIMEOUT;
        }
    }
    return 0;
}

uint8_t MT29F::errorHandler() {
    if (resetNAND() != 224) {
        if (!isNANDAlive()) {
            // TODO: Check if LCL is on and execute its task again if need be
            return STATUS_FAIL;
        }
    }
    return 0;
}

//
//etl::array<uint8_t, MT29F::WriteChunkSize> MT29F::detectCorrectECCError(
//        etl::span<uint8_t, (WriteChunkSize + NumECCBytes)> dataECC, etl::array<uint8_t, NumECCBytes> newECC) {
//    etl::array<uint8_t, NumECCBytes> storedECC;
//    etl::span<uint8_t> eccSpan = dataECC.subspan(WriteChunkSize, NumECCBytes);
//    etl::copy(eccSpan.begin(), eccSpan.end(), storedECC.begin());
//
//    if (etl::equal(storedECC.begin(), storedECC.end(), newECC.begin()))
//        return MT29F::dataChunker<WriteChunkSize>(dataECC, 0);
//
//    uint8_t xorECC[NumECCBytes];
//    for (size_t i = 0; i < NumECCBytes; i++) {
//        xorECC[i] = storedECC[i] ^ newECC[i];
//    }
//
//    uint16_t xorECCBytes = xorECC[0] ^ xorECC[1] ^ xorECC[2];
//    uint16_t setBits = etl::popcount(xorECCBytes);
//
//    if (setBits == 12) {
//        uint8_t bitAddress = 0;
//        bitAddress |= ((xorECC[2] & 0x08) | (xorECC[2] & 0x020) | (xorECC[2] & 0x80));
//        // Byte address = (LP17,LP15,LP13,LP11,LP9,LP7,LP5,LP3,LP1)
//        uint16_t byteAddress = 0;
//        for (size_t eccByte = 0; eccByte < 2; eccByte++) {
//            for (size_t offset = 0x01; offset <= 0x80; offset += 0x02) {
//                byteAddress |= (xorECC[eccByte] & offset);
//            }
//        }
//        byteAddress |= (xorECC[2] & 0x01);
//
//        dataECC[byteAddress] ^= (0x01 << bitAddress);
//        return MT29F::dataChunker<WriteChunkSize>(dataECC, 0);
//    } else if (setBits == 1) return {}; // TODO: replace with ECC_ERROR code
//    else return {}; // TODO: replace with UNCORRECTABLE_ERROR
//}



bool MT29F::writeNAND(uint8_t LUN, uint32_t page, uint32_t column, etl::array<uint8_t, 20> &data){
    nand_addr_t addr;
    addr.lun = LUN;
    addr.page = page;
    addr.column = column;

    NAND_Page_Program(addr, data.data(), data.size());
}

bool MT29F::readNAND(uint8_t LUN, uint32_t page, uint32_t column, etl::array<uint8_t, 20> &data) {
    nand_addr_t addr;
    addr.lun = LUN;
    addr.page = page;
    addr.column = column;
    NAND_Page_Read(addr, data.data(), data.size());
}