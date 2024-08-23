#pragma once

#include "Logger.hpp"
#include <cmath>
#include "peripheral/afec/plib_afec_common.h"
#include "Peripheral_Definitions.hpp"

#if THERMISTOR_PORT == 0

#include "peripheral/afec/plib_afec0.h"
#define AFEC_Initialize AFEC0_Initialize
#define AFEC_ChannelsEnable AFEC0_ChannelsEnable
#define AFEC_CallbackRegister AFEC0_CallbackRegister
#define AFEC_ConversionStart AFEC0_ConversionStart
#define AFEC_ChannelResultIsReady AFEC0_ChannelResultIsReady
#define AFEC_ChannelResultGet AFEC0_ChannelResultGet

#elif THERMISTOR_PORT == 1

#include "peripheral/afec/plib_afec0.h"
#define AFEC_Initialize AFEC1_Initialize
#define AFEC_ChannelsEnable AFEC1_ChannelsEnable
#define AFEC_CallbackRegister AFEC1_CallbackRegister
#define AFEC_ConversionStart AFEC1_ConversionStart
#define AFEC_ChannelResultIsReady AFEC1_ChannelResultIsReady
#define AFEC_ChannelResultGet AFEC1_ChannelResultGet


/**
 * Thermistor NRBE10524450B1F driver
 *
 * This is a simple driver to use the thermistor NRBE10524450B1F on ATSAMV71Q21B microcontrollers.
 *
 * For more details about the operation of the sensor, see the datasheets found at:
 * https://gitlab.com/acubesat/su/hardware/breakout-boards/-/issues/27#note_1841589545
 */
class Thermistor {
public:
    /**
     * Constructor for the Thermistor class.
     * @param R1, R2, R3: The resistances of the circuit in kilo ohms.
     * @param AdcChannelMask Mask of the AFEC channel that is being used.
     * @note Harmony only lets us use AFEC_CH0_MASK and AFEC_CH1_MASK
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     *
     * @warning if we want to use any of the  channels we need to first enable them from Harmony Configuration
     */
    Thermistor(float R1, float R2, float R3, AFEC_CHANNEL_MASK AdcChannelMask)
            : R1(R1),
              R2(R2),
              R3(R3),
              AdcChannelMask(AdcChannelMask) {}

    /**
     * Constructor for the Thermistor class that takes a default resistor value.
     * @param AdcChannelMask Mask of the AFEC channel that is being used.
     * @note Harmony only lets us use AFEC_CH0_MASK and AFEC_CH1_MASK
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     *
     * @warning if we want to use any of the  channels we need to first enable them from Harmony Configuration
     */
    Thermistor( AFEC_CHANNEL_MASK AdcChannelMask) :  AdcChannelMask(AdcChannelMask) {}

    /**
     *	@return The temperature in Celsius.
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
    const AFEC_CHANNEL_MASK AdcChannelMask;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result is stored.
     */
    uint16_t adcResult = 0;

    /**
     * Variable in which the temperature the thermistor measures is stored.
     */
    double temperature = 0;

    /**
     * Gets the ADC result using a callback function.
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
     *	@return The current resistance of the thermistor.
     */
    double getResistance();

    /**
     * Callback function to handle ADC conversion complete event.
     * @param status The status of the ADC conversion.
     * @param context A pointer to the Thermistor object.
     */
    static void ADCResultCallback(uint32_t status, uintptr_t context);
};