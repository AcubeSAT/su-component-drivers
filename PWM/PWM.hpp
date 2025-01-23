#pragma once

#include "definitions.h"

namespace PayloadDrivers {

/**
 * @enum PWM_PeripheralID
 * Identifies the PWM peripheral instance.
 */
enum class PWM_PeripheralID {
    PERIPHERAL_0,
    PERIPHERAL_1,
};

/**
 * @enum PWM_PeripheralChannel
 * Maps to specific PWM channels within a peripheral.
 */
enum class PWM_PeripheralChannel {
    CHANNEL0 = PWM_CHANNEL_0,
    CHANNEL1 = PWM_CHANNEL_1,
    CHANNEL2 = PWM_CHANNEL_2,
    CHANNEL3 = PWM_CHANNEL_3,
};

/**
 * @enum PWM_PeripheralChannelMask
 */
enum class PWM_PeripheralChannelMask {
    CHANNEL0 = PWM_CHANNEL_0_MASK,
    CHANNEL1 = PWM_CHANNEL_1_MASK,
    CHANNEL2 = PWM_CHANNEL_2_MASK,
    CHANNEL3 = PWM_CHANNEL_3_MASK,
};

/**
 * @class PWM
 * Template class to manage PWM peripherals and their respective channels.
 *
 * @tparam Peripheral The ID of the PWM peripheral selected.
 */
template<typename Peripheral>
class PWM {
public:
    /**
     * Constructor.
     *
     * @param peripheralID The PWM peripheral instance.
     * @param channelNumber The specific channel of the peripheral to be managed.
     */
    constexpr PWM(PWM_PeripheralID peripheralID, PWM_PeripheralChannel channelNumber)
        : peripheralID(peripheralID), channelNumber(convertChannelNumberEnum(channelNumber)) {}

    /**
     * Starts the PWM signal on the configured channel.
     */
    void start() const {
        if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelsStart(channelMask);
        } else if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelsStart(channelMask);
        }
    }

    /**
     * Stops the PWM signal on the configured channel.
     */
    void stop() const {
        if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelsStop(channelMask);
        } else if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelsStop(channelMask);
        }
    }

    /**
     * Sets the frequency of the PWM signal on the configured channel.
     *
     * @param frequency Frequency value to set (in Hz).
     */
    void setFrequency(uint32_t frequency) const {
        if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelPeriodSet(PWM_CHANNEL_0, frequency);
        } else if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelPeriodSet(PWM_CHANNEL_0, frequency);
        }
    }

    /**
     * Sets the duty cycle of the PWM signal on the configured channel.
     *
     * @param dutyCycle Duty cycle value to set (0-100%).
     */
    void setDutyCycle(uint32_t dutyCycle) const {
        if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelDutySet(PWM_CHANNEL_0, dutyCycle);
        } else if constexpr (peripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelDutySet(PWM_CHANNEL_0, dutyCycle);
        }
    }

private:
    PWM_PeripheralID peripheralID;
    PWM_CHANNEL_NUM channelNumber;

    /**
     * Converts the channel enum to its corresponding mask.
     *
     * @param channelNumber Channel enum to convert.
     * @return Corresponding channel mask.
     */
    PWM_CHANNEL_MASK channelMask = [](PWM_PeripheralChannel channelNumber) {
        switch (channelNumber) {
            case PWM_PeripheralChannel::CHANNEL0:
                return PWM_PeripheralChannelMask::CHANNEL0;
            case PWM_PeripheralChannel::CHANNEL1:
                return PWM_PeripheralChannelMask::CHANNEL1;
            case PWM_PeripheralChannel::CHANNEL2:
                return PWM_PeripheralChannelMask::CHANNEL2;
            case PWM_PeripheralChannel::CHANNEL3:
                return PWM_PeripheralChannelMask::CHANNEL3;
            default:
                break;
        }
    }(channelNumber);

    /**
     * Converts the custom channel enum class to its respective HAL enum.
     *
     * @param channelNum Channel enum to convert.
     * @return HAL enum representation of the channel.
     */
    constexpr PWM_CHANNEL_NUM convertChannelNumberEnum(PWM_PeripheralChannel channelNum) {
        switch (channelNumber) {
            case PWM_CHANNEL_0:
                return PWM_CHANNEL_0;
            case PWM_CHANNEL_1:
                return PWM_CHANNEL_1;
            case PWM_CHANNEL_2:
                return PWM_CHANNEL_2;
            case PWM_CHANNEL_3:
                return PWM_CHANNEL_3;
            default:
                break;
        }
    };

};

} // namespace PayloadDrivers
