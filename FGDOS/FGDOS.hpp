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
    struct ConfigData
    {
        etl::array<uint8_t, 7> data{
            writeMask|0x9//write base address
           ,0b0000'1011//0x9:default target value for default clock and window and high sensitivity: 90kHz
           ,0b0000'0110 //0xA:default threshold value for -||- : 50kHZ
           ,0b1100'1001//0xB:default window, set tDiv to 1, it is mistakenly set to 0 in the datasheet
           ,0b0111'1001//0xC:high sensitivity
           ,0 //0xD:start with charging disabled, charge voltage is 000->14.5V
           ,0b0000'0100//0xE dont measure during spi: true, interrupt:0, engate:0
       };

        //returns byte that must be written to 0xD to enable recharging without changing anything else
        inline uint8_t getRechargeEnableByte(){
            return data[5]|0b0100'0000;
        }
        //returns byte that must be written to 0xA to change threshold to required value without changing anything else
        inline uint8_t getThresholdByte(const uint8_t threshold4Bits){
            return (data[2]&0b1111'0000)|threshold4Bits;
        }
        //sets the 5 bits that specify target frequency
        inline void setTargetFrequency(const uint8_t freq5bits){
            assert(freq5bits<=0b11111);
            data[1]=(data[1]&0b1110'0000)|freq5bits;
        }
        //sets the 5 bits that specify threshold frequency
        inline void setThresholdFrequency(const uint8_t freq5bits){
            assert(freq5bits<=0b1111);
            data[2]=(data[2]&0b1110'0000)|freq5bits;
        }

        //sets sensitivity. The user must set target and threshold frequency separately
        inline void setSensitivity(const bool high){
            data[4]=(data[4]&0b1111'1000)|(high?0b001:0b100);

        }

        //sets charge voltage. It takes a 3 bit value which is translated to a value between 14.5 and 18 Volts
        inline void setVoltage(const uint8_t voltage3bits){
            assert(voltage3bits<=0b111);
            data[5]=(data[5]&0b1111'1000)|(voltage3bits);

        }
    };





public:
    /**
     * Constructor that initializes the FGDOS instance and sets the chip select pin to high by default.
     *
     *@param clockFrequency Frequency of clock provided at CK pin of Dosimeter
     *
     *@param chipSelectPin The pin used for chip select (CS) in SPI communication.

     *
     *@param highSensitivity True for High Sensitivity mode (default), low for Low Sensitivity Mode
     *
     *
     *@param chargeVoltage value from 0 to 7 that indicated charge voltage, from 14.5 to 18
     *
     */
    FGDOS(uint32_t clockFrequency,PIO_PIN chipSelectPin, bool highSensitivity,uint8_t chargeVoltage);

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
    //returns temperature in celsius
    //TODO do i need to apply post processing to temperature?
    [[nodiscard]] int8_t getTemperature()
    {
        calculateDose();
        return etl::bit_cast<int8_t>(temperature);

    }

    //returns total dose in grays received since the driver was initialized
    [[nodiscard]] float getTotalDose();
    //returns the approximate rate of radiation in grays per second
    [[nodiscard]] float getDoseRate();

private:

    //constexpr static bool DoDebugPrinting = true;
    /**
     * The chip select pin for SPI communication.
     */
    PIO_PIN ChipSelectPin = PIO_PIN_NONE;


    uint32_t lastReadTick =0;


    uint8_t temperature = 0;
    uint8_t rechargeCount = 0;
    uint32_t refFrequency=0;
    uint32_t sensorFrequency=0;

    //previous frequency, used to calculate dose
    uint32_t sensorPrevFrequency=0;
    //previous reference frequency, used if recharge(s) have occurred
    uint32_t refPrevFrequency=0;

    bool sensorOverflown=false;
    bool referenceOverflown=false;
    bool recharging=false;

    // combined dose of all previous timesteps (except the last one)
    float previousDose=0.0f;
    // dose of only the last timestep. This is separate so that the current rate of change can be calculated
    float doseIncrease=0.0f;

    uint32_t configClockFrequency=0;

    //default
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


    //whether target and threshold values have smaller ranges (*1024 instead of *8912)
    bool tDiv=false;

    //spi functions
    //write data should be const but can't due to SPI api
    [[nodiscard]] inline bool write(uint8_t* data,size_t size) const
    {
        PIO_PinWrite(ChipSelectPin, false);

        const bool success=FGDOS_Write(data,size);
        PIO_PinWrite(ChipSelectPin, true);
        return success;

    }
   [[nodiscard]] inline bool writeRead(uint8_t* writeData,size_t writeSize,uint8_t* readData,size_t readSize)const
    {


        PIO_PinWrite(ChipSelectPin, false);

        const bool success=FGDOS_WriteRead(writeData,writeSize,readData,readSize);
        PIO_PinWrite(ChipSelectPin, true);
        return success;

    }

    //check if enough time has elapsed since last read to read again or use old values
    bool enoughTimeElapsed(){return true;}
    //return seconds that passed from last read tick until right now
    float getSecondsPassed(){return 0.0f;}


    //calculate approximate dose during recharges, using previous ref frequency and assuming temperature changed linearly from the first to the last recharge
    float approximateDoseFromRecharges(){return 0.0f;};
    //update data if necessary and calculate the dose in grays
    void calculateDose();
    //set last read tick to right now
    void updateLastReadTick(){};
    //read all the necessary data from dosimeter. Returns false if data is not ready
    [[nodiscard]] bool updateData();
    //convert raw counter to frequency in hertz
    [[nodiscard]] uint32_t frequencyFromRaw(uint32_t freq) const;
    [[nodiscard]] uint32_t temperatureCompensateFrequency(uint32_t freq) const;


    //stage configuration and write it to dosimeter registers
    void initConfiguration(uint8_t chargeVoltage,bool highSensitivity, bool forceRecharge);

    //convert frequency in hertz to the appropriate 5 bit value according to datasheet
    [[nodiscard]] uint8_t frequencyTo5Bit(const uint32_t frequency) const{
        //convert to 5 bit values: new=old*window/(tDiv?(clock_freq*8192):clock_freq*1024)

       const auto result=static_cast<uint8_t>((frequency*windowAmount)/(configClockFrequency*(tDiv?1024:8192)));
    //assert that result is 5 bit value
        assert(result<=0b0001'1111);
    return result;

}

//set recharge count indicator in dosimeter register to 0
void clearRechargeCount() const;


};
