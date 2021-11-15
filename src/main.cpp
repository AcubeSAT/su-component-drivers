#include "SEGGER_RTT/SEGGER_RTT.h"
#include "MCP9808/Inc/MCP9808.h"

#ifdef __cplusplus
extern "C" {
#endif

    void main_cpp(){
        float result;
        MCP9808 TempSensor;
        TempSensor.getTemp(result);
        SEGGER_RTT_printf(0, "%d\n", result);
    }

#ifdef __cplusplus
}
#endif
