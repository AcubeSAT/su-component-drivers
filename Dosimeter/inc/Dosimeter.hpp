#pragma once

#include "definitions.h"

class Dosimeter {
public:
    explicit Dosimeter(PIO_PIN ChipSelect) : ChipSelect(ChipSelect) {
        pullUpChipSelect();
    };

private:
    PIO_PIN ChipSelect = PIO_PIN_NONE;

    enum Register: uint8_t {
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

    enum RegisterBitMask : uint8_t {
        RECHEV_EAWR_FCH_ENMC = 0b10000000,
        EVBCHP_NEBUF_ECH = 0b01000000,
        NCHP_ENTEMP_EPWR = 0b00100000,
        ENDCH_NELF_NEASNR = 0b00010000,
        WINDOW = 0b00001100,
        SET = 0b00000011,
        RCHCNT = 0b01111111,
        F1R_F1S_MSB_TDIV_E9S_ENGATE = 0b00000001,
        F1ROVF_F1SOVF_SNRF_NIRQOC = 0b00000010,
        DNEWR_DNEWS_NEIDCM_EDIRT = 0b00000100,
        TARGET_THRESHOLD = 0b00011111,
        ENOSC_E2V_LOWN = 0b00001000,
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
