#pragma once

#include <etl/array.h>
#include "Peripheral_Definitions.hpp"
#include "Task.hpp"
#include "Logger.hpp"
#include "definitions.h"
#include <etl/bit.h>

#if FGDOS_SPI_PORT == 0

#define FGDOS_Initialize SPI0_Initialize
#define FGDOS_WriteRead SPI0_WriteRead
#define FGDOS_Write SPI0_Write
#define FGDOS_Read SPI0_Read

#else

#define FGDOS_Initialize SPI1_Initialize
#define FGDOS_WriteRead SPI1_WriteRead
#define FGDOS_Write SPI1_Write
#define FGDOS_Read SPI1_Read

#endif

//TODO test,fix temperature compensation,error handling,fix style, add documentation






/**
 * @class FGDOS
 * @brief Interface for communicating with the Floating Gate Dosimeter (FGDOS) radiation sensors.
 */
class FGDOS
{
    constexpr static uint8_t writeMask=0b0100'0000;
    constexpr static uint8_t readMask=0b1000'0000;
    //private struct that prepares config data to be written to registers
    /**
     * @struct ConfigData
     * Used to stage the configuration data to be written with SPI
     */
    struct ConfigData
    {
        etl::array<uint8_t, 7> data{
            writeMask|0x9//write base address
           ,0b0001'0110//0x9:default target value for default clock and window and high sensitivity, tdiv=1: 90kHz -> 22 as specified by manufacturer
           ,0b0000'1100 //0xA:default threshold value for -||- : 50kHZ -> 12 -||-
           ,0b1100'1001//0xB:default window, set tDiv to 1 for accuracy
           ,0b0111'1001//0xC:high sensitivity
           ,0 //0xD:start with charging disabled, charge voltage is 000->14.5V
           ,0b0000'0100//0xE dont measure during spi: true, interrupt:0, engate:0
       };

