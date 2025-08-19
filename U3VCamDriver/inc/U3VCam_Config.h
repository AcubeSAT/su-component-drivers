#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * U3VCamDriver Camera model select.
 * 
 * Here are defined the supported 'USB3 Vision' camera models with existing
 * configuration mapping. By defining a specific model below, the corresponding
 * configurations regarding register addresses, conversion formulas and preset
 * values, are enabled in the model configuration mapping below.
 * @warning Define only one model at a time, and undefine the rest.
 */
#undef  U3V_FLIR_CM3_U3_12S2C_CS
#define  U3V_FLIR_BFS_U3_16S2C_CS


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
/*******************************************************************************
 * FLIR Chameleon3 CM3-U3-13S2C-CS
 ******************************************************************************/
#if defined(U3V_FLIR_CM3_U3_12S2C_CS)                                           /* U3V XML manifest info */
    #define U3V_CAM_CFG_REG_BASE_ADR                (UINT64_C(0xFFFFF0F00000))  /* CamRegBaseAddress */
    #define U3V_CAM_CFG_TEMPERATURE_REG_ADR         (UINT64_C(0x082C))          /* Temperature_Reg */
    #define U3V_CAM_CFG_DEVICE_RESET_REG_ADR        (UINT64_C(0x400C))          /* DeviceReset_CtrlValueReg */
    #define U3V_CAM_CFG_IMG_PRESET_CURRENT_REG_ADR  (UINT64_C(0x0624))          /* CurMemCh_Reg - RO */
    #define U3V_CAM_CFG_IMG_PRESET_SELECT_REG_ADR   (UINT64_C(0x0620))          /* MemSaveCh_Reg - RW */
    #define U3V_CAM_CFG_IMG_PRESET_LOAD_REG_ADR     (UINT64_C(0x5114))          /* UserSetLoad_CtrlValueReg - WO */
    #define U3V_CAM_CFG_ACQ_MODE_REG_ADR            (UINT64_C(0x4028))          /* AcquisitionMode_Reg */
    #define U3V_CAM_CFG_ACQ_START_REG_ADR           (UINT64_C(0x4030))          /* AcquisitionStart_Reg */
    #define U3V_CAM_CFG_ACQ_STOP_REG_ADR            (UINT64_C(0x0614))          /* AcquisitionStop_Reg */
    #define U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR        (UINT64_C(0x4070))          /* ColorCodingID_Reg */
    #define U3V_CAM_CFG_PAYLOAD_SIZE_REG_ADR        (UINT64_C(0x5410))          /* PayloadSizeVal_Reg */
    #define U3V_CAM_CFG_ACQ_MODE_SEL                (UINT32_C(0x1))             /* 0 = CONTINUOUS / 1 = SINGLE_FRAME / 2 = MULTI_FRAME */
    #define U3V_CAM_CFG_PIXEL_FORMAT_SEL            (UINT32_C(0x4))             /* 4 = 0x02180014 = U3V_PFNC_RGB8 in PixelFormatCtrlVal_Int formula */
    #define U3V_DEVICE_RESET_CMD                    (UINT32_C(0x1))             /* 1 = reset true */
    #define U3V_ACQUISITION_START_CMD               (UINT32_C(0x1))             /* 1 = acq start true */
    #define U3V_ACQUISITION_STOP_CMD                (UINT32_C(0x0))             /* 0 = acq stop true */
    #define U3V_SI_CTRL_ENABLE_CMD                  (UINT32_C(0x1))             /* 1 = SI control enable true */
    #define U3V_SI_CTRL_DISABLE_CMD                 (UINT32_C(0x0))             /* 1 = SI control disable true */
    #define U3V_CAM_IMG_PRESET_DEFAULT_SET          (UINT32_C(0x0))             /* UserSetSelector: Default set (0) */
    #define U3V_CAM_IMG_PRESET_USER_SET_0           (UINT32_C(0x1))             /* UserSetSelector: User set 0 (1) */
    #define U3V_CAM_IMG_PRESET_USER_SET_1           (UINT32_C(0x2))             /* UserSetSelector: User set 1 (2) */
    #define U3V_SET_IMG_PRESET_LOAD_CMD(val)        (val)                       /* UserSetLoad command depends on UserSetSelector loadout */
    #define U3V_GET_IMG_PRESET_CURRENT_CONV(val)    ((val & 0xF0000000) >> 28)  /* CurMemCh_Reg: value is stored on highest 4 bits (bits 28 to 31) */
    #define U3V_GET_IMG_PRESET_SELECT_CONV(val)     (val)                       /* MemSaveCh_Reg: no conversion */
    #define U3V_SET_IMG_PRESET_SELECT_CONV(val)     (val)                       /* MemSaveCh_Reg: no conversion */
    #define U3V_SET_IMG_PRESET_LOAD_CONV(val)       ((val & 0x0000000F) << 28)  /* UserSetLoad_CtrlValueReg: value is stored on highest 4 bits (bits 28 to 31) */
    #define U3V_GET_ACQ_MODE_CONV(val)              (val)                       /* no conversion */
    #define U3V_SET_ACQ_MODE_CONV(val)              (val)                       /* no conversion */ 
    #define U3V_SET_ACQ_START_CONV(val)             ((val & 0x00000001) << 31)  /* bit 31 */
    #define U3V_SET_ACQ_STOP_CONV(val)              ((val & 0x00000001) << 31)  /* bit 31 */
    #define U3V_SET_DEVICE_RESET_CONV(val)          (val)                       /* bit 0, no conversion */
    #define U3V_GET_PAYLOAD_SIZE_CONV(val)          (val)                       /* no conversion */
    #define U3V_GET_PIXEL_FORMAT_CONV(val)          ((val & 0xFF000000) >> 24)  /* value is stored on high byte (bits 24 to 31) */
    #define U3V_SET_PIXEL_FORMAT_CONV(val)          ((val & 0x000000FF) << 24)  /* value is stored on high byte (bits 24 to 31) */
    #define U3V_GET_TEMPERATURE_CONV(val)           (((float)(val & 0x00000FFF) / 10.0F) - 273.15F) /* convert Kelvin to Celsius (from unsigned int input) */
