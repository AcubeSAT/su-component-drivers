#pragma once

#include "Logger.hpp"
#include "plib_afec_common.h"
#include "plib_afec0.h"
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
     * @param ResistorValue The value of the resistor (in kiloOhms) that is used to map the current output of the sensor.
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

    /**
 * Variable in which the Analog to Digital (ADC) conversion result is stored.
 */
    uint16_t AdcResult = 0;

    /**
 * Number of the AFEC peripheral channel being used.
 */
    const AFEC_CHANNEL_NUM AdcChannelNumber = AFEC_CH8;

    /**
     * Resistances of the circuit in kilo ohms.
     */
    const float R1 = 1.0f;
    const float R2 = 3.57f;
    const float R3 = 301.0f;


    /**
     * Mask of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_MASK AdcChannelMask = AFEC_CH8_MASK;
private:
    /**
     * Power Supply of the NRBE10524450B1F thermistor
     *
     * @note If we decide to change the Power Supply ,
     * we will need to change the value of this member variable as well
     */
    static constexpr uint8_t PowerSupply = 5;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    static constexpr uint16_t MaxADCValue = 4095;



    /**
     * Value of the voltage that we connect the sensor to.
     *
     * @note This member variable is currently not in use
     */
    float OutputVoltage;






    /**
     * Variable in which the Temperature the thermistor measures is stored.
     */
    double Temperature;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    float ResistorValue;

    /**
     * Sets the Analog to Digital conversion result.
     * @param adcResult The result of the ADC conversion.
     */
    uint16_t getADCResult();

    /**
     * @return VoltageValue calculated using AdcResult and MaxADCValue
     */
    void VoltageValueCalculation();

    /**	Takes the voltage read by the  MCU and converts it to the resistance that the thermistor has.
     *	@return double The current resistance of the thermistor.
     */
    void Voltage2Resistance();

    /**	Takes the resistance calculated and converts it to a readable temperature using a polynomial,
     *  created from the values provided in the datasheet.
     *	@return double The temperature in Celsius.
     */
    void Resistance2Temperature();
};