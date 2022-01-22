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



/*** Type definitions ***/

/*** Application States ***
 *
 * Summary:
 *  USB Application states enumeration
 *
 * Description:
 *   This enumeration defines the valid application states.  These states determine the behavior of
 *   the USB application at various times.
 */

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


typedef enum
{
    CAM_DISCONNECTED     = -1,
    CAM_STATUS_UNKNOWN   =  0,
    CAM_CONNECTED        =  1
} T_CamConnectionStatus;


typedef enum
{
    CAM_DEV_ID_ERROR     = 0,
    CAM_DEV_ID_OK        = 1
} T_CamUSBDevIDStatus;


typedef bool  CamIDValid_t;



/*** Constant declarations ***/



/*** Variable declarations ***/



/*** Function declarations ***/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