/*******************************************************************************
 * FLIR Blackfly S BFS-U3-16S2C-CS
 ******************************************************************************/
#elif defined(U3V_FLIR_BFS_U3_16S2C_CS)                                         /* U3V XML manifest info */
    #define U3V_CAM_CFG_REG_BASE_ADR                (UINT64_C(0x0))             /* CamRegBaseAddress (use 0 when N/A) */
    #define U3V_CAM_CFG_TEMPERATURE_REG_ADR         (UINT64_C(0x00041004))      /* DeviceTemperatureSensor_Val */
    #define U3V_CAM_CFG_DEVICE_RESET_REG_ADR        (UINT64_C(0x00042004))      /* DeviceReset_Val */
    #define U3V_CAM_CFG_IMG_PRESET_CURRENT_REG_ADR  (UINT64_C(0x00074008))      /* UserSetSelector_Val - RW */
    #define U3V_CAM_CFG_IMG_PRESET_SELECT_REG_ADR   (UINT64_C(0x00074008))      /* UserSetSelector_Val - RW */
    #define U3V_CAM_CFG_IMG_PRESET_LOAD_REG_ADR     (UINT64_C(0x00074024))      /* UserSetLoad_Val - RW */
    #define U3V_CAM_CFG_ACQ_MODE_REG_ADR            (UINT64_C(0x000C00C8))      /* AcquisitionMode_Val */
    #define U3V_CAM_CFG_ACQ_START_REG_ADR           (UINT64_C(0x000C0004))      /* AcquisitionStart_Val */
    #define U3V_CAM_CFG_ACQ_STOP_REG_ADR            (UINT64_C(0x000C0024))      /* AcquisitionStop_Val */
    #define U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR        (UINT64_C(0x00086008))      /* PixelFormat_Val */
    #define U3V_CAM_CFG_PAYLOAD_SIZE_REG_ADR        (UINT64_C(0x20002008))      /* PayloadSize_Val */
    #define U3V_CAM_CFG_ACQ_MODE_SEL                (UINT32_C(0x1))             /* 0 = CONTINUOUS / 1 = SINGLE_FRAME / 2 = MULTI_FRAME */
    #define U3V_CAM_CFG_PIXEL_FORMAT_SEL            (UINT32_C(0x02180014))      /* 0x02180014 = U3V_PFNC_RGB8 */
    #define U3V_DEVICE_RESET_CMD                    (UINT32_C(0x1))             /* 1 = reset true */
    #define U3V_ACQUISITION_START_CMD               (UINT32_C(0x1))             /* 1 = acq start true */
    #define U3V_ACQUISITION_STOP_CMD                (UINT32_C(0x1))             /* 1 = acq stop true */
    #define U3V_SI_CTRL_ENABLE_CMD                  (UINT32_C(0x1))             /* 1 = SI control enable true */
    #define U3V_SI_CTRL_DISABLE_CMD                 (UINT32_C(0x0))             /* 1 = SI control disable true */
    #define U3V_CAM_IMG_PRESET_DEFAULT_SET          (UINT32_C(0x0))             /* UserSetSelector: Default set (0) */
    #define U3V_CAM_IMG_PRESET_USER_SET_0           (UINT32_C(0x1F))            /* UserSetSelector: User set 0 (31) */
    #define U3V_CAM_IMG_PRESET_USER_SET_1           (UINT32_C(0x1E))            /* UserSetSelector: User set 1 (30) */
    #define U3V_SET_IMG_PRESET_LOAD_CMD(val)        (UINT32_C(0x1))             /* UserSetLoad command (1) */
    #define U3V_GET_IMG_PRESET_CURRENT_CONV(val)    (val)                       /* UserSetSelector_Val: no conversion */
    #define U3V_GET_IMG_PRESET_SELECT_CONV(val)     (val)                       /* UserSetSelector_Val: no conversion */
    #define U3V_SET_IMG_PRESET_SELECT_CONV(val)     (val)                       /* UserSetSelector_Val: no conversion */
    #define U3V_SET_IMG_PRESET_LOAD_CONV(val)       (val)                       /* UserSetLoad_Val: no conversion */
    #define U3V_GET_ACQ_MODE_CONV(val)              (val)                       /* no conversion */
    #define U3V_SET_ACQ_MODE_CONV(val)              (val)                       /* no conversion */
    #define U3V_SET_ACQ_START_CONV(val)             (val)                       /* no conversion */
    #define U3V_SET_ACQ_STOP_CONV(val)              (val)                       /* no conversion */
    #define U3V_SET_DEVICE_RESET_CONV(val)          (val)                       /* no conversion */ 
    #define U3V_GET_PAYLOAD_SIZE_CONV(val)          (val)                       /* no conversion */
    #define U3V_GET_PIXEL_FORMAT_CONV(val)          (val)                       /* no conversion */
    #define U3V_SET_PIXEL_FORMAT_CONV(val)          (val)                       /* no conversion */
    #define U3V_GET_TEMPERATURE_CONV(val)           (((float)(val & 0x0000FFFF) / 10.0F)) /* Celsius (from unsigned int input) */
