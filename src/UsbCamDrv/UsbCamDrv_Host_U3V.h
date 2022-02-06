//
// Created by mojo on 8/12/21.
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


/********************************************************
* Macro definitions
*********************************************************/


#define USB_HOST_U3V_RESULT_MIN                        -100

#define USB_HOST_U3V_HANDLE_INVALID                    ((T_UsbHostU3VHandle)(-1))
#define USB_HOST_U3V_TRANSFER_HANDLE_INVALID           ((T_UsbHostU3VTransferHandle)(-1))
#define USB_HOST_U3V_REQUEST_HANDLE_INVALID            ((T_UsbHostU3VRequestHandle)(-1))

#define USB_HOST_U3V_INTERFACE   &gUSBHostU3VClientDriver


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_UsbHostU3VHandle;
typedef uintptr_t T_UsbHostU3VTransferHandle;
typedef uintptr_t T_UsbHostU3VRequestHandle;
typedef uintptr_t T_UsbHostU3VObject;
typedef USB_HOST_DEVICE_OBJ_HANDLE T_UsbHostDeviceObjectHandle;  /* Inherited type from usb_host.h */


typedef enum
{
    USB_HOST_U3V_RESULT_FAILURE             = USB_HOST_U3V_RESULT_MIN,
    USB_HOST_U3V_RESULT_BUSY,
    USB_HOST_U3V_RESULT_REQUEST_STALLED,
    USB_HOST_U3V_RESULT_INVALID_PARAMETER,
    USB_HOST_U3V_RESULT_DEVICE_UNKNOWN,
    USB_HOST_U3V_RESULT_ABORTED,
    USB_HOST_U3V_RESULT_HANDLE_INVALID,
    USB_HOST_U3V_RESULT_SUCCESS             = 1
} T_UsbHostU3VResult;


typedef struct
{   
    T_UsbHostU3VRequestHandle   requestHandle;        /* Request handle of this request */   
    T_UsbHostU3VResult          result;               /* Termination status */
    size_t                      length;               /* Size of the data transferred in the request */
}
T_USB_HOST_U3V_EVENT_ACM_GET_LINE_CODING_COMPLETE_DATA,             
T_USB_HOST_U3V_EVENT_ACM_SET_LINE_CODING_COMPLETE_DATA,
T_USB_HOST_U3V_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE_DATA,
T_USB_HOST_U3V_EVENT_ACM_SEND_BREAK_COMPLETE_DATA;      //todo review


typedef struct
{   
    T_UsbHostU3VTransferHandle  transferHandle;      /* Transfer handle of this transfer */
    T_UsbHostU3VResult          result;              /* Termination transfer status */
    size_t                      length;              /* Size of the data transferred in the request */
}
T_USB_HOST_U3V_EVENT_SERIAL_STATE_NOTIFICATION_RECEIVED_DATA,
T_USB_HOST_U3V_EVENT_READ_COMPLETE_DATA,
T_USB_HOST_U3V_EVENT_WRITE_COMPLETE_DATA;               //todo review


typedef enum
{
    USB_HOST_U3V_EVENT_READ_COMPLETE,
    USB_HOST_U3V_EVENT_WRITE_COMPLETE,
    USB_HOST_U3V_EVENT_ACM_SEND_BREAK_COMPLETE,
    USB_HOST_U3V_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE,
    USB_HOST_U3V_EVENT_ACM_SET_LINE_CODING_COMPLETE,
    USB_HOST_U3V_EVENT_ACM_GET_LINE_CODING_COMPLETE,
    USB_HOST_U3V_EVENT_SERIAL_STATE_NOTIFICATION_RECEIVED,
    USB_HOST_U3V_EVENT_DEVICE_DETACHED
} T_UsbHostU3VEvent;        //todo review


typedef enum
{
    USB_HOST_U3V_EVENT_RESPONE_NONE  = 0     /* This means no response is required */

} T_UsbHostU3VEventResponse;


typedef void (*T_UsbHostU3VAttachEventHandler)(T_UsbHostU3VObject u3vObjHandle, uintptr_t context);


typedef T_UsbHostU3VEventResponse (*T_UsbHostU3VEventHandler)(T_UsbHostU3VHandle     u3vHandle,
                                                              T_UsbHostU3VEvent      event,
                                                              void                  *eventData,
                                                              uintptr_t              context);


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

T_UsbHostU3VResult USB_HostU3V_AttachEventHandlerSet(T_UsbHostU3VAttachEventHandler eventHandler,
                                                     uintptr_t                      context);

T_UsbHostDeviceObjectHandle USB_HostU3V_DeviceObjectHandleGet(T_UsbHostU3VObject u3vDeviceObj);  //to review return type

T_UsbHostU3VHandle USB_HostU3V_Open(T_UsbHostU3VObject u3vDeviceObj);

void USB_HostU3V_Close(T_UsbHostU3VHandle u3vDeviceHandle);

T_UsbHostU3VResult USB_HostU3V_EventHandlerSet(T_UsbHostU3VHandle       handle,
                                               T_UsbHostU3VEventHandler eventHandler,
                                               uintptr_t                context);

T_UsbHostU3VResult USB_HostU3V_Read(T_UsbHostU3VHandle          handle,
                                    T_UsbHostU3VTransferHandle *transferHandle,
                                    void                       *data,
                                    size_t                      size);

T_UsbHostU3VResult USB_HostU3V_Write(T_UsbHostU3VHandle          handle,
                                     T_UsbHostU3VTransferHandle *transferHandle,
                                     void                       *data,
                                     size_t                      size);

// T_UsbHostU3VResult USB_HostU3V_SerialStateNotificationGet(T_UsbHostU3VHandle        handle,
//                                                         T_UsbHostU3VTransferHandle *transferHandle,
//                                                         USB_CDC_SERIAL_STATE       *serialState);

// T_UsbHostU3VResult USB_HostU3V_ACM_LineCodingSet(T_UsbHostU3VHandle        handle,
//                                                 T_UsbHostU3VRequestHandle *requestHandle,
//                                                 USB_CDC_LINE_CODING       *lineCoding);

// T_UsbHostU3VResult USB_HostU3V_ACM_ControlLineStateSet(T_UsbHostU3VHandle         handle,
//                                                       T_UsbHostU3VRequestHandle  *requestHandle,
//                                                       USB_CDC_CONTROL_LINE_STATE *controlLineState);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_H
