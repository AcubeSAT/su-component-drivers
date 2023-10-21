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
#define AD590_AFEC_CallbackRegister AFEC0_CallbackRegister

#elif AD590_AFEC_Peripheral == 1

#include "peripheral/afec/plib_afec1.h"
#define AD590_AFEC_ChannelResultGet AFEC1_ChannelResultGet
#define AD590_AFEC_ConversionStart AFEC1_ConversionStart
#define AD590_AFEC_ChannelResultIsReady AFEC1_ChannelResultIsReady
#define AD590_AFEC_CallbackRegister AFEC1_CallbackRegister

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
    inline static constexpr uint16_t numOfBits = 4096;

    /**
     * Value of the voltage that we connect the sensor to.
     */
    inline static constexpr uint16_t voltageValue = 3300;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    const float resistorValue = 0.0f;

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM adcChannelNumber;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result from channel 0 is stored.
     */
    uint16_t adcResult;

public:

    /**
     * Getter function for the number of the channel used in the ADC conversion.
     * @return AFEC peripheral channel number
     */
    inline AFEC_CHANNEL_NUM getADCChannelNum(){
        return adcChannelNumber;
    }

    /**
    * Sets the Analog to Digital conversion result.
    * @param adcResult
    */
    void setADCResult(const uint16_t adcResult);

    AD590(float resistorValue, AFEC_CHANNEL_NUM adcChannelNumber): resistorValue(resistorValue), adcChannelNumber(adcChannelNumber) {}

    /**
     * Gets the analog temperature from the AD590 temperature sensor, converts the voltage to current and finally to temperature in celsius..
     * @return The temperature in Celsius
     */
    float getTemperature();
};

