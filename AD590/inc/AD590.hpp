#pragma once

#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.hpp"
#include "peripheral/afec/plib_afec_common.h"

/**
 * AD590 temperature sensor driver
 *
 * This is a simple driver to use the AD590 sensor on ATSAMV71Q21B microcontrollers.
 *
 * For more details about the operation of the sensor, see the datasheets found at:
 * https://www.mouser.com/catalog/specsheets/intersil_fn3171.pdf
 * and https://www.analog.com/media/en/technical-documentation/data-sheets/ad590.pdf
 */

class AD590 {
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
    * @param adcResult The result of the ADC conversion.
    */
    void setADCResult(const uint16_t adcResult);

    /**
     * Contructor for the AD590 class.
     * @param resistorValue The value of the resistor that is used to map the current output of the sensor.
     * @param adcChannelNumber Number of the AFEC channel that is being used.
     */
    AD590(float resistorValue, AFEC_CHANNEL_NUM adcChannelNumber): resistorValue(resistorValue), adcChannelNumber(adcChannelNumber) {}

    /**
     * Gets the analog temperature from the AD590 temperature sensor, converts the voltage to current and finally to temperature in celsius.
     * @return The temperature in Celsius.
     */
    float getTemperature();

private:
    /**
     * Nominal Current Output at 25°C (298.2 K)
     */
    inline static constexpr float offsetCurrent = 298.2;

    /**
     * Reference temperature constant in Celsius
     */
    inline static constexpr float referenceTemperature = 25.0f;

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
};