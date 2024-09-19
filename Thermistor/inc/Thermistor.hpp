#pragma once

#include "Logger.hpp"
#include <cmath>
#include "peripheral/afec/plib_afec_common.h"
#include "Peripheral_Definitions.hpp"
#include "peripheral/afec/plib_afec0.h"
#include "peripheral/afec/plib_afec1.h"
#include "inc/FreeRTOSTasks/AFECHandlingTask.hpp"

/**
 * Thermistor NRBE10524450B1F driver
 *
 * This is a driver to use the thermistor NRBE10524450B1F on ATSAMV71Q21B microcontrollers.
 *
 * For more details about the operation of the sensor, see the datasheets found at:
 * https://gitlab.com/acubesat/su/hardware/breakout-boards/-/issues/27#note_1841589545
 */
template <AFECPeripheral AfecPeripheral>
class Thermistor {
public:
    /**
     * Constructor for the Thermistor class that takes a default resistor value.
     *
     * @tparam afecPeripheral The AFEC Peripheral that is being used (either AFEC0 or AFEC1).
     * @param adcChannelNum Number of the AFEC channel that is being used.
     * @param adcChannelMask Mask of the AFEC channel that is being used.
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     *
     * @warning if we want to use any of the  channels we need to first enable them from Harmony Configuration
     */
    explicit Thermistor(AFEC_CHANNEL_MASK afecChannelMask, AFEC_CHANNEL_NUM afecChannelNum);

    /**
     *	@return The temperature the Thermistor measures in Celsius.
     */
    double getTemperature();

private:
    /**
     * Power Supply of the NRBE10524450B1F thermistor
     *
     * @note If we decide to change the Power Supply ,
     * we will need to change the value of this member variable as well
     */
    static constexpr float PowerSupply = 4.97f;

    /**
     * Reference Reference Voltage used for calculations by the Afec
     *
     * @note The value of VrefAfec corresponds to the MaxADCValue
     *
     * @example 3V3 (=VrefAfec) corresponds to 4095(=MaxADCValue)
     */
    static constexpr float VrefAfec = 3.3f;

    /**
     * Resistances of the circuit in kilo ohms
     */
    const float R1 = 1.0f;
    const float R2 = 3.57f;
    const float R3 = 301.0f;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    static constexpr uint16_t
    MaxADCValue = 4095;

    /**
     * Mask of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_MASK afecChannelMask;

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM afecChannelNum;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result is stored.
     */
    uint16_t thermistorAdcResult = 0;

    /**
     * Gets the ADC result using a callback function.
     *
     * @return The ADC result value.
     */
    uint16_t getADCResult();

    /**
     * Calculates and returns the output voltage
     * output voltage: Value of the voltage Vout of the thermistor that ranges from 0 to 3V3
     *
     * @return outputVoltage calculated using adcResult and MaxADCValue
     */
    float getOutputVoltage();

    /**
     * Takes the voltage read by the  MCU and converts it to the resistance that the thermistor has.
     *
     * @return The current resistance of the thermistor.
     */
    double getResistance();
};