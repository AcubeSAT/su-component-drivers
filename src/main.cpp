#include "SEGGER_RTT/SEGGER_RTT.h"

#ifdef __cplusplus
extern "C" {
#endif

    void main_cpp(){
        float result;
        SEGGER_RTT_printf(0, "%d\n", "Hello World");
    }

#ifdef __cplusplus
}
#endif
