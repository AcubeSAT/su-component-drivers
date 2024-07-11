#pragma once

#include <cstdint>
#include "FreeRTOS.h"
#include "Logger.hpp"
#include "task.h"
#include "Peripheral_Definitions.hpp"
#include "peripheral/afec/plib_afec_common.h"
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
     * Getter function for the number of the channel used in the ADC conversion.
     * @return AFEC peripheral channel number
     */
    AFEC_CHANNEL_NUM getADCChannelNum() const {
        return AdcChannelNumber;
    }

    /**
    * Sets the Analog to Digital conversion result.
    * @param adcResult The result of the ADC conversion.
    */
    void setADCResult(const uint16_t ADCResult) {
        adcResult = ADCResult;
    }

    /**
     * Contructor for the Thermistor class.
     * @param resistorValue The value of the resistor (in kiloOhms) that is used to map the current output of the sensor.
     * @param adcChannelNumber Number of the AFEC channel that is being used.
     * @note This function does not enable or configure the corresponding AFEC channel
     */
    Thermistor(float ResistorValue, AFEC_CHANNEL_NUM AdcChannelNumber) : ResistorValue(ResistorValue),
                                                                    AdcChannelNumber(AdcChannelNumber) {}

    /**
    * Gets the last measured analog temperature from the AD590 temperature sensor, by converting the voltage to current
    * and finally to temperature in Celsius.
    * @return The temperature in Celsius.
     */
    etl::expected<float, bool> getTemperature() const;

private:
    /**
     * Nominal Current Output at 25°C (298.2 K)
     */
    static constexpr float OffsetCurrent = 298.2f;

    /**
     * Reference temperature constant in Celsius
     */
    static constexpr float ReferenceTemperature = 25.0f;

    /**
     * Number of bits that the Analog to Digital (ADC) conversion result consists of.
     */
    static constexpr uint16_t MaxADCValue = 4095;

    /**
     * Value of the voltage that we connect the sensor to.
     */
    static constexpr uint16_t VoltageValue = 3300;

    /**
     * Value of the resistor, in kilo-ohms (kΩ), that maps the current output of the sensor onto the range 0-3.3V.
     */
    const float ResistorValue = 0.0f;

    /**
     * Number of the AFEC peripheral channel being used.
     */
    const AFEC_CHANNEL_NUM AdcChannelNumber;

    /**
     * Variable in which the Analog to Digital (ADC) conversion result from channel 0 is stored.
     */
    uint16_t adcResult = 0;
};