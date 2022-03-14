//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_CONFIG_H
#define COMPONENT_DRIVERS_USBCAMDRV_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
* Macros Definitions
*********************************************************/

#define U3V_HOST_INSTANCES_NUMBER                   1u
#define U3V_HOST_ATTACH_LISTENERS_NUMBER            1u

#define U3V_ENDIANNESS                              LITTLE_ENDIAN
#define U3V_PIXEL_FORMAT                            _RGB8
#define U3V_PIXEL_BITSIZE                           24u

#define U3V_IN_BUFFER_MAX_SIZE                      512u
#define U3V_REQ_TIMEOUT                             5000u
#define U3V_REQ_READWRITE_RETRY_TIMES_LMT           4800u


// DEBUG XULT board
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

#endif //COMPONENT_DRIVERS_USBCAMDRV_CONFIG_H
