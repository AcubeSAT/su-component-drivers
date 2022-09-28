
#ifndef COMPONENT_DRIVERS_U3VCAM_CONFIG_H
#define COMPONENT_DRIVERS_U3VCAM_CONFIG_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

/**
 * U3VCamDriver Selected camera model defines.
 * 
 * These model name definitions match a specific entry in
 * u3vCamRegisterCfg. For any new camera model to be
 * added in the driver, the model name needs to be defined
 * here with the corresponding entry in the 
 * u3vCamRegisterCfg table to match dynamic register
 * address values.
 * Then to enable a specific camera model for the current
 * build, simply define its name to U3V_CAM_MODEL_SELECTED
 */
#define U3V_FLIR_CM3_U3_12S2C_CS                        (0)
#define U3V_XIMEA_XIQ_MQ013CG_E2                        (1)
/* U3V camera model selected for current build */
#define U3V_CAM_MODEL_SELECTED                          (U3V_FLIR_CM3_U3_12S2C_CS)

#if (U3V_CAM_MODEL_SELECTED == U3V_FLIR_CM3_U3_12S2C_CS)
    #define U3V_CFG_MDL_CAM_REG_BASE_ADDRESS            (0xFFFFF0F00000)    /* CamRegBaseAddress */
    #define U3V_CFG_MDL_SBRM_OFFSET                     (0x200000)          /* SBRMOffset */
    #define U3V_CFG_MDL_TEMPERATURE_REG                 (0x82C)             /* Temperature_Reg */
    #define U3V_CFG_MDL_DEVICE_RESET_REG                (0x400C)            /* DeviceReset_CtrlValueReg */
    #define U3V_CFG_MDL_SINGLE_FRAME_ACQ_MODE_REG       (0x638)             /* SingleFrameAcquisitionMode_Reg */
    #define U3V_CFG_MDL_ACQ_MODE_REG                    (0x4028)            /* AcquisitionMode_Reg */
    #define U3V_CFG_MDL_ACQ_START_REG                   (0x4030)            /* AcquisitionStart_Reg */
    #define U3V_CFG_MDL_ACQ_STOP_REG                    (0x614)             /* AcquisitionStop_Reg */
    #define U3V_CFG_MDL_COLOR_CODING_ID_REG             (0x4070)            /* ColorCodingID_Reg */
    #define U3V_CFG_MDL_PAYLOAD_SIZE_VAL_REG            (0x5410)            /* PayloadSizeVal_Reg */
    #define U3V_CFG_MDL_PIXEL_FORMAT_CTRL_INT_SEL       (0x04)              /* U3V_PFNC_RGB8 = 0x02180014 -> 04 in PixelFormatCtrlVal_Int formula */
#elif (U3V_CAM_MODEL_SELECTED == U3V_XIMEA_XIQ_MQ013CG_E2)
    #define U3V_CFG_MDL_CAM_REG_BASE_ADDRESS            (0x0)               /* CamRegBaseAddress */
    #define U3V_CFG_MDL_SBRM_OFFSET                     (0x0)               /* SBRMOffset */
    #define U3V_CFG_MDL_TEMPERATURE_REG                 (0x0)               /* Temperature_Reg */
    #define U3V_CFG_MDL_DEVICE_RESET_REG                (0x0)               /* DeviceReset_CtrlValueReg */
    #define U3V_CFG_MDL_SINGLE_FRAME_ACQ_MODE_REG       (0x0)               /* SingleFrameAcquisitionMode_Reg */
    #define U3V_CFG_MDL_ACQ_MODE_REG                    (0x0)               /* AcquisitionMode_Reg */
    #define U3V_CFG_MDL_ACQ_START_REG                   (0x0)               /* AcquisitionStart_Reg */
    #define U3V_CFG_MDL_ACQ_STOP_REG                    (0x0)               /* AcquisitionStop_Reg */
    #define U3V_CFG_MDL_COLOR_CODING_ID_REG             (0x0)               /* ColorCodingID_Reg */
    #define U3V_CFG_MDL_PAYLOAD_SIZE_VAL_REG            (0x0)               /* PayloadSizeVal_Reg */
    #define U3V_CFG_MDL_PIXEL_FORMAT_CTRL_INT_SEL       (0x0)               /* U3V_PFNC_RGB8 = x -> y in PixelFormatCtrlVal_Int formula */
#else
    #error "Invalid USB3 Vision camera model selected"
#endif

/**
 * U3VCamDriver error report callout function.
 * 
 * This is a 'stub' function to match optionally
 * set an error handler function to report or log
 * the error event.
 */
#define U3V_REPORT_ERROR(errID)                         (void)(errID) //TODO: add error report IF

#define U3V_HOST_INSTANCES_NUMBER                       (1U)
#define U3V_HOST_ATTACH_LISTENERS_NUMBER                (1U)
#define U3V_IN_BUFFER_MAX_SIZE                          (1024U)
#define U3V_LEADER_MAX_SIZE                             (256U)
#define U3V_TRAILER_MAX_SIZE                            (256U)
#define U3V_REQ_TIMEOUT_MS                              (1600UL)
#define U3V_TARGET_ARCH_BYTE_ALIGNMENT                  (8U)        /* Byte alignment / padding of MCU architecture */

//TODO: remove on integration all defs below, used for debug (XULT board specific)
#include "device.h"
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



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
