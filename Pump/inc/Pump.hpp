#pragma once

#include <cstdint>
#include <type_traits>
#include <etl/array.h>

#include "INA228.hpp"
#include "PCA9685.hpp"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

class Pump {
    /**
     * Direction of pump turning
     * @note According to the indication on the pump and the assembly procedures, the ForwardDirection should be used
     */
    enum class PumpDirection {
        ForwardDirection,
        ReverseDirection,

    };

public:
    /**
     * Constructor for the PumpDriver class.
     */
    Pump(bool sleeping, float frequency);

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

private:
    /**
     * I2C chip address for the PCA9685 device.
     */
    const PCA9685::I2CAddress PCA9685ChipAddress = PCA9685::I2CAddress::I2CAddress_101011;

    /**
     * Driver for the PWM generator PCA9685.
     */
    PCA9685 pwmGenerator{PCA9685::I2CAddress::I2CAddress_101011};

    /**
     * INA current sensor for the pump
     */
    INA228 ina228 {INA228::I2CAddress::Address_1000100}

    /**
     * The number of the PWM signals that are required to operate the pump.
     */
    static constexpr uint8_t StepperMotorPhasesNumber = 4;

    static constexpr uint8_t StepperMotorStepsNumber = 24;

    static constexpr float StepperMotorRotationPerFullStepAngle = 360.0f / StepperMotorStepsNumber;

    /**
     *
     * @brief Positions {0: Phase A1, 1: Phase A2, 2: Phase B1, 3: Phase B2}
     */
    struct PWMChannelConfig {
        PCA9685::PWMChannel pwmChannel;
        uint8_t delayPercent;
    };

    etl::array<PWMChannelConfig, StepperMotorPhasesNumber> pwmChannelsConfig{{{PCA9685::PWMChannel::CHANNEL0, 0},
                                                                              {PCA9685::PWMChannel::CHANNEL1, 50},
                                                                              {PCA9685::PWMChannel::CHANNEL2, 75},
                                                                              {PCA9685::PWMChannel::CHANNEL3, 25}}};

};
