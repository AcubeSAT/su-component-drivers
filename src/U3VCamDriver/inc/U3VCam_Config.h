//
// Created by fomarko on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_CONFIG_H
#define COMPONENT_DRIVERS_U3VCAM_CONFIG_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macros Definitions
*********************************************************/
/* U3V camera models defined here */
#define U3V_FLIR_CM3_U3_12S2C_CS                    (0)
#define U3V_XIMEA_XIQ_MQ013CG_E2                    (1)

/* U3V camera to be enabled in current build set here */
#define U3V_CAM_MODEL_SELECTED                      U3V_FLIR_CM3_U3_12S2C_CS

#define U3V_HOST_INSTANCES_NUMBER                   1U
#define U3V_HOST_ATTACH_LISTENERS_NUMBER            1U
#define U3V_IN_BUFFER_MAX_SIZE                      512U
#define U3V_REQ_TIMEOUT_MS                          1600UL
#define U3V_TARGET_ARCH_BYTE_ALIGNMENT              8U                  /* Byte alignment / padding for MCU architecture */

#define U3V_DMA_CH_SEL                              SYS_DMA_CHANNEL_0   /* SYS_DMA_SOURCE_ADDRESSING_MODE type */ //TODO: decide if this stays on integration

//TODO: remove or replace before integration all below - DEBUG XULT board specific definitions
#define LED0_Toggle()                               (PIOA_REGS->PIO_ODSR ^= (1<<23))
#define LED0_On()                                   (PIOA_REGS->PIO_CODR = (1<<23))
#define LED0_Off()                                  (PIOA_REGS->PIO_SODR = (1<<23))
#define LED1_Toggle()                               (PIOC_REGS->PIO_ODSR ^= (1<<9))
#define LED1_On()                                   (PIOC_REGS->PIO_CODR = (1<<9))
#define LED1_Off()                                  (PIOC_REGS->PIO_SODR = (1<<9))
#define SWITCH0_Get()                               ((PIOA_REGS->PIO_PDSR >> 9) & 0x1)
#define SWITCH0_STATE_PRESSED                       0
#define SWITCH0_STATE_RELEASED                      1
#define SWITCH1_Get()                               ((PIOB_REGS->PIO_PDSR >> 12) & 0x1)
#define SWITCH1_STATE_PRESSED                       0
#define SWITCH1_STATE_RELEASED                      1
#define VBUS_HOST_EN_PowerEnable()                  (PIOC_REGS->PIO_CODR = (1<<16))
#define VBUS_HOST_EN_PowerDisable()                 (PIOC_REGS->PIO_SODR = (1<<16))


/********************************************************
* Type definitions
*********************************************************/

typedef struct
{
    uint64_t camRegBaseAddress;
    uint64_t SBRMOffset;
    uint64_t temperature_Reg;                       /* reg value size = 4 bytes */
    uint64_t deviceReset_Reg;                       /* reg value size = 4 bytes */
    uint64_t singleFrameAcquisitionMode_Reg;        /* reg value size = 4 bytes */
    uint64_t acquisitionMode_Reg;                   /* reg value size = 4 bytes */
    uint64_t acquisitionStart_Reg;                  /* reg value size = 4 bytes */
    uint64_t acquisitionStop_Reg;                   /* reg value size = 4 bytes */
    uint64_t colorCodingID_Reg;                     /* reg value size = 4 bytes */
    uint64_t payloadSizeVal_Reg;                    /* reg value size = 4 bytes */
    uint32_t pixelFormatCtrlVal_Int_Sel;
} T_U3VCamRegisterCfg;


/********************************************************
* Variable declarations
*********************************************************/

extern const T_U3VCamRegisterCfg U3VCamRegisterCfgTable;



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
