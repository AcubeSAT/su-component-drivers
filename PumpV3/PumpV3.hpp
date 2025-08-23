#ifndef PUMPV3_H
#define PUMPV3_H
#include "PCA9685.hpp"

#endif //PUMPV3_H

/**
 * Step mode configurations referenced from the DRV8825 datasheet
 * @note Due to hardware constraints, the modes greater than 3 are not possible to be set
 */
enum  class PumpStepMode : uint8_t {
    FullStep = 0, //"Full step (2-phase excitation) with 71% current"
    HalfStep = 1, //1/2 step (1-2 phase excitation)
    QuarterStep = 2, //1/4 step (W1-2 phase excitation)
    EightMicrosteps = 3, //8 microsteps/step

};


/**
 * @class PumpV3
 * @brief A driver for the Pump circuit on the v3 pcb. Functions as a wrapper for the underlying PCA9685 driver
 */
class PumpV3 {
    public:
        /**
        *
        * @param mode Step mode for the pump driver
        * @param sleeping Whether the driving circuit should be in sleep mode
        * @param frequency Frequency of step PWM. This controls the speed
        * @param direction Controls in which direction the pump will spin
        */
        PumpV3(PumpStepMode mode, bool sleeping, float frequency, bool direction);
        /**
         * Disables the driving circuit, putting it to sleep mode
         */
        void enterSleep();
        /**
         * Enables the driver
         */
        void exitSleep();

        /**
         * Sets the pump spin direction
         * @param direction The new direction for the pump to spin at
         */
        void setDirection(bool direction);

        /**
         * Checks whether the pump driving circuit has a fault
         * @return true if the driving circuit signals a fault
         */
        static bool getPumpFault();
    private:
        /**
         * PCA PWM channel corresponding to driver step input
         */
        constexpr static auto Step =PCA9685::PWMChannel::CHANNEL6;
        /**
         * PCA PWM channel corresponding to mode select bit 0. Used a high or low signal, not a PWM
         */
        constexpr static auto Mode0 =PCA9685::PWMChannel::CHANNEL5;
         /**
         * PCA PWM channel corresponding to mode select bit 1. Used a high or low signal, not a PWM
         */
        constexpr static auto Mode1 =PCA9685::PWMChannel::CHANNEL1;
        /**
        * PCA PWM channel corresponding to direction select. Used a high or low signal, not a PWM
         */
        constexpr static auto Dir=PCA9685::PWMChannel::CHANNEL7;
         /**
        * PCA PWM channel corresponding to sleep mode. Used a high or low signal, not a PWM
         */
        constexpr static auto Sleep=PCA9685::PWMChannel::CHANNEL9;
        /**
        * PCA PWM channel corresponding to reset . Used a high or low signal, not a PWM. Active Low
        */
        constexpr static auto Reset=PCA9685::PWMChannel::CHANNEL8;
        /**
         * Hardcoded PCA address
         */
        constexpr static auto PcaAddress = PCA9685::I2CAddress::I2CAddress_000000;
        /**
         * Underlying PCA driver
         */
        PCA9685 pca9685;

};
