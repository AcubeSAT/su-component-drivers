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
#include "UsbCamDrv_U3V_Control_IF.h"

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
    U3V_DRV_INITIALIZATION_FAIL = -1,
    U3V_DRV_NOT_INITIALIZED     =  0,
    U3V_DRV_INITIALIZATION_OK   =  1
} T_U3VCamDriverInitStatus;


typedef enum
{
    USB_APP_STATE_BUS_ENABLE,                           /* USB Application enabled the bus*/
    USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,         /* USB Application waits for bus to be enabled */
    USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH,               /* USB Application waits for U3V Device Attach */
    USB_APP_STATE_OPEN_DEVICE,                          /* USB U3V Device is Attached */
    USB_APP_STATE_SETUP_U3V_CONTROL_CH,                 /* USB Application sets up the Control Channel */
    USB_APP_STATE_GET_U3V_MANIFEST,                     /* USB Application retrieves U3V Manifest file from device */
    USB_APP_STATE_GET_CAM_TEMPERATURE,                  /* USB Application get cam Temperature in Celcius */
    USB_APP_STATE_GET_STREAM_CAPABILITIES,              /* USB Application retrieves Stream Channel capabilities */
    USB_APP_STATE_SETUP_PIXEL_FORMAT,                   /* USB Application setup Pixel format */
    USB_APP_STATE_SETUP_ACQUISITION_MODE,               /* USB Application setup Acquisition mode */
    USB_APP_STATE_READY_TO_START_IMG_ACQUISITION,       /* USB Application is ready to receive image acquisition request */
    USB_APP_STATE_WAIT_TO_ACQUIRE_IMAGE,                /* USB Application is waiting to acquire image */

    USB_APP_STATE_UNSPECIFIED,  //todo: add missing states, this one is only for debug

    USB_APP_STATE_ERROR
} T_UsbU3vAppStates;    //in work


typedef struct
{
    T_UsbU3vAppStates           state;                 /* The USB application's current state */
    T_U3VHostObject             u3vObj;                /* U3V Object */
    T_U3VHostHandle             u3vHostHandle;         /* Handle to the U3V device. */
    T_U3VHostRequestHandle      requestHandle;         /* Handle to request */
    T_U3VControlChannelHandle   controlChHandle;       /* Handle to U3V Control Channel */
    bool                        readTransferDone;      /* True when a read transfer has complete */
    T_U3VHostResult             readTransferResult;    /* Read Transfer Result */
    bool                        writeTransferDone;     /* True when a write transfer has complete */
    T_U3VHostResult             writeTransferResult;   /* Write Transfer Result */
    bool                        deviceIsAttached;      /* True if a device is attached */
    bool                        deviceWasDetached;     /* True if device was detached */
    uint32_t                    pixelFormat;           /* Active Pixel Format value (PixelFormatCtrlVal_Int)*/
    float                       camTemperature;        /* Cam temperature in Celcius */
    T_U3VHostAcquisitionMode    acquisitionMode;       /* Acquisition mode */
    bool                        acquisitionRequested;  /* Image Acquisition Requested */
    bool                        camSwResetRequested;   /* Cam SW reset requested */
    
} T_UsbU3VAppData;      //in work


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

T_U3VCamDriverStatus UsbCamDrv_CamSwReset(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_APP_H
