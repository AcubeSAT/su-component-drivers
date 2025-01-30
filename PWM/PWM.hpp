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
    explicit PWM(PWM_PeripheralChannel channelNumber)
              : channelNumber(ConvertChannelNumberEnum(channelNumber)),
                channelMask(ConvertChannelNumberToMask(channelNumber)),
                channelPrescaler(getChannelPrescaler()),
                CPRD_RegisterValue(initializeCPRDValue()) {
        setDisabledChannelPolarity();
    }

    using CTDY_RegisterType_t = uint32_t;
    using CPRD_RegisterType_t = uint32_t;
    using ChannelPrescalerType_t = uint32_t;

    /**
     * @enum ChannelPrescaler
     *
     * The possible prescaler values that control the frequency division of the clock signal.
     * Register CMRx, CPRE[3:0] bits.
     */
    enum class ChannelPrescaler : ChannelPrescalerType_t {
        MCK = 0x0,
        MCK_DIV_2 = 0x1,
        MCK_DIV_4 = 0x2,
        MCK_DIV_8 = 0x3,
        MCK_DIV_16 = 0x4,
        MCK_DIV_32 = 0x5,
        MCK_DIV_64 = 0x6,
        MCK_DIV_128 = 0x7,
        MCK_DIV_256 = 0x8,
        MCK_DIV_512 = 0x9,
        MCK_DIV_1024 = 0xA,
    };

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
     * Stops the PWM signal on the configured channel.
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
        CPRD_RegisterValue = static_cast<CPRD_RegisterType_t>(static_cast<float>(MCK_ClockFrequency) / (channelPrescaler * frequency));

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
     * The selected channel of the PWM peripheral (0, 1, 2, 3, 4).
     */
    PWM_CHANNEL_NUM channelNumber;

    /**
     * The corresponding mask of the PWM peripheral.
     */
    PWM_CHANNEL_MASK channelMask;

    /**
     * The MCK clock frequency.
     */
    static constexpr uint32_t MCK_ClockFrequency = 150e6;

    /**
     * The channel prescaler value.
     */
    float channelPrescaler;
    CPRD_RegisterType_t CPRD_RegisterValue;

    /**
     * Converts the channel enum to its corresponding mask.
     *
     * @param channelNumber Channel enum to convert.
     * @return Corresponding channel mask.
     */
    constexpr PWM_CHANNEL_MASK ConvertChannelNumberToMask(PWM_PeripheralChannel channelNumber) {
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
    constexpr PWM_CHANNEL_NUM ConvertChannelNumberEnum(PWM_PeripheralChannel channelNum) {
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

    /**
     * Reads the channel prescaler value from the hardware register.
     *
     * @return The prescaler value as a float.
     */
    float getChannelPrescaler() {
        const auto CPRE_Value = [=]() -> uint32_t {
            if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
                return PWM_CMR_CPRE(PWM0_REGS->PWM_CH_NUM[channelNumber].PWM_CMR);
            } else {
                return PWM_CMR_CPRE(PWM1_REGS->PWM_CH_NUM[channelNumber].PWM_CMR);
            }
        }();

        switch (static_cast<ChannelPrescaler>(CPRE_Value)) {
        case ChannelPrescaler::MCK_DIV_2:
            return 2;
        case ChannelPrescaler::MCK_DIV_4:
            return 4;
        case ChannelPrescaler::MCK_DIV_8:
            return 8;
        case ChannelPrescaler::MCK_DIV_16:
            return 16;
        case ChannelPrescaler::MCK_DIV_32:
            return 32;
        case ChannelPrescaler::MCK_DIV_64:
            return 64;
        case ChannelPrescaler::MCK_DIV_128:
            return 128;
        case ChannelPrescaler::MCK_DIV_256:
            return 256;
        case ChannelPrescaler::MCK_DIV_512:
            return 512;
        case ChannelPrescaler::MCK_DIV_1024:
            return 1024;
        default:
            return 1;
        }
    }

    /**
     * Initializes the CPRD register value with a default value.
     *
     * @return Default CPRD value.
     */
    [[nodiscard]] CPRD_RegisterType_t initializeCPRDValue() const {
        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            return PWM0_REGS->PWM_CH_NUM[channelNumber].PWM_CPRD;
        } else {
            return PWM1_REGS->PWM_CH_NUM[channelNumber].PWM_CPRD;
        }
    }

    /**
     * The value of the DPOLI bit of the PWM_CMR register.
     */
    static constexpr uint32_t CMR_DPOLI_Value = 1;

    /**
     * Set the disabled channel polarity DPOLI bit of PWM_CMR register.
     */
    void setDisabledChannelPolarity() {
        if constexpr (PeripheralID == PWM_PeripheralID::PERIPHERAL_0) {
            PWM0_REGS->PWM_CH_NUM[channelNumber].PWM_CMR |= PWM_CMR_DPOLI(CMR_DPOLI_Value);;
        } else {
            PWM1_REGS->PWM_CH_NUM[channelNumber].PWM_CMR |= PWM_CMR_DPOLI(CMR_DPOLI_Value);
        }
    }
};

extern template class PWM<PWM_PeripheralID::PERIPHERAL_0>;
extern template class PWM<PWM_PeripheralID::PERIPHERAL_1>;

} // namespace PayloadDrivers
