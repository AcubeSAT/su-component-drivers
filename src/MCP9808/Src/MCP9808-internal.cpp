#include "../inc/MCP9808.h"

void MCP9808::writeReg(uint8_t addr, uint16_t &data) {
    uint8_t txData[3];  // data to be transmitted
    txData[0] = addr;   // register address
    txData[1] = data >> 8; // high 8-bits
    txData[2] = data & 0x00FF; // low 8-bits

    TWIHS0_Write(MCP9808_I2C_BUS_ADDR, txData, 3);
}

void MCP9808::readReg(uint8_t addr, uint16_t &result) {
    uint8_t buffer[2]; // a place to hold the incoming data

    TWIHS0_Write(MCP9808_I2C_BUS_ADDR, &addr, 1);
    TWIHS0_Read(MCP9808_I2C_BUS_ADDR, buffer, 2);

    // combine the 2 octets received (MSB, LSB) into one 16-bit uint)
    result = ((static_cast<uint16_t>(buffer[0]) << 8) | static_cast<uint16_t>(buffer[1]));
}

void MCP9808::setReg(uint8_t addr, uint16_t mask, uint16_t setting) {
    uint16_t previous;
    readReg(addr, previous); // store the previous value of the register
    uint16_t newSetting = (mask & previous) | setting;
    writeReg(addr, newSetting); // write the new one after changing the desired setting
}


