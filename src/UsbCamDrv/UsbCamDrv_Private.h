//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
#define COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H

#include "UsbCamDrv_Main.h"

#ifdef __cplusplus
extern "C" {
#endif


/*** Macro definitions ***/

#define USB_HOST_U3V_RESULT_MIN                        -100


/*** Type definitions ***/

typedef enum
{
    USB_APP_STATE_BUS_ENABLE,                          /* USB Application enabled the bus*/
    USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,        /* USB Application waits for bus to be enabled */
    USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH,              /* USB Application waits for CDC Device Attach */
    USB_APP_STATE_OPEN_DEVICE,                         /* USB CDC Device is Attached */
    USB_APP_STATE_SET_LINE_CODING,                     /* USB Set the Line Coding */
    USB_APP_STATE_WAIT_FOR_GET_LINE_CODING,            /* USB Application waits to get the device line coding */
    USB_APP_STATE_SEND_SET_LINE_CODING,                /* USB Application sets the line coding */
    USB_APP_STATE_WAIT_FOR_SET_LINE_CODING,            /* USB Appliction waits till set line coding is done */
    USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE,         /* USB Application sets the contol line state */
    USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE,     /* USB Application waits for the set control line state to complete */
    USB_APP_STATE_SEND_PROMPT_TO_DEVICE,               /* USB Application sends the prompt to the device */
    USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE,       /* USB Application waits for prompt send complete */
    USB_APP_STATE_GET_DATA_FROM_DEVICE,                /* USB Application request to get data from device */
    USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE,           /* USB Application waits for data from device */
    USB_APP_STATE_ERROR                                /* USB Application is in error state */
} USB_APP_STATES;


typedef USB_APP_STATES T_UsbCamAppStates;


typedef uintptr_t T_UsbHostU3VObject;


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
    uint8_t                     inDataArray[64];       /* First place to be aligned - Array to hold read data */
    USB_APP_STATES              state;                 /* The USB application's current state */
    USB_HOST_CDC_OBJ            cdcObj;                /* CDC Object */
    bool                        deviceIsAttached;      /* True if a device is attached */
    bool                        controlRequestDone;    /* True if control request is done */
    USB_HOST_CDC_RESULT         controlRequestResult;  /* Control Request Result */
    USB_CDC_LINE_CODING         cdcHostLineCoding;     /* A CDC Line Coding object */
    USB_CDC_CONTROL_LINE_STATE  controlLineState;      /* A Control Line State object*/
    USB_HOST_CDC_HANDLE         cdcHostHandle;         /* Handle to the CDC device. */
    USB_HOST_CDC_REQUEST_HANDLE requestHandle;         /* Handle to request */
    bool                        writeTransferDone;     /* True when a write transfer has complete */
    USB_HOST_CDC_RESULT         writeTransferResult;   /* Write Transfer Result */
    bool                        readTransferDone;      /* True when a read transfer has complete */
    USB_HOST_CDC_RESULT         readTransferResult;    /* Read Transfer Result */
    bool                        deviceWasDetached;     /* True if device was detached */
} USB_APP_DATA;

typedef struct
{
    uint8_t                     inDataArray[64];       /* First place to be aligned - Array to hold read data */
    T_UsbCamAppStates           state;                 /* The USB application's current state */
    T_UsbHostU3VObject          u3vObj;                /* USB3 Vision Object */
    bool                        deviceIsAttached;      /* True if a device is attached */
    bool                        controlRequestDone;    /* True if control request is done */
    T_UsbHostU3VResult          controlRequestResult;  /* Control Request Result */
    USB_CDC_LINE_CODING         cdcHostLineCoding;     /* A CDC Line Coding object */
    USB_CDC_CONTROL_LINE_STATE  controlLineState;      /* A Control Line State object*/
    USB_HOST_CDC_HANDLE         cdcHostHandle;         /* Handle to the CDC device. */
    USB_HOST_CDC_REQUEST_HANDLE requestHandle;         /* Handle to request */
    bool                        writeTransferDone;     /* True when a write transfer has complete */
    USB_HOST_CDC_RESULT         writeTransferResult;   /* Write Transfer Result */
    bool                        readTransferDone;      /* True when a read transfer has complete */
    USB_HOST_CDC_RESULT         readTransferResult;    /* Read Transfer Result */
    bool                        deviceWasDetached;     /* True if device was detached */
} T_U3VCamAppData;


typedef enum
{
    U3V_CAM_DISCONNECTED     = -1,
    U3V_CAM_STATUS_UNKNOWN   =  0,
    U3V_CAM_CONNECTED        =  1
} T_U3VCamConnectStatus;


typedef enum
{
    U3V_CAM_DEV_ID_ERROR     = 0,
    U3V_CAM_DEV_ID_OK        = 1
} T_U3VCamDevIDStatus;


typedef bool  U3VCamIDValid_t;


/*** Constant declarations ***/

/*** Variable declarations ***/

/*** Function declarations ***/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