/******************************************************************************/
#else
    #error "Invalid USB3 Vision camera model selected"
#endif


/**
 * U3VCamDriver static assert
 * 
 * Macro used for static assertion during compile time for critical entities.
 */
#ifdef __cplusplus
    #define U3V_STATIC_ASSERT                       static_assert
#else
    #define U3V_STATIC_ASSERT                       _Static_assert
#endif

/**
 * U3VCamDriver error report callout function.
 * 
 * This macro can be used optionally as a 'stub' function for calling an 
 * external error handler to report or log the error event by U3V App.
 */

/**
 * U3V Host supported instances (devices) number.
 * 
 * @warning Current implementation is designed to support only 1 device.
 */
#define U3V_HOST_INSTANCES_NUMBER                   UINT32_C(1)

/**
 * U3V Host attach listeners number.
 * 
 * Defines how many 'wait to connect' instances can be active in parallel.
 * @note As long as the U3V_HOST_INSTANCES_NUMBER is 1, then the number of 
 * listeners shall not be more than 1.
 */
#define U3V_HOST_ATTACH_LISTENERS_NUMBER            UINT32_C(1)

/**
 * U3V Host Control Interface wait for acknowledge delay (ms).
 * 
 * Defines the delay time when re-checking the acknowledge message from client
 * when using CtrlIfReadMemory and CtrlIfWriteMemory functions.
 * @note Should usually be equal to the task cyclic run time interval, not less.
 */
#define U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS      UINT32_C(10)

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
#define U3V_PAYLD_BLOCK_MAX_SIZE                    ((size_t)0x8000)   /* 32768 */

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a leader
 * packet will not exceed 70 bytes).
 * @warning Cannot be greater than U3V_PAYLD_BLOCK_MAX_SIZE
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_LEADER_MAX_SIZE                         ((size_t)1024)

/**
 * U3V Payload leader max size.
 * 
 * Max size supported by the host to hold leader packer data. (Normally a 
 * trailer packet will not exceed 40 bytes).
 * @warning Cannot be greater than U3V_PAYLD_BLOCK_MAX_SIZE
 * @note This value will not occupy any buffer space in RAM, but will simply be 
 * sent to the connected U3V device as the USB3 Vision protocol requires.
 */
#define U3V_TRAILER_MAX_SIZE                        ((size_t)1024)

/**
 * U3V Host request timeout.
 * 
 * This is the Control Interface R/W request timeout in milliseconds. After a 
 * message is discpatched over the USB transfer layer, if there is no response
 * from the device in this period of time, then the Control Interface will 
 * return an error. For most transactions the normal response is less than 
 * 900ms.
 */
#define U3V_REQ_TIMEOUT_MS                          UINT32_C(1600)

/**
 * U3V Host architecture memory byte alignment.
 * 
 * This is the memory alignment (byte padding) supported by the target MCU 
 * architecture. This value will be used for calculating the number and the 
 * segmentation of the payload blocks for the image data transfer.  
 */
#define U3V_TARGET_ARCH_BYTE_ALIGNMENT              UINT32_C(8)

/**
 * U3V Control Interface Command buffer max size.
 * 
 * Defines the maximum size of the command (CMD) buffer used by the Control
 * Interface.
 * @note Minimum size shall be at least the size of T_U3VCtrlIfCmdHeader + 
 * U3V_MAX_DESCR_STR_LENGTH.  
 */
#define U3V_CTRL_IF_CMD_BUFFER_MAX_SIZE             ((size_t)84)

/**
 * U3V Control Interface Acknowledge buffer max size.
 * 
 * Defines the maximum size of the acknowledge (ACK) buffer used by the Control
 * Interface.
 * @note Minimum size shall be at least the size of T_U3VCtrlIfAckHeader + 
 * U3V_MAX_DESCR_STR_LENGTH.
 */
#define U3V_CTRL_IF_ACK_BUFFER_MAX_SIZE             ((size_t)76)



#ifdef __cplusplus
}
#endif //__cplusplus

