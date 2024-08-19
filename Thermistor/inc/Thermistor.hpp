#pragma once

#include "Logger.hpp"
#include "peripheral/afec/plib_afec_common.h"
#include "peripheral/afec/plib_afec0.h"
#include "peripheral/afec/plib_afec1.h"
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
     * @param AdcChannelNumber Number of the AFEC channel that is being used.
     * @param AdcChannelMask Mask of the AFEC channel that is being used.
     * @note Harmony only lets us use AFEC_CH0_MASK and AFEC_CH1_MASK
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     *
     * @warning if we want to use any of the  channels we need to first enable them from Harmony Configuration
     */
    Thermistor(float R1, float R2, float R3, AFEC_CHANNEL_NUM AdcChannelNumber, AFEC_CHANNEL_MASK AdcChannelMask)
            : R1(R1),
              R2(R2),
              R3(R3),
              AdcChannelNumber(AdcChannelNumber), AdcChannelMask(AdcChannelMask) {}

    /**
     * Constructor for the Thermistor class that takes a default resistor value.
     * @param AdcChannelNumber Number of the AFEC channel that is being used.
     * @param AdcChannelMask Mask of the AFEC channel that is being used.
     * @note Harmony only lets us use AFEC_CH0_MASK and AFEC_CH1_MASK
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     *
     * @warning if we want to use any of the  channels we need to first enable them from Harmony Configuration
     */
    Thermistor(AFEC_CHANNEL_NUM AdcChannelNumber, AFEC_CHANNEL_MASK AdcChannelMask) : AdcChannelNumber(
            AdcChannelNumber), AdcChannelMask(AdcChannelMask) {}

    /**
    * Gets the last measured analog temperature from the NRBE10524450B1F temperature sensor, by converting the voltage to current
    * and finally to temperature in Celsius.
    * @return The temperature in Celsius.
     */
    double getTemperature();


private:
    /**
     * Power Supply of the NRBE10524450B1F thermistor
     *
     * @note If we decide to change the Power Supply ,
     * we will need to change the value of this member variable as well
     */
    static constexpr float PowerSupply = 5.0f;

    /**
     * Resistances of the circuit in kilo ohms
     */
    const float R1 = 1.0f;
    const float R2 = 3.57f;
    const float R3 = 301.0f;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    static constexpr uint16_t MaxADCValue = 4096;

    /**
     * Value of the voltage Vout of the thermistor.
     * Ranges from 0 to 3V3
     */
    float OutputVoltage = 0.0f;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    float ResistorValue = 0.0f;

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM AdcChannelNumber;

    /**
     * Mask of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_MASK AdcChannelMask;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result is stored.
     */
     uint16_t AdcResult = 0;



    /**
     * Variable in which the Temperature the thermistor measures is stored.
     */
    double Temperature = 0;


    /**
     * Gets the ADC result using a callback function.
     * @return The ADC result value.
     */

    uint16_t getADCResult(AFEC_CALLBACK callback, uintptr_t context);

    /**
     * @return VoltageValue calculated using AdcResult and MaxADCValue
     */
    void OutputVoltageCalculation();

    /**	Takes the voltage read by the  MCU and converts it to the resistance that the thermistor has.
     *	@return double The current resistance of the thermistor.
     */
    void Voltage2Resistance();

    /**	Takes the resistance calculated and converts it to a readable temperature using a polynomial,
     *  created from the values provided in the datasheet.
     *	@return double The temperature in Celsius.
     */
    void Resistance2Temperature();

/**
     * Callback function to handle ADC conversion complete event.
     * @param status The status of the ADC conversion.
     * @param context A pointer to the Thermistor object.
     */
    static void ADCResultCallback(uint32_t status, uintptr_t context);
};