#include "FlowValve.h"

FlowValve::FlowValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel) {
    this->valve_id = valve_id;
    this->dacc_channel = dacc_channel;
    valve_state = false; //closed
}

bool FlowValve::is_open() {
    return valve_state;
}

void FlowValve::open_valve() {
    if(!this->is_open()){
        DACC_DataWrite(dacc_channel, 4095);
        vTaskDelay(pdMS_TO_TICKS(30));
        DACC_DataWrite(dacc_channel, 0);
        valve_state = true;
    }
    else{
        SEGGER_RTT_printf(0, "The valve with id %d is already open", valve_id);
    }
}

void FlowValve::close_valve() {
    if(this->is_open()){
        DACC_DataWrite(dacc_channel, 4095);
        vTaskDelay(pdMS_TO_TICKS(30));
        DACC_DataWrite(dacc_channel, 0);
        valve_state = false;
    }
    else{
        SEGGER_RTT_printf(0, "The valve with id %d is already closed", valve_id);
    }
}

