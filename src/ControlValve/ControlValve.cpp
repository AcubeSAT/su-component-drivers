#include "ControlValve.h"

ControlValve::ControlValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel) {
    this->valve_id = valve_id;
    this->dacc_channel = dacc_channel;
}

void ControlValve::open_for_ms(uint32_t time) {
    DACC_DataWrite(dacc_channel, 4095);
    vTaskDelay(pdMS_TO_TICKS(time));
    DACC_DataWrite(dacc_channel, 0);
}


