#pragma once

#include <cstdint>
#include <type_traits>
#include <etl/array.h>
#include "PCA9685.hpp"
#include "Logger.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "peripheral/pio/plib_pio.h"
#include "Peripheral_Definitions.hpp"

class PumpDriver {
public:

    /**
     * Constructor for the PumpDriver class.
     */
    PumpDriver();

    /**
     * Function that initializes the pump.
     *
     * @brief Set up the PCA9685 device.
     */
    void init();

    /**
     * Function that sets the pump to operating mode.
     */
    void open();

    /**
     * Function that stops the operation of the pump.
     */
    void close();

    /**
     * Function that sets the speed of the pump's integrated stepper motor.
     *
     * @param speed The rotational speed of the pump's integrated stepper motor.
     */
    void setPumpSpeed(uint8_t speed);

private:

    /**
     * I2C chip address for the PCA9685 device.
     */
    const PCA9685::I2CAddress PCA9685ChipAddress = PCA9685::I2CAddress::I2CAddress_101011;

    /**
     * The number of the PWM signals that are required to operate the pump.
     */
    static constexpr uint8_t NumberOfPWMSignals = 4;

    /**
     * Array of the PCA9685 PWM outputs that will be used.
     *
     * @brief Positions {0: Phase A1, 1: Phase A2, 2: Phase B1, 3: Phase B2}
     */
    const etl::array<PCA9685::PWMChannel, NumberOfPWMSignals> PCA9685PWMChannels = {PCA9685::PWMChannel::CHANNEL0,
                                                                                    PCA9685::PWMChannel::CHANNEL1,
                                                                                    PCA9685::PWMChannel::CHANNEL2,
                                                                                    PCA9685::PWMChannel::CHANNEL3};

    /**
     * Driver for the PWM generator PCA9685.
     */
    PCA9685 pwmGenerator{PCA9685::I2CAddress::I2CAddress_101011};


};