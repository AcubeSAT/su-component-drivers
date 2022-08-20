//
// Created by fomarko on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_CONFIG_H
#define COMPONENT_DRIVERS_U3VCAM_CONFIG_H


#include <stdint.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"  //TODO: remove on integration

#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macros Definitions
*********************************************************/
/* U3V camera models defined here */
#define U3V_FLIR_CM3_U3_12S2C_CS                    0
#define U3V_XIMEA_XIQ_MQ013CG_E2                    1
/* U3V camera model selected for current build */
#define U3V_CAM_MODEL_SELECTED                      U3V_FLIR_CM3_U3_12S2C_CS

#define U3V_HOST_INSTANCES_NUMBER                   1U
#define U3V_HOST_ATTACH_LISTENERS_NUMBER            1U
#define U3V_IN_BUFFER_MAX_SIZE                      1024U
#define U3V_LEADER_MAX_SIZE                         256U
#define U3V_TRAILER_MAX_SIZE                        256U
#define U3V_REQ_TIMEOUT_MS                          1600UL
#define U3V_TARGET_ARCH_BYTE_ALIGNMENT              8U        /* Byte alignment / padding of MCU architecture */

//TODO: remove on integration all defs below, used for debug (XULT board specific)
#define LED0_Toggle()                               (PIOA_REGS->PIO_ODSR ^= (1<<23))
#define LED0_On()                                   (PIOA_REGS->PIO_CODR = (1<<23))
#define LED0_Off()                                  (PIOA_REGS->PIO_SODR = (1<<23))
#define LED1_Toggle()                               (PIOC_REGS->PIO_ODSR ^= (1<<9))
#define LED1_On()                                   (PIOC_REGS->PIO_CODR = (1<<9))
#define LED1_Off()                                  (PIOC_REGS->PIO_SODR = (1<<9))
#define VBUS_HOST_EN_PowerEnable()                  (PIOC_REGS->PIO_CODR = (1<<16))
#define VBUS_HOST_EN_PowerDisable()                 (PIOC_REGS->PIO_SODR = (1<<16))


/********************************************************
* Type definitions
*********************************************************/

typedef struct
{
    uint64_t camRegBaseAddress;
    uint64_t SBRMOffset;
    uint64_t temperature_Reg;
    uint64_t deviceReset_Reg;
    uint64_t singleFrameAcquisitionMode_Reg;
    uint64_t acquisitionMode_Reg;
    uint64_t acquisitionStart_Reg;
    uint64_t acquisitionStop_Reg;
    uint64_t colorCodingID_Reg;
    uint64_t payloadSizeVal_Reg;
    uint32_t pixelFormatCtrlVal_Int_Sel;
} T_U3VCamRegisterCfg;      /* value size is 4 bytes for all (x_Reg) registers above */


/********************************************************
* Variable declarations
*********************************************************/

extern const T_U3VCamRegisterCfg U3VCamRegisterCfgTable;



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
