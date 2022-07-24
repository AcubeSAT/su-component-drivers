//
// Created by fomarko on 02/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_APP_H
#define COMPONENT_DRIVERS_U3VCAM_APP_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
// #include "definitions.h"
#include "U3VCam_Host.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/


/********************************************************
* Type definitions
*********************************************************/

// typedef enum
// {
//     U3V_CAM_DRV_NOT_INITD   = -2,
//     U3V_CAM_DRV_ERROR       = -1,
//     U3V_CAM_DRV_OK          =  0
// } T_U3VCamDriverStatus;          //TODO: remove, this is included in ext header

typedef enum
{
    U3V_DRV_INITIALIZATION_FAIL = -1,
    U3V_DRV_NOT_INITIALIZED     =  0,
    U3V_DRV_INITIALIZATION_OK   =  1
} T_U3VDriverInitStatus;

typedef enum
{
    U3V_APP_STATE_BUS_ENABLE,                     /* U3V Application enabled the bus*/
    U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,   /* U3V Application waits for bus to be enabled */
    U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH,         /* U3V Application waits for U3V Device Attach */
    U3V_APP_STATE_OPEN_DEVICE,                    /* U3V Device is Attached */
    U3V_APP_STATE_SETUP_U3V_CONTROL_IF,           /* U3V Application sets up the Control Interface */
    U3V_APP_STATE_READ_DEVICE_TEXT_DESCR,         /* U3V Application retrieves Stream Interface capabilities */
    U3V_APP_STATE_GET_STREAM_CAPABILITIES,        /* U3V Application retrieves Stream Interface capabilities */
    U3V_APP_STATE_SETUP_PIXEL_FORMAT,             /* U3V Application setup Pixel format */
    U3V_APP_STATE_SETUP_ACQUISITION_MODE,         /* U3V Application setup Acquisition mode */
    U3V_APP_STATE_SETUP_U3V_STREAM_IF,            /* U3V Application setup Stream Interface */
    U3V_APP_STATE_GET_CAM_TEMPERATURE,            /* U3V Application get cam Temperature in Celcius */
    U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION, /* U3V Application is ready to receive image acquisition request */
    U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE,          /* U3V Application is waiting to acquire image */
    U3V_APP_STATE_STOP_IMAGE_ACQ,                 /* U3V Applicatiion request to stop image acquisition*/
    U3V_APP_STATE_ERROR,                          /* U3V Application error state */

    U3V_APP_STATE_NOP // TODO: remove, debug state
} T_U3VAppState;

typedef struct
{
	// uint32_t    genCpVersion;                                       //TODO: remove if never used
	// uint32_t    u3vVersion;                                         //TODO: remove if never used
	// char        deviceGuid[U3V_MAX_DESCR_STR_LENGTH];               //TODO: remove if never used
	char        vendorName[U3V_REG_MANUFACTURER_NAME_SIZE];
	char        modelName[U3V_REG_MODEL_NAME_SIZE];
	// char        familyName[U3V_REG_FAMILY_NAME_SIZE];               //TODO: remove if never used
	char        deviceVersion[U3V_REG_DEVICE_VERSION_SIZE];         //TODO: remove if never used
	// char        manufacturerInfo[U3V_REG_MANUFACTURER_INFO_SIZE];   //TODO: remove if never used
	char        serialNumber[U3V_REG_SERIAL_NUMBER_SIZE];
	// char        userDefinedName[U3V_REG_USER_DEFINED_NAME_SIZE];    //TODO: remove if never used
} T_U3VAppDevTextDescr;

typedef struct
{
    T_U3VAppState               state;                 /* The U3V application's current state */
    T_U3VHostHandle             u3vHostHandle;         /* Handle to the U3V device. */
    bool                        deviceIsAttached;      /* True if a device is attached */
    bool                        deviceWasDetached;     /* True if device was detached */
    float                       camTemperature;        /* Cam temperature in Celcius */
    uint32_t                    pixelFormat;           /* Active Pixel Format value (PixelFormatCtrlVal_Int)*/
    uint32_t                    payloadSize;           /* Image payload size */
    uint32_t                    acquisitionMode;       /* Acquisition mode */
    bool                        acquisitionRequested;  /* Image Acquisition Requested */
    bool                        camSwResetRequested;   /* Cam SW reset requested */
    T_U3VAppDevTextDescr        camTextDescr;          /* Cam Text descriptors (SN, manufacturer, model, version) */
    T_U3VImgPayloadContainer    imgPayloadContainer;
} T_U3VAppData;

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

// void U3VCamDriver_Initialize(void);
// void U3VCamDriver_Tasks(void); 
// T_U3VCamDriverStatus U3VCamDriver_AcquireNewImage(void *params);
// T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_APP_H
