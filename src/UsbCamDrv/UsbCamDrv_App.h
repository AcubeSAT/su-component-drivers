//
// Created by mojo on 02/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_APP_H
#define COMPONENT_DRIVERS_USBCAMDRV_APP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"
#include "UsbCamDrv_Host_U3V.h"
#include "UsbCamDrv/UsbCamDrv_DeviceClassSpec_U3V.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
* Macro definitions
*********************************************************/


/********************************************************
* Type definitions
*********************************************************/

typedef enum
{
    U3V_CAM_DRV_NOT_INITD   = -2,
    U3V_CAM_DRV_ERROR       = -1,
    U3V_CAM_DRV_OK          =  0
} T_U3VCamDriverStatus;


typedef enum
{
    U3V_DRV_INITIALZN_FAIL   = -1,
    U3V_DRV_NOT_INITIALZD    =  0,
    U3V_DRV_INITIALZN_OK     =  1
} T_U3VCamDriverInitStatus;


typedef enum        //to do: rework
{
    USB_APP_STATE_BUS_ENABLE,                          /* USB Application enabled the bus*/
    USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,        /* USB Application waits for bus to be enabled */
    USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH,              /* USB Application waits for U3V Device Attach */
    USB_APP_STATE_OPEN_DEVICE,                         /* USB U3V Device is Attached */
    
    USB_APP_STATE_SET_LINE_CODING,                     /* USB Set the Line Coding */
    USB_APP_STATE_WAIT_FOR_GET_LINE_CODING,            /* USB Application waits to get the device line coding */
    USB_APP_STATE_SEND_SET_LINE_CODING,                /* USB Application sets the line coding */
    USB_APP_STATE_WAIT_FOR_SET_LINE_CODING,            /* USB Application waits till set line coding is done */
    USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE,         /* USB Application sets the control line state */
    USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE,     /* USB Application waits for the set control line state to complete */
    USB_APP_STATE_SEND_PROMPT_TO_DEVICE,               /* USB Application sends the prompt to the device */
    USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE,       /* USB Application waits for prompt send complete */
    USB_APP_STATE_GET_DATA_FROM_DEVICE,                /* USB Application request to get data from device */
    USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE,           /* USB Application waits for data from device */
    USB_APP_STATE_ERROR                                /* USB Application is in error state */
} T_UsbAppStates;


typedef struct
{
    uint8_t                     inDataArray[64];       /* First place to be aligned - Array to hold read data */
    T_UsbAppStates              state;                 /* The USB application's current state */
    T_U3VHostObject             u3vObj;                /* U3V Object */
    bool                        deviceIsAttached;      /* True if a device is attached */
    bool                        controlRequestDone;    /* True if control request is done */
    T_U3VHostResult             controlRequestResult;  /* Control Request Result */
    // USB_CDC_LINE_CODING         cdcHostLineCoding;     /* A CDC Line Coding object */
    // USB_CDC_CONTROL_LINE_STATE  controlLineState;      /* A Control Line State object*/
    T_U3VHostHandle             u3vHostHandle;         /* Handle to the U3V device. */
    T_U3VHostRequestHandle      requestHandle;         /* Handle to request */
    bool                        writeTransferDone;     /* True when a write transfer has complete */
    T_U3VHostResult             writeTransferResult;   /* Write Transfer Result */
    bool                        readTransferDone;      /* True when a read transfer has complete */
    T_U3VHostResult             readTransferResult;    /* Read Transfer Result */
    bool                        deviceWasDetached;     /* True if device was detached */
} T_UsbAppData;     //todo rework


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


typedef bool T_U3VCamIDValid;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

void UsbCamDrv_Initialize(void);

void UsbCamDrv_Tasks(void); 

T_U3VCamDriverStatus UsbCamDrv_AcquireNewImage(void *params);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_APP_H
