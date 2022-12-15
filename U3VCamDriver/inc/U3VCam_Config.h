
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
 * U3VCamDriver Camera model select.
 * 
 * Here are defined the supported 'USB3 Vision' camera models with existing
 * configuration mapping. By selecting a specific model, the corresponding
 * configurations regarding register addresses, conversion formulas and preset
 * values, are enabled in the model configuration mapping below.
 * @note To select a specific camera model for the current build, simply assign 
 * its name into U3V_CAM_MODEL_SELECTED definition
 */
#define U3V_FLIR_CM3_U3_12S2C_CS                        (0)
#define U3V_NEW_MODEL_UNDEFINED                         (1)
#define U3V_CAM_MODEL_SELECTED                          (U3V_FLIR_CM3_U3_12S2C_CS)


/**
 * U3VCamDriver Selected camera model configuration mapping.
 * 
 * Here are defined the camera model specific configurations regarding register 
 * addresses, conversion formulas and preset values.
 * @note When adding a new model below, copy all areas of an existing model and
 * replace the corresponding values for each field by using the device's U3V
 * manifest (XML file or datasheet).
 * @warning Some registers of specific models may require a unique conversion
 * for the get/set of values on registers, check manifest document for each.
 * When no conversion is necessary simpy use 1:1 conversion macros, this means
 * that the integer value will be assigned as a full 32bit value on register.
 */
#if (U3V_CAM_MODEL_SELECTED == U3V_FLIR_CM3_U3_12S2C_CS)                                    /* U3V XML manifest info */
    #define U3V_CAM_CFG_REG_BASE_ADR                    ((uint64_t)0xFFFFF0F00000U)         /* CamRegBaseAddress */
    #define U3V_CAM_CFG_TEMPERATURE_REG_ADR             ((uint64_t)0x082CU)                 /* Temperature_Reg */
    #define U3V_CAM_CFG_DEVICE_RESET_REG_ADR            ((uint64_t)0x400CU)                 /* DeviceReset_CtrlValueReg */
    #define U3V_CAM_CFG_ACQ_MODE_REG_ADR                ((uint64_t)0x4028U)                 /* AcquisitionMode_Reg */
    #define U3V_CAM_CFG_ACQ_START_REG_ADR               ((uint64_t)0x4030U)                 /* AcquisitionStart_Reg */
    #define U3V_CAM_CFG_ACQ_STOP_REG_ADR                ((uint64_t)0x0614U)                 /* AcquisitionStop_Reg */
    #define U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR            ((uint64_t)0x4070U)                 /* ColorCodingID_Reg */
    #define U3V_CAM_CFG_PAYLOAD_SIZE_REG_ADR            ((uint64_t)0x5410U)                 /* PayloadSizeVal_Reg */
    #define U3V_CAM_CFG_ACQ_MODE_SEL                    (0x01UL)                            /* 0 = CONTINUOUS / 1 = SINGLE_FRAME / 2 = MULTI_FRAME */
    #define U3V_CAM_CFG_PIXEL_FORMAT_SEL                (0x04UL)                            /* 04 = U3V_PFNC_RGB8 = 0x02180014 in PixelFormatCtrlVal_Int formula */
    #define U3V_GET_ACQ_MODE_CONV(val)                  (val)                               /* no conversion */
    #define U3V_SET_ACQ_MODE_CONV(val)                  (val)                               /* no conversion */ 
    #define U3V_SET_ACQ_START_CONV(val)                 ((val << 31U) & 0x80000000UL)       /* bit 31 */
    #define U3V_SET_ACQ_STOP_CONV(val)                  ((val << 31U) & 0x80000000UL)       /* bit 31 */
    #define U3V_SET_DEVICE_RESET_CONV(val)              (val)                               /* bit 0, no need for conversion */
    #define U3V_GET_PAYLOAD_SIZE_CONV(val)              (val)                               /* no conversion */
    #define U3V_GET_PIXEL_FORMAT_CONV(val)              ((val >> 24U) & 0x000000FFUL)       /* value is stored on high byte (bits 24 to 31) */
    #define U3V_SET_PIXEL_FORMAT_CONV(val)              ((val << 24U) & 0xFF000000UL)       /* value is stored on high byte (bits 24 to 31) */
    #define U3V_GET_TEMPERATURE_CONV(val)               (((float)(val & 0xFFFUL) / 10.0F) - 273.15F) /* convert Kelvin to Celcius (from unsigned int input) */

