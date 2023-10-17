#pragma once

#include "definitions.h"

class Dosimeter {
public:
    explicit Dosimeter(PIO_PIN ChipSelect) : ChipSelect(ChipSelect) {
        pullUpChipSelect();
    };

private:
    PIO_PIN ChipSelect = PIO_PIN_NONE;

    enum class Register: uint8_t {
        TEMP = 0x00, ///> Read-only
        RECHARGE_STATUS = 0x01, ///> Read-only
        // REGISTER = 0x02, Not supported
        F1R_LSB = 0x03, ///> Read-only
        F1R_MIDB = 0x04, ///> Read-only
        F1R_MSB_AND_STATUS = 0x05, ///> Read-only
        F1S_LSB = 0x06, ///> Read-only
        F1S_MIDB = 0x07, ///> Read-only
        F1S_MSB_AND_STATUS = 0x08, ///> Read-only
        TARGET = 0x09, ///> Read-Write, Upper level target frequency
        THRESHOLD = 0x0A, ///> Read-Write, Lower level threshold frequency
        MISC1 = 0x0B, ///> Read-Write
        MISC2 = 0x0C, ///> Read-Write
        MISC3 = 0x0D, ///> Read-Write
        MISC4 = 0x0E, ///> Read-Write
        // REGISTER = 0x0F, Not supported
        SN_LSB = 0x10, ///> Read-Write
        SN_MIDB = 0x11, ///> Read-Write
        SN_MSB = 0x12, ///> Read-Write
        CHIPID = 0x13, ///> Read-only
        RESERVED = 0x14 // check if bits need to have a specific value
    };

    enum RegisterSpecifiedValue : uint8_t {
        TARGET = 0b0001'1111, // value AND TARGET
        THRESHOLD = 0b0001'1111, // value AND THRESHOLD
        MISC2 = 0b0111'1000, // value OR MISC2
        MISC4 = 0b0000'0111 // value AND MISC4
    };

    enum RegisterBitMask : uint8_t {
        RECHEV = 0b1000'0000,
        EAWR = 0b1000'0000,
        FCH = 0b1000'0000,
        ENMC = 0b1000'0000,

        EVBCHP = 0b0100'0000,
        NEBUF = 0b0100'0000,
        ECH = 0b0100'0000,

        NCHP = 0b0010'0000,
        ENTEMP = 0b0010'0000,
        EPWR = 0b0010'0000,

        ENDCH = 0b0001'0000,
        NELF = 0b0001'0000,
        NEASNR = 0b0001'0000,

        WINDOW = 0b0000'1100,

        SET = 0b0000'0011,
        RCHCNT = 0b0111'1111,

        F1R_MSB = 0b0000'0001,
        F1S_MSB = 0b0000'0001,
        TDIV = 0b0000'0001,
        E9S = 0b0000'0001,
        ENGATE = 0b0000'0001,

        F1ROVF = 0b0000'0010,
        F1SOVF = 0b0000'0010,
        SNRF = 0b0000'0010,
        NIRQOC = 0b0000'0010,

        DNEWR = 0b0000'0100,
        DNEWS = 0b0000'0100,
        NEIDCM = 0b0000'0100,
        EDIRT = 0b0000'0100,

        ENOSC = 0b0000'1000,
        E2V = 0b0000'1000,
        LOWN = 0b0000'1000
    };

    enum RegisterBitShift : uint8_t {


    };

    inline void pullDownChipSelect() {
        PIO_PinWrite(ChipSelect, false);
    }

    inline void pullUpChipSelect() {
        PIO_PinWrite(ChipSelect, true);
    }

    void readRegister();

    void writeRegister();
};
