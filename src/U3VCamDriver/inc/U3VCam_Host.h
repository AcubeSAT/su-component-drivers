//
// Created by fomarko on 04/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_HOST_H
#define COMPONENT_DRIVERS_U3VCAM_HOST_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "usb/usb_host_client_driver.h"
#include "U3VCam_Device_Class_Specs.h"
#include "U3VCam_Config.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

#define U3V_HOST_RESULT_MIN                         (USB_HOST_RESULT_MIN)

#define U3V_HOST_HANDLE_INVALID                     ((T_U3VHostHandle)(-1))
#define U3V_HOST_TRANSFER_HANDLE_INVALID            ((T_U3VHostTransferHandle)(-1))
#define U3V_HOST_REQUEST_HANDLE_INVALID             ((T_U3VHostRequestHandle)(-1))

#define U3V_INTERFACE                               (&gUSBHostU3VClientDriver)


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_U3VHostHandle;
typedef uintptr_t T_U3VHostTransferHandle;
typedef uintptr_t T_U3VHostRequestHandle;
typedef USB_HOST_DEVICE_OBJ_HANDLE T_U3VHostDeviceObjHandle;

typedef enum
{
    U3V_HOST_RESULT_FAILURE             = U3V_HOST_RESULT_MIN,
    U3V_HOST_RESULT_BUSY,
    U3V_HOST_RESULT_REQUEST_STALLED,
    U3V_HOST_RESULT_INVALID_PARAMETER,
    U3V_HOST_RESULT_DEVICE_UNKNOWN,
    U3V_HOST_RESULT_ABORTED,
    U3V_HOST_RESULT_HANDLE_INVALID,
    U3V_HOST_RESULT_SUCCESS             = 1
} T_U3VHostResult;

typedef enum
{
    U3V_MEM_REG_INT_PIXELFORMAT         = 0x10,
    U3V_MEM_REG_INT_PAYLOAD_SIZE,
    U3V_MEM_REG_INT_ACQUISITION_MODE,
    U3V_MEM_REG_INT_DEVICE_RESET,
} T_U3VMemRegInteger;

typedef enum
{
    U3V_MEM_REG_FLOAT_TEMPERATURE       = 0x20,
} T_U3VMemRegFloat;

typedef enum
{
    U3V_MEM_REG_STRING_MANUFACTURER_NAME    = 0x40,
    U3V_MEM_REG_STRING_MODEL_NAME,
    U3V_MEM_REG_STRING_FAMILY_NAME,
    U3V_MEM_REG_STRING_DEVICE_VERSION,
    U3V_MEM_REG_STRING_MANUFACTURER_INFO,
    U3V_MEM_REG_STRING_SERIAL_NUMBER,
    U3V_MEM_REG_STRING_USER_DEFINED_NAME,
} T_U3VMemRegString;

typedef struct
{   
    T_U3VHostTransferHandle  transferHandle;
    T_U3VHostResult          result;
    size_t                   length;
} T_U3VHostEventReadCompleteData, T_U3VHostEventWriteCompleteData;

typedef enum
{
    U3V_HOST_EVENT_READ_COMPLETE = 0x10,
    U3V_HOST_EVENT_WRITE_COMPLETE,
    U3V_HOST_EVENT_IMG_PLD_RECEIVED,
} T_U3VHostEvent;

typedef enum
{
    U3V_HOST_EVENT_RESPONE_NONE  = 0     /* This means no response is required */
} T_U3VHostEventResponse;

typedef void (*T_U3VHostAttachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

typedef void (*T_U3VHostDetachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

typedef T_U3VHostEventResponse (*T_U3VHostEventHandler)(T_U3VHostHandle u3vObjHandle, T_U3VHostEvent event, void *eventData, uintptr_t context);

typedef struct 
{
	// uint32_t    genCpVersion;                                        //TODO: remove if never used
	// uint32_t    u3vVersion;                                          //TODO: remove if never used
	// char        deviceGuid[U3V_MAX_DESCR_STR_LENGTH];                //TODO: remove if never used
	// char        vendorName[U3V_REG_MANUFACTURER_NAME_SIZE];          //TODO: remove if never used
	// char        modelName[U3V_REG_MODEL_NAME_SIZE];                  //TODO: remove if never used
	// char        familyName[U3V_REG_FAMILY_NAME_SIZE];                //TODO: remove if never used
	// char        deviceVersion[U3V_REG_DEVICE_VERSION_SIZE];          //TODO: remove if never used
	// char        manufacturerInfo[U3V_REG_MANUFACTURER_INFO_SIZE];    //TODO: remove if never used
	// char        serialNumberU3v[U3V_REG_SERIAL_NUMBER_SIZE];         //TODO: remove if never used
	// char        userDefinedName[U3V_REG_USER_DEFINED_NAME_SIZE];     //TODO: remove if never used
	// uint8_t     speedSupport;                                        //TODO: remove if never used
    // uint8_t     previouslyInitialized;                               //TODO: remove if never used
	uint32_t    hostByteAlignment;
	// uint32_t    osMaxTransferSize;                                   //TODO: remove if never used
	uint64_t    sirmAddr;
	uint32_t    transferAlignment;
	// uint32_t    segmentedXferSupported;                              //TODO: remove if never used
	// uint32_t    segmentedXferEnabled;                                //TODO: remove if never used
	// uint32_t    legacyCtrlEpStallEnabled;                            //TODO: remove if never used
} T_U3VDeviceInfo;


typedef struct 
{
	uint64_t imageSize;
	uint64_t chunkDataSize;
	uint32_t maxBlockSize;
	uint32_t blockPadding;
	uint64_t blockSize;
	uint32_t maxLeaderSize;
	uint32_t maxTrailerSize;
} T_U3VStreamIfConfig;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/

extern USB_HOST_CLIENT_DRIVER   gUSBHostU3VClientDriver;


/********************************************************
* Function declarations
*********************************************************/

T_U3VHostResult U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context);

T_U3VHostResult U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle, T_U3VHostDetachEventHandler detachEventHandler, uintptr_t context);

T_U3VHostHandle U3VHost_Open(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_EventHandlerSet(T_U3VHostHandle handle, T_U3VHostEventHandler eventHandler, uintptr_t context);

T_U3VHostResult U3VHost_GetStreamCapabilities(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_SetupStreamTransferParams(T_U3VHostHandle u3vObjHandle, T_U3VStreamIfConfig *streamConfig);

T_U3VHostResult U3VHost_StreamChControl(T_U3VHostHandle u3vObjHandle, bool enable);

T_U3VHostResult U3VHost_AcquisitionStart(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_AcquisitionStop(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_StartImgPayldTransfer(T_U3VHostHandle u3vObjHandle, void *imgBfr, size_t size);

T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VHostHandle u3vObjHandle);

void U3VHost_CtrlIf_InterfaceDestroy(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_ReadMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t *const pReadValue);

T_U3VHostResult U3VHost_ReadMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat floatReg, float *const pReadValue);

T_U3VHostResult U3VHost_ReadMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString stringReg, char *const pReadBfr); /* string buffer size must be 64bytes long (at least) */

T_U3VHostResult U3VHost_WriteMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t writeValue);

T_U3VHostResult U3VHost_WriteMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat integerReg, float writeValue);

T_U3VHostResult U3VHost_WriteMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString integerReg, const char *pWriteBfr); /* string buffer size must be 64bytes long (at least) */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_HOST_H
