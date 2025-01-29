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
 * @tparam PeripheralID The ID of the PWM peripheral selected.
 */
template<PWM_PeripheralID PeripheralID>
class PWM {
public:
    /**
     * Constructor.
     *
     * @param channelNumber The specific channel of the peripheral to be managed.
     */
    explicit constexpr PWM(PWM_PeripheralChannel channelNumber)
              : channelNumber(convertChannelNumberEnum(channelNumber)),
                channelMask(convertChannelNumberToMask(channelNumber)) {}

    PWM_CHANNEL_NUM channelNumber;
    PWM_CHANNEL_MASK channelMask;

    using CTDY_RegisterType_t = uint32_t;
    using CPRD_RegisterType_t = uint32_t;
    CPRD_RegisterType_t CPRD_RegisterValue = 15000;
    static constexpr uint32_t PeripheralClockFrequency = 150000000;
    float ClockPrescaler = 16.0f;
c
    /**
     * Starts the PWM signal on the configured channel.
     */
    void start() const {
        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelsStart(channelMask);
        } else if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelsStart(channelMask);
        }
    }

    /**
     * Stops the PWM signal on the configured channel.z
     */
    void stop() const {
        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelsStop(channelMask);
        } else if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelsStop(channelMask);
        }
    }

    /**
     * Sets the frequency of the PWM signal on the configured channel.
     *
     * @param frequency Frequency value to set (in Hz).
     */
    void setFrequency(float frequency) {
        CPRD_RegisterValue = static_cast<CPRD_RegisterType_t>(static_cast<float>(PeripheralClockFrequency) / (ClockPrescaler * frequency));

        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelPeriodSet(channelNumber, CPRD_RegisterValue);
        } else if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelPeriodSet(channelNumber, CPRD_RegisterValue);
        }
    }

    /**
     * Sets the duty cycle of the PWM signal on the configured channel.
     *
     * @param dutyCycle Duty cycle value to set (0-100%).
     */
    void setDutyCycle(float dutyCycle) const {
        const CTDY_RegisterType_t CTDY_RegisterValue = dutyCycle * static_cast<float>(CPRD_RegisterValue) / 100.0f;

        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_ChannelDutySet(channelNumber, CTDY_RegisterValue);
        } else if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_1) {
            PWM1_ChannelDutySet(channelNumber, CTDY_RegisterValue);
        }
    }

private:
    /**
     * Converts the channel enum to its corresponding mask.
     *
     * @param channelNumber Channel enum to convert.
     * @return Corresponding channel mask.
     */
     constexpr PWM_CHANNEL_MASK convertChannelNumberToMask(PWM_PeripheralChannel channelNumber) {
        switch (channelNumber) {
            case PWM_PeripheralChannel::CHANNEL0:
                return PWM_CHANNEL_0_MASK;
            case PWM_PeripheralChannel::CHANNEL1:
                return PWM_CHANNEL_1_MASK;
            case PWM_PeripheralChannel::CHANNEL2:
                return PWM_CHANNEL_2_MASK;
            case PWM_PeripheralChannel::CHANNEL3:
                return PWM_CHANNEL_3_MASK;
            default:
                return {};
        }
    }

    /**
     * Converts the custom channel enum class to its respective HAL enum.
     *
     * @param channelNum Channel enum to convert.
     * @return HAL enum representation of the channel.
     */
    constexpr PWM_CHANNEL_NUM convertChannelNumberEnum(PWM_PeripheralChannel channelNum) {
         switch (channelNum) {
         case PWM_PeripheralChannel::CHANNEL0:
             return PWM_CHANNEL_0;
         case PWM_PeripheralChannel::CHANNEL1:
             return PWM_CHANNEL_1;
         case PWM_PeripheralChannel::CHANNEL2:
             return PWM_CHANNEL_2;
         case PWM_PeripheralChannel::CHANNEL3:
             return PWM_CHANNEL_3;
         default:
             return {};
         }
     }
};

extern template class PWM<PWM_PeripheralID::PERIPHERAL_0>;
extern template class PWM<PWM_PeripheralID::PERIPHERAL_1>;

} // namespace PayloadDrivers
