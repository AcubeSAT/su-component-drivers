
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
 * These model name definitions match a specific entry in u3vCamRegisterCfg. For
 *  any new camera model to be added in the driver, the model name needs to be 
 * defined here with the corresponding entry in the u3vCamRegisterCfg struct to 
 * match dynamic register address values (see u3vCamRegisterCfg definition).
 * @note To enable a specific camera model for the current build, simply define 
 * its name to U3V_CAM_MODEL_SELECTED
 */
#define U3V_FLIR_CM3_U3_12S2C_CS                        (0)
#define U3V_XIMEA_XIQ_MQ013CG_E2                        (1)
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
 * U3V Host image data buffer max size.
 * 
 * Max buffer size in bytes for the transfer of the image payload data.
 * @warning The size shall be a multiple of the byte alignment size of the MCU 
 * architecture and a value supported by the USB2.0-HS protocol.
 * @note For optimized results, prefer using 1024 or 512 bytes. 
 */
#define U3V_IN_BUFFER_MAX_SIZE                          (1024U)

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a leader
 * packet will not exceed 70 bytes).
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_LEADER_MAX_SIZE                             (256U)

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a 
 * trailer packet will not exceed 40 bytes).
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_TRAILER_MAX_SIZE                            (256U)

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


/********************************************************
* Type definitions
*********************************************************/

/**
 * U3V Camera register configuration. 
 * 
 * This config struct contains all the essential register addresses and 
 * constants being used by the U3VCamDriver.
 * @note Corresponding values can be obtained by the XML document or application
 * datasheet of the specific U3V device. All USB3 Vision compliant devices shall
 * share the same naming convention as the USB3 Vision standard dictates. You 
 * may use the struct member names below to look-up for their values.
 */
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
} T_U3VCamRegisterCfg;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONFIG_H
