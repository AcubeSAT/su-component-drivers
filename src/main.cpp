#include "SEGGER_RTT/SEGGER_RTT.h"
#include "FlowValve/FlowValve.h"
#include "ControlValve/ControlValve.h"

#ifdef __cplusplus
extern "C" {
#endif

    void main_cpp(){
        FlowValve valve1(0,DACC_CHANNEL_0);
        ControlValve valve2(0, DACC_CHANNEL_0);
        valve1.open_valve();
        valve2.open_for_ms(300);
        SEGGER_RTT_printf(0, "Hello World\n");
    }

#ifdef __cplusplus
}
#endif
