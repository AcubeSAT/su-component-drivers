//
// Created by mojo on 04/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_H
#define COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "usb/usb_host_client_driver.h"
#include "UsbCamDrv_DeviceClassSpec_U3V.h"


/********************************************************
* Macro definitions
*********************************************************/

#define U3V_HOST_RESULT_MIN                         (USB_HOST_RESULT_MIN)

#define U3V_HOST_HANDLE_INVALID                    ((T_U3VHostHandle)(-1))
#define U3V_HOST_TRANSFER_HANDLE_INVALID           ((T_U3VHostTransferHandle)(-1))
#define U3V_HOST_REQUEST_HANDLE_INVALID            ((T_U3VHostRequestHandle)(-1))

#define U3V_INTERFACE                              (&gUSBHostU3VClientDriver)


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_U3VHostHandle;
typedef uintptr_t T_U3VHostTransferHandle;
typedef uintptr_t T_U3VHostRequestHandle;
typedef uintptr_t T_U3VHostObject;
typedef USB_HOST_DEVICE_OBJ_HANDLE T_U3VHostDeviceObjHandle;  /* Inherited type from usb_host.h */


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

typedef struct
{   
    T_U3VHostTransferHandle  transferHandle;      /* Transfer handle of this transfer */
    T_U3VHostResult          result;              /* Termination transfer status */
    size_t                   length;              /* Size of the data transferred in the request */
} T_U3VHostEventReadCompleteData,
    T_U3VHostEventWriteCompleteData;

typedef enum
{
    U3V_HOST_EVENT_READ_COMPLETE,
    U3V_HOST_EVENT_WRITE_COMPLETE
} T_U3VHostEvent;

typedef enum
{
    U3V_HOST_EVENT_RESPONE_NONE  = 0     /* This means no response is required */

} T_U3VHostEventResponse;

typedef void (*T_U3VHostAttachEventHandler)(T_U3VHostObject u3vObjHandle, uintptr_t context);

typedef void (*T_U3VHostDetachEventHandler)(T_U3VHostHandle u3vHandle, uintptr_t context);

typedef T_U3VHostEventResponse (*T_U3VHostEventHandler)(T_U3VHostHandle u3vHandle,
                                                        T_U3VHostEvent event,
                                                        void *eventData,
                                                        uintptr_t context);

typedef struct 
{
	uint32_t    genCpVersion;                               //todo remove
	uint32_t    u3vVersion;                                 //todo remove
	char        deviceGuid[U3V_MAX_DESCR_STR_LENGTH];       //todo remove
	char        vendorName[U3V_MAX_DESCR_STR_LENGTH];       //todo remove
	char        modelName[U3V_MAX_DESCR_STR_LENGTH];        //todo remove
	char        familyName[U3V_MAX_DESCR_STR_LENGTH];       //todo remove
	char        deviceVersion[U3V_MAX_DESCR_STR_LENGTH];    //todo remove
	char        manufacturerInfo[U3V_MAX_DESCR_STR_LENGTH]; //todo remove
	char        serialNumberU3v[U3V_MAX_DESCR_STR_LENGTH];
	char        userDefinedName[U3V_MAX_DESCR_STR_LENGTH];  //todo remove
	uint8_t     speedSupport;                               //todo remove
	uint8_t     previouslyInitialized;
	uint32_t    hostByteAlignment;
	uint32_t    osMaxTransferSize;
	uint64_t    sirmAddr;
	uint32_t    transferAlignment;
	uint32_t    segmentedXferSupported;
	uint32_t    segmentedXferEnabled;
	uint32_t    legacyCtrlEpStallEnabled;
}T_U3VDeviceInfo;


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

T_U3VHostResult USB_U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler,
                                                  uintptr_t context);

T_U3VHostResult USB_U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle,
                                                  T_U3VHostDetachEventHandler detachEventHandler,
                                                  uintptr_t context);

T_U3VHostDeviceObjHandle USB_U3VHost_DeviceObjectHandleGet(T_U3VHostObject u3vDeviceObj);  //to review return type

T_U3VHostHandle USB_U3VHost_Open(T_U3VHostObject u3vDeviceObj);

void USB_U3VHost_Close(T_U3VHostHandle u3vDeviceHandle);

T_U3VHostResult USB_U3VHost_EventHandlerSet(T_U3VHostHandle handle,
                                            T_U3VHostEventHandler eventHandler,
                                            uintptr_t context);

T_U3VHostResult USB_U3VHost_GetManifestFile(T_U3VHostObject u3vDeviceObj);

T_U3VHostResult USB_U3VHost_GetPixelFormat(T_U3VHostObject u3vDeviceObj, uint32_t *const pixelCoding);

T_U3VHostResult USB_U3VHost_SetPixelFormat(T_U3VHostObject u3vDeviceObj, const uint32_t pixelCodingVal);

T_U3VHostResult USB_U3VHost_GetStreamCapabilities(T_U3VHostObject u3vDeviceObj);

T_U3VHostResult USB_U3VHost_GetCamSerialNumber(T_U3VHostObject u3vDeviceObj,  void *bfr);    /* buffer size must be at least 64bytes long */

T_U3VHostResult USB_U3VHost_GetCamFirmwareVersion(T_U3VHostObject u3vDeviceObj,  void *bfr);     /* buffer size must be at least 64bytes long */

T_U3VHostResult USB_U3VHost_GetCamTemperature(T_U3VHostObject u3vDeviceObj, float *const pCamTemp);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_H
