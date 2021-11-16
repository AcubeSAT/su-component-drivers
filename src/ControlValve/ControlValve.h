#ifndef TEST_INTEGRATION_CONTROLVALVE_H
#define TEST_INTEGRATION_CONTROLVALVE_H

#include "plib_dacc.h"
#include "FreeRTOS.h"
#include "task.h"

class ControlValve{
private:
    uint8_t valve_id;
    DACC_CHANNEL_NUM dacc_channel;

public:
    ControlValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel);
    void open_for_ms(uint32_t time);
};

#endif //TEST_INTEGRATION_CONTROLVALVE_H
