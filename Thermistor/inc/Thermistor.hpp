#pragma once

#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.hpp"
#include "peripheral/afec/plib_afec_common.h"
#include "src/config/default/peripheral/afec/plib_afec0.h"
#include "src/config/default/peripheral/afec/plib_afec1.h"
#include "etl/expected.h"

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
     * Contructor for the Thermistor class.
     * @param resistorValue The value of the resistor (in kiloOhms) that is used to map the current output of the sensor.
     * @param AdcChannelNumber Number of the AFEC channel that is being used.
     * @param AdcChannelMask Mask of the AFEC channel that is being used.
     * @note Harmony only lets us use AFEC_CH0_MASK and AFEC_CH1_MASK
     *
     * @note This function does not enable or configure the corresponding AFEC channel
     */
    Thermistor(float ResistorValue, AFEC_CHANNEL_NUM AdcChannelNumber, AFEC_CHANNEL_MASK AdcChannelMask) : ResistorValue(ResistorValue),
                                                                         AdcChannelNumber(AdcChannelNumber), AdcChannelMask(AdcChannelMask) {}

    /**
     * Getter function for the number of the channel used in the ADC conversion.
     * @return AFEC peripheral channel number
     */
    AFEC_CHANNEL_NUM getADCChannelNum() const;

    /**
    * Sets the Analog to Digital conversion result.
    * @param adcResult The result of the ADC conversion.
    */
    void setADCResult(const uint16_t ADCResult);

    /**
    * Gets the last measured analog temperature from the NRBE10524450B1F temperature sensor, by converting the voltage to current
    * and finally to temperature in Celsius.
    * @return The temperature in Celsius.
     */
    etl::expected<float, bool> getTemperature() const;

private:
    /**
     * Power Supply of the NRBE10524450B1F thermistor
     *
     * @note If we decide to change the Power Supply ,
     * we will need to change the value of this member variable as well
     */
    static constexpr float PowerSupply = 5.0f

    /**
     * Nominal Current Output at 25°C (298.2 K)
     */
    static float OffsetCurrent = PowerSupply * 10e5 / ResistorValue;

    /**
     * Reference temperature constant in Celsius
     */
    static constexpr float ReferenceTemperature = 25.0f;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    static constexpr uint16_t
    MaxADCValue = 4450;

    /**
     * Value of the voltage that we connect the sensor to.
     */
    static constexpr uint16_t
    VoltageValue = 3300;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    const float ResistorValue = 0.0f;

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM AdcChannelNumber;

    /**
     * Mask of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_MASK AdcChannelMask;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result from channel 0 is stored.
     */
    uint16_t AdcResult = 0;

    /**
     * Variable in which the Temperature the thermistor measures is stored.
     */
    double Temperature;

    /**
    * Sets the Analog to Digital conversion result.
    * @param adcResult The result of the ADC conversion.
    */
    uint16_t getADCResult();

    /**	Takes the voltage read by the  MCU and converts it to the resistance that the thermistor has.
     *	@return double The current resistance of the thermistor.
     */
    void Voltage2Resistance();

    /**	Takes the resistance calculated and converts it to a readable temperature using a polynomial,
     *  created from the values provided in the datasheet.
     *	@return double The temperature in celsius.
     */
    double Resistance2Temperature();
};