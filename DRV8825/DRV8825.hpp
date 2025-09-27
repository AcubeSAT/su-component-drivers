#pragma once
#include "INA228.hpp"
#include "MCP4725.hpp"
#include "PCA9685.hpp"

/**
 * Step mode configurations referenced from the DRV8825 datasheet
 * @note Due to hardware constraints, some step modes are omitted
 */
enum  class PumpStepMode : uint8_t {
    FullStep = 0,
    HalfStep = 1,
    QuarterStep = 2,
    EightMicrosteps = 3

};

/**
 * Direction of pump turning
 * @note According to the indication on the pump and the assembly procedures, the ForwardDirection should be used
 */
enum class PumpDirection {
    ForwardDirection,
    ReverseDirection,

};

/**
 * @class DRV8825
 * @brief A driver for DRV8825, the Pump circuit on the v3 SU PCB. This class functions as a wrapper for the underlying PCA9685 driver.
 */
class DRV8825 {
    public:
        /**
        *
        * @param mode Step mode for the pump driver
        * @param sleeping Whether the driving circuit should be in sleep mode
        * @param frequency Frequency of step PWM. This controls the speed
        */
        DRV8825(PumpStepMode mode, bool sleeping, float frequency);
        /**
         * Disables the driving circuit, putting it to sleep mode
         */
        void enterSleep();
        /**
         * Enables the driver
         */
        void exitSleep();



        /**
         * Checks whether the pump driving circuit has a fault
         * @return true if the driving circuit signals a fault
         */
        static bool getPumpFault();

        /**
         * Gets the pump current using the INA current sensor driver
         * @return The pump drawn by the current in Amps
         */
        float getPumpCurrent() const;

        /**
        * Sets the pump spin direction
        * @param direction The new direction for the pump to spin at
        * @note The ReverseDirection should NOT be used unless specifically activated by ground intervention
        */
        void setDirection(PumpDirection direction);

        /**
         * Sets the signal frequency that the DRV receives. This controls the speed
         * @param frequency The frequency in Hertz
         */
        void setFrequency(float frequency);

        /**
         * Sets the DRV Step mode
         * @param mode The selected Mode
         */
        void setStepMode(PumpStepMode mode);

        /**
         * DAC driver that controls the reference voltage for the DRV
         */
        MCP4725 dac;
    private:

        /**
         * PCA PWM channel corresponding to driver step input
         */
        constexpr static auto Step = PCA9685::PWMChannel::CHANNEL6;
        /**
         * PCA PWM channel corresponding to mode select bit 0. Used a high or low signal, not a PWM
         */
        constexpr static auto Mode0 = PCA9685::PWMChannel::CHANNEL5;
         /**
         * PCA PWM channel corresponding to mode select bit 1. Used a high or low signal, not a PWM
         */
        constexpr static auto Mode1 = PCA9685::PWMChannel::CHANNEL1;
        /**
        * PCA PWM channel corresponding to direction select. Used a high or low signal, not a PWM
         */
        constexpr static auto Dir = PCA9685::PWMChannel::CHANNEL7;
         /**
        * PCA PWM channel corresponding to sleep mode. Used a high or low signal, not a PWM
         */
        constexpr static auto Sleep = PCA9685::PWMChannel::CHANNEL9;
        /**
        * PCA PWM channel corresponding to reset . Used a high or low signal, not a PWM. Active Low
        */
        constexpr static auto Reset = PCA9685::PWMChannel::CHANNEL8;
        /**
         * Hardcoded PCA address
         */
        constexpr static auto PcaAddress = PCA9685::I2CAddress::I2CAddress_000000;
        /**
         * Underlying PCA driver
         */
        PCA9685 pca9685;

        /**
         * INA current sensor driver
         */
        INA228 ina;



        /**
         * Hardcoded I2C address for the DAC that controls the driver circuit reference voltage
         */
        constexpr  static uint8_t DacAddress = 0b1100110;

        /**
         * Hardcoded I2C address for the INA current sensor
         */
        constexpr static auto InaAddress = INA228::I2CAddress::Address_1000100;

        /**
         * Duty cycle for the driver PWM input signal.
         * @note 50% for a square wave
         */
        constexpr static uint8_t StepDutyCycle = 50;

};
