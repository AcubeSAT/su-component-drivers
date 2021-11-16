#ifndef TEST_INTEGRATION_FLOWVALVE_H
#define TEST_INTEGRATION_FLOWVALVE_H

#include "plib_dacc.h"
#include "FreeRTOS.h"
#include "task.h"

class FlowValve{
private:
    uint8_t valve_id;
    uint8_t valve_state;
    DACC_CHANNEL_NUM dacc_channel;

public:
    FlowValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel);
    bool is_open();
    void open_valve();
    void close_valve();

};

#endif //TEST_INTEGRATION_FLOWVALVE_H
