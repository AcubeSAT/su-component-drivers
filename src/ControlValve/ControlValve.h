#ifndef TEST_INTEGRATION_CONTROLVALVE_H
#define TEST_INTEGRATION_CONTROLVALVE_H

#include "plib_dacc.h"
#include "FreeRTOS.h"
#include "task.h"

/*
 *  LHDA0523312H non-latching Control Valve Driver
 *
 *  This class is implemented for ATSAMV71Q21B
 */

class ControlValve{
private:

    /*
     *  Parameters of the Control Valve:
     *  - valve_id: The id of the valve
     *  - dacc_channel: the dacc_channel the valve is connected to
     */

    uint8_t valve_id;
    DACC_CHANNEL_NUM dacc_channel;

public:

    /*
     * The constructor of the valve
     *
     * @param valve_id the id of the valve
     * @param dacc_channel the dacc_channel the valve is connected to
     */
    ControlValve(uint8_t valve_id, DACC_CHANNEL_NUM dacc_channel);

    /*
     * It opens the valve for a specific time
     *
     * @param time The time we want the valve to be open
     */
    void open_for_ms(uint32_t time);
};

#endif //TEST_INTEGRATION_CONTROLVALVE_H
