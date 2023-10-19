#pragma once

#include "definitions.h"
#include "Peripheral_Definitions.hpp"
#include "Task.hpp"
#include "Logger.hpp"

class Dosimeter {
public:
    explicit Dosimeter(PIO_PIN ChipSelect) : ChipSelect(ChipSelect) {
        pullUpChipSelect();
    };

    void setTargetRegister(uint8_t value);

    void setThresholdRegister(uint8_t value);

    void configureSensitivityMode();

    /**
     * According to the section QUICK SET-UP AND OPERATION EXAMPLE of the datasheet
     */
    void quickSetup();

private:
    /**
     * SPI Command Type (Write/Read)
     */
    enum SPICommandType : uint8_t {
        SPI_WRITE_COMMAND = 0b0100'0000,
        SPI_READ_COMMAND = 0b1000'0000,
    };

    const PIO_PIN ChipSelect = PIO_PIN_NONE;

    constexpr static inline uint8_t RegisterSizeInBytes = 1;
    constexpr static inline uint8_t RegisterAddressSizeInBytes = 1;
//    constexpr static inline uint8_t ChipID = 0x81;

    const uint32_t ChipSerialNumber = 0;

    /**
     * Wait period before for abandoning an SPI transfer because the send/receive buffer does not get unloaded/gets loaded.
     */
    constexpr static inline uint16_t TimeoutTicks = 5000;

    enum class SensorCounterStatus : uint8_t {
        NO_COUNTER_OVERFLOW = 0,
        COUNTER_OVERFLOW = 1
    };

    enum class ReferenceDataStatus : uint8_t {
        NO_NEW_DATA_AVAILABLE = 0,
        NEW_DATA_AVAILABLE = 1
    };

    enum class SensorDataStatus : uint8_t {
        NO_NEW_DATA_AVAILABLE = 0,
        NEW_DATA_AVAILABLE = 1
    };

    enum class ReferenceFrequencyConfiguration : uint8_t {
        SET_HIGH = 0,
        SET_LOW = 1
    };

    enum class ReferenceCounterStatus : uint8_t {
        NO_COUNTER_OVERFLOW = 0,
        COUNTER_OVERFLOW = 1
    };

    enum class MeasurementWindowInterval : uint8_t {
        THIRTY_TWO_K = 0b00,
        SIXTEEN_K = 0b01,
        EIGHT_K = 0b10,
        FOUR_K = 0b11
    };

    enum class MeasurementsDuringSPIStatus : uint8_t {
        ALLOWED = 0,
        NOT_ALLOWED = 1
    };

    enum class WindowMeasurementStatus : uint8_t {
        BY_COUNTS_AT_CK = 0,
        BY_GATING_AT_CK = 1
    };

    enum class ChargingMode : uint8_t {
        AUTO_CHARGING_DISABLED = 0,
        AUTO_CHARGING_ENABLED
    };

    enum class RechargingStatus : uint8_t {
        RECHARGING_ALLOWED = 0,
        RECHARGING_NOT_ALLOWED = 1
    };

    enum class RechargingEventStatus : uint8_t {
        NOT_IN_PROGRESS = 0,
        IN_PROGRESS = 1
    };

    enum class RechargingCounter : uint8_t {
        NO_RECHARGES = 0,
        AT_LEAST_127_RECHARGES = 1
    };

    enum class ChargePumpConnectedToVB : uint8_t {
        NO = 0,
        YES = 1
    };

    enum class ChargePumpOutputVoltage : uint8_t { // set(2:0) bits
        V14_5 = 0b000,
        V15_0 = 0b001,
        V15_5 = 0b010,
        V16_0 = 0b011,
        V16_5 = 0b100,
        V17_0 = 0b101,
        V17_5 = 0b110,
        V18_0 = 0b111
    };

    enum class RechargeSource : uint8_t {
        INTERNAL_PUMP = 0,
        EXTERNAL_VOLTAGE_AT_VB = 1
    };

    enum class DividerForTargetAndThreshold : uint8_t {
        THIRTEEN_LSB_BITS = 0,
        TEN_LSB_BITS = 1
    };

    enum class ForceChargeInManualMode : uint8_t {
        CHARGING_STOPPED = 0,
        CHARGING_STARTED = 1
    };

    enum class SensitivityConfiguration : uint8_t { // sens(2:0) bits
        HIGH_SENSITIVITY = 0b001,
        LOW_SENSITIVITY = 0b100
    };

    enum class InterruptOutputConfiguration : uint8_t {
        PUSH_PULL = 0,
        OPEN_COLLECTOR = 1
    };

    enum ChipID : uint8_t {
        VERSION_1 = 0x01,
        VERSION_Z_Z1 = 0x81
    };

    enum RegisterAddress : uint8_t {
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
        SN_LSB = 0x10, ///> Read-only
        SN_MIDB = 0x11, ///> Read-only
        SN_MSB = 0x12, ///> Read-only
        CHIPID = 0x13, ///> Read-only
        RESERVED = 0x14 // check if bits need to have a specific value
    };

    enum RegisterSpecifiedValue : uint8_t {
        TARGET_DEFAULT = 0b0001'1111, // value AND TARGET_DEFAULT
        THRESHOLD_DEFAULT = 0b0001'1111, // value AND THRESHOLD_DEFAULT
        MISC1_DEFAULT = 0b1111'1101, // value AND MISC1_DEFAULT
        MISC2_DEFAULT = 0b0111'1000, // value OR MISC2_DEFAULT
        MISC4_DEFAULT = 0b0000'0111 // value AND MISC4_DEFAULT
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

    template<typename F, typename... Arguments>
    void executeSPITransaction(F spiFunction, Arguments... arguments);

    uint8_t readRegister(RegisterAddress readRegister);

    void writeRegister(RegisterAddress writeRegister, uint8_t data);

    static void waitForTransfer();

    static uint8_t prepareRegisterValue(RegisterAddress registerAddress, uint8_t data);

    void readSerialNumber();

    uint8_t readChipID();

    bool sensorIsAlive();
};
