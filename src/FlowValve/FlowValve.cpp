#include "FlowValve.h"

FlowValve::FlowValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel) {
    this->valve_id = valve_id;
    this->dacc_channel = dacc_channel;
    valve_state = 0; //closed
}

bool FlowValve::is_open() {
    return valve_state == 1 ? true : false;
}

void FlowValve::open_valve() {
    if(!this->is_open()){
        DACC_DataWrite(dacc_channel, 4095);
        vTaskDelay(pdMS_TO_TICKS(30));
        DACC_DataWrite(dacc_channel, 0);
    }
}

void FlowValve::close_valve() {
    if(this->is_open()){
        DACC_DataWrite(dacc_channel, 4095);
        vTaskDelay(pdMS_TO_TICKS(30));
        DACC_DataWrite(dacc_channel, 0);
    }
}