#elif (U3V_CAM_MODEL_SELECTED == U3V_NEW_MODEL_UNDEFINED)                                   /* U3V XML manifest info */
    #define U3V_CAM_CFG_REG_BASE_ADR                    ((uint64_t)0x00U)                   /* CamRegBaseAddress (use 0 when N/A) */
    #define U3V_CAM_CFG_TEMPERATURE_REG_ADR             ((uint64_t)0x00U)                   /* DeviceTemperatureReg */
    #define U3V_CAM_CFG_DEVICE_RESET_REG_ADR            ((uint64_t)0x00U)                   /* DeviceResetReg */
    #define U3V_CAM_CFG_ACQ_MODE_REG_ADR                ((uint64_t)0x00U)                   /* AcquisitionModeReg */
    #define U3V_CAM_CFG_ACQ_START_REG_ADR               ((uint64_t)0x00U)                   /* AcquisitionStartReg */
    #define U3V_CAM_CFG_ACQ_STOP_REG_ADR                ((uint64_t)0x00U)                   /* AcquisitionStopReg */
    #define U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR            ((uint64_t)0x00U)                   /* PixelFormatReg */
    #define U3V_CAM_CFG_PAYLOAD_SIZE_REG_ADR            ((uint64_t)0x00U)                   /* PayloadSizeVal_Reg */
    #define U3V_CAM_CFG_ACQ_MODE_SEL                    (0x01UL)                            /* use value for single image acq */
    #define U3V_CAM_CFG_PIXEL_FORMAT_SEL                (0UL)                               /* PFNC option */
    #define U3V_GET_ACQ_MODE_CONV(val)                  (val)                               /* no conversion */
    #define U3V_SET_ACQ_MODE_CONV(val)                  (val)                               /* no conversion */
    #define U3V_SET_ACQ_START_CONV(val)                 (val)                               /* no conversion */
    #define U3V_SET_ACQ_STOP_CONV(val)                  (val)                               /* no conversion */
    #define U3V_SET_DEVICE_RESET_CONV(val)              (val)                               /* no conversion */ 
    #define U3V_GET_PAYLOAD_SIZE_CONV(val)              (val)                               /* no conversion */
    #define U3V_GET_PIXEL_FORMAT_CONV(val)              (val)                               /* no conversion */
    #define U3V_SET_PIXEL_FORMAT_CONV(val)              (val)                               /* no conversion */
    #define U3V_GET_TEMPERATURE_CONV(val)               (*(float*)&(val))                   /* value in Celcius (convert to float type from unsigned int input) */
    #warning "Template USB3 Vision camera model selected, non functional"   //remove this when model is defined

#else
    #error "Invalid USB3 Vision camera model selected"
#endif

/**
 * U3VCamDriver error report callout function.
 * 
 * This macro can be used optionally as a 'stub' function for calling an 
 * external error handler to report or log the error event by U3V App.
 */
#define U3V_REPORT_ERROR(errID)                         (void)(errID) //TODO: add error report IF

/**
 * U3V Host supported instances (devices) number.
 * 
 * @warning Current implementation is designed to support only 1 device.
 */
#define U3V_HOST_INSTANCES_NUMBER                       (1U)

/**
 * U3V Host attach listeners number.
 * 
 * Defines how many 'wait to connect' instances can be active in parallel.
 * @note As long as the U3V_HOST_INSTANCES_NUMBER is 1, then the number of 
 * listeners shall not be more than 1.
 */
#define U3V_HOST_ATTACH_LISTENERS_NUMBER                (1U)

/**
 * U3V Host Control Interface wait for acknowledge delay (ms).
 * 
 * Defines the delay time when re-checking the acknowledge message from client
 * when using CtrlIfReadMemory and CtrlIfWriteMemory functions.
 * @note Should usually be equal to the task cyclic run time interval, not less.
 */
#define U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS          (10U)

/**
 * U3V Host image payload data block max size.
 * 
 * Max size in bytes for the transfer block of the image payload data.
 * @warning The size shall be a multiple of the byte alignment size of the MCU 
 * architecture and preferably a binary multiple of 1024. May be device 
 * dependent.
 * @warning Always make sure that the destination buffer of the received image
 * is at least equal or larger than this, especially when RAM space is used.
 */
#define U3V_PAYLD_BLOCK_MAX_SIZE                        (0x8000)    /* 32768 */

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a leader
 * packet will not exceed 70 bytes).
 * @warning Cannot be greater than U3V_PAYLD_BLOCK_MAX_SIZE
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_LEADER_MAX_SIZE                             (1024U)

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a 
 * trailer packet will not exceed 40 bytes).
 * @warning Cannot be greater than U3V_PAYLD_BLOCK_MAX_SIZE
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_TRAILER_MAX_SIZE                            (1024U)

/**
 * U3V Host request timeout.
 * 
 * This is the Control Interface R/W request timeout in milliseconds. After a 
 * message is discpatched over the USB transfer layer, if there is no response
 * from the device in this period of time, then the Control Interface will 
 * return an error. For most transactions the normal response is less than 
 * 900ms.
 */
#define U3V_REQ_TIMEOUT_MS                              (1600UL)

/**
 * U3V Host architecture memory byte alignment.
 * 
 * This is the memory alignment (byte padding) supported by the target MCU 
 * architecture. This value will be used for calculating the number and the 
 * segmentation of the payload blocks for the image data transfer.  
 */
#define U3V_TARGET_ARCH_BYTE_ALIGNMENT                  (8U)

/**
 * U3V Trigger reset command value.
 * 
 */
#define U3V_DEVICE_RESET_CMD                            (1UL)

/**
 * U3V Acquisition start command.
 * 
 */
#define U3V_ACQUISITION_START_CMD                       (1UL)

/**
 * U3V Acquisition start command.
 * 
 */
#define U3V_ACQUISITION_STOP_CMD                        (0UL)



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