        /**
         * Used to enable recharge through a direct write to the appropriate register
         * @return the configuration byte that has the recharge enable bit set to true and the rest of the bits unaltered
         */
        [[nodiscard]] uint8_t getRechargeEnableByte() const{
            return data[5]|0b0100'0000;
        }
        //returns byte that must be written to 0xA to change threshold to required value without changing anything else
        /**
         * Used to edit threshold frequency through a direct write to the appropriate register
         * @param threshold5Bits 5bit value that corresponds to the frequency , acquired through frequencyTo5Bit()
         * @return The appropriate configuration byte, with the threshold frequency altered to match the specified value and the rest of the bits unaltered
         */
        [[nodiscard]] uint8_t getThresholdByte(const uint8_t threshold5Bits) const{
            return (data[2]&0b1110'0000)|threshold5Bits;
        }
        /**
            * Sets the Target frequency
            * @param freq5bits 5 bit value that corresponds to the frequency, acquired through frequencyTo5Bit()
        */
        inline void setTargetFrequency(const uint8_t freq5bits){
            configASSERT(freq5bits<=0b11111);
            data[1]=(data[1]&0b1110'0000)|freq5bits;
        }

        /**
         * Sets the Threshold frequency
         * @param freq5bits 5 bit value that corresponds to the frequency, acquired through frequencyTo5Bit()
         */
        inline void setThresholdFrequency(const uint8_t freq5bits){
            configASSERT(freq5bits<=0b1111);
            data[2]=(data[2]&0b1110'0000)|freq5bits;
        }

        /**
         * Sets the sensitivity to high or low. The user must set target and threshold frequencies separately
         * @param high true if sensitivity is high, false if low
         */
        inline void setSensitivity(const bool high){
            data[4]=(data[4]&0b1111'1000)|(high?0b001:0b100);

        }


        /**
         *Sets the recharge voltage
         * @param voltage3bits A 3 bit value which is translated to a value between 14.5 and 18 Volts
         */
        inline void setVoltage(const uint8_t voltage3bits){
            configASSERT(voltage3bits<=0b111);
            data[5]=(data[5]&0b1111'1000)|(voltage3bits);

        }
    };





public:
    /**
     * Constructor that initializes the FGDOS instance and sets the chip select pin to high by default.
     *
     *@param clockFrequency Frequency of clock provided at CK pin of Dosimeter. This is a hardware dependent value.
     *
     *@param chipSelectPin The pin used for chip select (CS) in SPI communication.

     *
     *@param highSensitivity True for High Sensitivity mode (default), low for Low Sensitivity Mode
     *
     *
     *@param chargeVoltage value from 0 to 7 that indicated charge voltage, from 14.5 to 18
     *
     *@param temperatureOffset value provided by calibration to correctly calculate the temperature
     */
    FGDOS(uint32_t clockFrequency,PIO_PIN chipSelectPin, bool highSensitivity,uint8_t chargeVoltage,int8_t temperatureOffset);

    /**
     * @brief Retrieves the Chip ID from the FGDOS device.
     *
     * @return The Chip ID as a byte.
     */
    [[nodiscard]] uint8_t getChipID() const;

    /**
     *@brief Retrieves the Serial Number from the FGDOS device.
     *
     * @return The 3-byte Serial Number as a 4 byte value
     */
    [[nodiscard]] uint32_t getSerialNumber() const;

    /**
     * Gets the temperature from the device (may perform a read or use a cached value)
     * @return The signed temperature value in degrees Celsius
     */
    //TODO: calibration
    [[nodiscard]] int getTemperature()
    {
        calculateDose();
        return temperature+temperatureOffset;

    }

    /**
     * Reads data from the device and updates the total dose received. May use a cached value.
     * @return The dose in Grays
     */
    [[nodiscard]] float getTotalDose();
    /**
     * Approximates the dose rate using the last two values read. May perform a new read.
     * @return The approximated dose rate in Grays per second
     */
    [[nodiscard]] float getDoseRate();

private:

    /**
     * The chip select pin for SPI communication.
     */
    PIO_PIN ChipSelectPin = PIO_PIN_NONE;

    /**
     * Constant temperature offset, provided by calibration
     */
    int8_t temperatureOffset=0;

    /**
     * Timestamp of last read
     */
    uint32_t lastReadTick =0;

    /**
     * cached temperature value
     */
    uint8_t temperature = 0;
    /**
     * cached recharge count value (usually 0)
     */
    uint8_t rechargeCount = 0;

    /**
     * cached Reference Frequency value
     */
    uint32_t refFrequency=0;
    /**
     * cached Sensor Frequency value
     */
    uint32_t sensorFrequency=0;

    /**
     * Sensor Frequency from the previous read, used for dose increase calculations
     */
    uint32_t sensorPrevFrequency=0;
    /**
        * Sensor Reference from the previous read, used for approximation if recharges occurred
     */
    uint32_t refPrevFrequency=0;

    /**
     * Flag corresponding to the device's sensor overflow flag
     */
    bool sensorOverflown=false;

    /**
     * Flag corresponding to the device's reference overflow flag
     */
    bool referenceOverflown=false;

    /**
     * Flag corresponding to the device's recharge in progress flag
     */
    bool recharging=false;

    /**
     * Accumulated dose of all previous reads except the last one
     */
    float previousDose=0.0f;
    /**
     * Dose increase, calculated fron the last read
     */
    float doseIncrease=0.0f;

    /**
     * Hardware clock frequency
     */
    uint32_t configClockFrequency=0;

    /**
     * Window amount value. Currently only the default value is supported.
     */
    uint32_t windowAmount=8192;


    constexpr static uint32_t thresholdFrequencyDefaultHigh=50'000;

    constexpr static uint32_t targetFrequencyDefaultHigh=90'000;

    constexpr static uint32_t thresholdFrequencyDefaultLow=140'000;

    constexpr static uint32_t targetFrequencyDefaultLow=180'000;

    //50k for high sensitivity, 140k for low. Target frequency is 40k above threshold
    uint32_t thresholdFrequency=0;
    uint32_t targetFrequency=0;

    //10k for low and 70k for high

    float configSensitivity=0.0f;
    constexpr static float sensitivityDefaultLow=10'000.0f;
    constexpr static float sensitivityDefaultHigh=70'000.0f;
    //placeholder value for linear coefficient that related reference frequency to temperature
    constexpr static float configCoefTempToRef=0.0f;



    /**
     * Whether target and threshold values have smaller ranges (*1024 instead of *8912). Currently forced to true for accuracy
     */
    bool tDiv=true;

    /**
     * SPI Write Wrapper
     * @param data pointer to data to be written
     * @param size size of data to be written
     * @return true if successful
     */
    [[nodiscard]] inline bool write(uint8_t* data,size_t size) const
    {
        PIO_PinWrite(ChipSelectPin, false);

        const bool success=FGDOS_Write(data,size);
        PIO_PinWrite(ChipSelectPin, true);
        return success;

    }

    /**
     * SPI WriteRead Wrapper
     * @param writeData pointer to data to be written
     * @param writeSize size of data to be written
     * @param readData pointer to data to be read
     * @param readSize size of data to be read
     * @return true if successful
     */
    [[nodiscard]] inline bool writeRead(uint8_t* writeData,size_t writeSize,uint8_t* readData,size_t readSize)const
    {


        PIO_PinWrite(ChipSelectPin, false);

        const bool success=FGDOS_WriteRead(writeData,writeSize,readData,readSize);
        PIO_PinWrite(ChipSelectPin, true);
        return success;

    }

    /**
     * NYI. Check if enough time is elapsed to read again
     * @return false if not enough time is elapsed to try reading
     */
    bool enoughTimeElapsed(){return true;}
    /**
     * NYI.Helper function to check time elapsed since last read
     * @return the seconds passed from last read
     */
    float getSecondsPassed(){return 0.0f;}


    /**
     * NYI. Do not call if recharge count is 0. Uses count of recharges and previous reference frequency to approximate the does increase from recharges.
     * @return dose increase from recharges
     */
    float approximateDoseFromRecharges(){return 0.0f;};
    /**
     * Calls @ref updateData() and updates dose values
     */
    void calculateDose();
    /**
     * NYI. Helper Function that updates @ref lastReadTick
     */
    void updateLastReadTick(){};
    /**
     * Attempts to read from the device if necessary and updates read values
     * @return false if data was not updated
     */
    [[nodiscard]] bool updateData();
    /**
     * Converts raw value from register to frequency in Hertz
     * @param freq raw value read from sensor or reference register
     * @return frequency in hertz
     */
    [[nodiscard]] uint32_t frequencyFromRaw(uint32_t freq) const;
    /**
     * NYI. Compensates frequency for temperature
     * @param freq Sensor Frequency in hertz
     * @return compensated frequency in hertz
     */
    [[nodiscard]] uint32_t temperatureCompensateFrequency(uint32_t freq) const;


    /**
     * Writes all relevant configuration values to device registers
     * @param chargeVoltage 3 bit value taht corresponds to charge voltage from 14.5 to 18 Volts
     * @param highSensitivity true for high sensitivity mode, false for low
     * @param forceRecharge if true, an initial recharge will occur. Recommended for first use after installation
     */
    void initConfiguration(uint8_t chargeVoltage,bool highSensitivity, bool forceRecharge);

    /**
     * Converts frequency to 5bit value to be used in target and threshold registers
     * @param frequency the frequency in Hertz
     * @return The converted 5bit value
     */
    [[nodiscard]] uint8_t frequencyTo5Bit(const uint32_t frequency) const{
        //convert to 5 bit values: new=old*window/(tDiv?(clock_freq*8192):clock_freq*1024)
        //use rounding for better accuracy
        const auto numerator=((frequency*windowAmount));
        const auto denominator=(configClockFrequency*(tDiv?1024:8192));
       const auto result=numerator/denominator+(2*(numerator%denominator)>=(denominator));

    //configASSERT that result is 5 bit value
        configASSERT(result<=0b0001'1111);
    return static_cast<uint8_t>(result)&0b0001'1111;

}

    /**
     * Clears the recharge count register. Required after each read
     */
    void clearRechargeCount() const;

    /**
     *Only for testing
     *Prints all config values
     */
    void debugPrintConfig() const;



};
