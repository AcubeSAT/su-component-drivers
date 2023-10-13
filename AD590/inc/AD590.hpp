#pragma once

#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.hpp"

#if AD590_AFEC_Peripheral == 0

#include "peripheral/afec/plib_afec0.h"
#define AD590_AFEC_ChannelResultGet AFEC0_ChannelResultGet
#define AD590_AFEC_ConversionStart AFEC0_ConversionStart
#define AD590_AFEC_ChannelResultIsReady AFEC0_ChannelResultIsReady

#elif AD590_AFEC_Peripheral == 1

#include "peripheral/afec/plib_afec1.h"
#define AD590_AFEC_ChannelResultGet AFEC1_ChannelResultGet
#define AD590_AFEC_ConversionStart AFEC1_ConversionStart
#define AD590_AFEC_ChannelResultIsReady AFEC1_ChannelResultIsReady

#endif

/**
 * AD590 temperature sensor driver
 *
 * This is a simple driver to use the AD590 sensor on ATSAMV71Q21B microcontrollers.
 *
 * For more details about the operation of the sensor, see the datasheets found at:
 * https://www.mouser.com/catalog/specsheets/intersil_fn3171.pdf
 * and https://www.analog.com/media/en/technical-documentation/data-sheets/ad590.pdf
 *
 */

class AD590 {
private:

    /**
     * Nominal Current Output at 25°C (298.2 K)
     */
    inline static constexpr float offsetCurrent = 298.2;

    /**
     * Reference temperature constant in Celsius
     */
    inline static constexpr float referenceTemperature = 25;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    const int numOfBits;

    /**
     * Value of the voltage that we connect the sensor to.
     */
    const int voltageValue;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    const float resistorValue ;

public:

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM adcChannelNumber;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result from channel 0 is stored.
     */
     uint16_t adcResult;

    /**
     * Sets the Analog to Digital conversion result.
     * @param adcResult
     */
    void setADCResult(const uint16_t adcResult);


    AD590(int numOfBits,int voltageValue, float resistorValue, AFEC_CHANNEL_NUM adcChannelNumber): numOfBits(numOfBits),voltageValue(voltageValue),resistorValue(resistorValue), adcChannelNumber(adcChannelNumber) {}


    /**
     * Converts the voltage to current and finally to temperature in Celsius.
     * @param ADCconversion The ADC conversion result
     * @return The temperature in Celsius
     */
    float convertADCValueToTemperature(uint16_t ADCconversion);

    /**
     * Gets the analog temperature from the AD590 temperature sensor, converts it to digital and prints it.
     */
    float getTemperature();
};

