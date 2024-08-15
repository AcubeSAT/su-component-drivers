#include "Thermistor.hpp"
#include <cmath>

uint16_t Thermistor::getADCResult() {
    bool status;
    constexpr uint8_t maxRetries = 3;
    if (AdcChannelMask == AFEC_CH0_MASK) {
        AFEC0_Initialize();
        AFEC0_ChannelsEnable((AFEC_CHANNEL_MASK) AFEC_CH0);
        AFEC0_ConversionStart();
        status = AFEC0_ChannelResultIsReady(AdcChannelNumber);
        for(int retrycount=0; retrycount<maxRetries; retrycount++) {
            if (status) {
                AdcResult = AFEC0_ChannelResultGet(AdcChannelNumber);
                return AdcResult;
            }
            status = AFEC0_ChannelResultIsReady(AdcChannelNumber);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        LOG_ERROR << "Failed to get AFEC0 channel result after " << maxRetries << " retries.";
        return ErrorHandler::ExecutionCompletionErrorType;
    } else if (AdcChannelMask == AFEC_CH1_MASK) {
        AFEC1_Initialize();
        AFEC1_ChannelsEnable((AFEC_CHANNEL_MASK) AFEC_CH0);
        AFEC1_ConversionStart();
        status = AFEC1_ChannelResultIsReady(AdcChannelNumber);
        for(int retrycount=0; retrycount<maxRetries; retrycount++) {
            if (status) {
                AdcResult = AFEC1_ChannelResultGet(AdcChannelNumber);
                return AdcResult;
            }
            status = AFEC1_ChannelResultIsReady(AdcChannelNumber);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        LOG_ERROR << "Failed to get AFEC1 channel result after " << maxRetries << " retries.";
        return ErrorHandler::ExecutionCompletionErrorType;
    }
}

void Thermistor::OutputVoltageCalculation() {
    getADCResult();
    OutputVoltage = (float)AdcResult / MaxADCValue * PowerSupply;
}

void Thermistor::Voltage2Resistance() {
    ResistorValue = R3 * PowerSupply * (R2 + R1) / ((R2 + R1) * OutputVoltage + R2 * PowerSupply) - R3;
}

void Thermistor::Resistance2Temperature() {
    Voltage2Resistance();
    if (ResistorValue < 166.71) {
        Temperature = 0.0001 * pow(ResistorValue, 2) +
                      -0.0743 * ResistorValue + 21.5320;
    } else if (ResistorValue < 402.32 && ResistorValue > 166.71) {
        Temperature = 0.0004 * pow(ResistorValue, 2) +
                      -0.2277 * ResistorValue + 42.2494;
    } else {
        Temperature = 0.0039 * pow(ResistorValue, 2) +
                      -0.7207 * ResistorValue + 66.7732;
    }
}

double Thermistor::getTemperature() {
    Resistance2Temperature();
    return Temperature;
}