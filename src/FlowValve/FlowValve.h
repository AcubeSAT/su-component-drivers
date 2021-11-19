#ifndef TEST_INTEGRATION_FLOWVALVE_H
#define TEST_INTEGRATION_FLOWVALVE_H

#include "plib_dacc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../SEGGER_RTT/SEGGER_RTT.h"

/*
 *  LHLA0542311H latching Flow Valve Driver
 *
 *  This class is implemented for ATSAMV71Q21B
 */

class FlowValve{
private:

    /*
     *  Parameters of the Flow Valve:
     *  - valve_id: The id of the valve
     *  - valve_state: The state of the valve. Closed->false, Open->true
     *  - dacc_channel: the dacc_channel the valve is connected to
     */

    uint8_t valve_id;
    bool valve_state;
    DACC_CHANNEL_NUM dacc_channel;

public:

    /*
     * The constructor of the valve
     *
     * @param valve_id the id of the valve
     * @param dacc_channel the dacc_channel the valve is connected to
     */
    FlowValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel);

    /*
     * Returns the state of the Valve. Whether it is open or not
     */
    bool is_open();

    /*
     * It opens the valve
     */
    void open_valve();

    /*
     * It closes the valve
     */
    void close_valve();

};

#endif //TEST_INTEGRATION_FLOWVALVE_H
