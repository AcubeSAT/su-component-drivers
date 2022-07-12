//
// Created by fomarko on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_CONFIG_H
#define COMPONENT_DRIVERS_U3VCAM_CONFIG_H


#include "U3VCam_Registers_LUT.h"


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
* Macros Definitions
*********************************************************/

#define U3V_CAM_MODEL_SEL                           U3V_FLIR_CM3_U3_12S2C_CS_LUT_ENTRY

#define U3V_HOST_INSTANCES_NUMBER                   1U
#define U3V_HOST_ATTACH_LISTENERS_NUMBER            1U

#define U3V_IN_BUFFER_MAX_SIZE                      512U
#define U3V_REQ_TIMEOUT                             2000UL

#define U3V_TARGET_ARCH_BYTE_ALIGNMENT              8U                  /* Byte alignment / padding for MCU architecture */

#define U3V_DMA_CH_SEL                              SYS_DMA_CHANNEL_0   /* SYS_DMA_SOURCE_ADDRESSING_MODE type */

//TODO: remove or replace before integration - DEBUG XULT board specific definitions
/*** LED Macros for LED0 ***/
#define LED0_Toggle()                               (PIOA_REGS->PIO_ODSR ^= (1<<23))
#define LED0_On()                                   (PIOA_REGS->PIO_CODR = (1<<23))
#define LED0_Off()                                  (PIOA_REGS->PIO_SODR = (1<<23))

/*** LED Macros for LED1 ***/
#define LED1_Toggle()                               (PIOC_REGS->PIO_ODSR ^= (1<<9))
#define LED1_On()                                   (PIOC_REGS->PIO_CODR = (1<<9))
#define LED1_Off()                                  (PIOC_REGS->PIO_SODR = (1<<9))

/*** SWITCH Macros for SWITCH0 ***/
#define SWITCH0_Get()                               ((PIOA_REGS->PIO_PDSR >> 9) & 0x1)
#define SWITCH0_STATE_PRESSED                       0
#define SWITCH0_STATE_RELEASED                      1

/*** SWITCH Macros for SWITCH1 ***/
#define SWITCH1_Get()                               ((PIOB_REGS->PIO_PDSR >> 12) & 0x1)
#define SWITCH1_STATE_PRESSED                       0
#define SWITCH1_STATE_RELEASED                      1

/*** VBUS Macros for VBUS_HOST_EN ***/
#define VBUS_HOST_EN_PowerEnable()                  (PIOC_REGS->PIO_CODR = (1<<16))
#define VBUS_HOST_EN_PowerDisable()                 (PIOC_REGS->PIO_SODR = (1<<16))



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
