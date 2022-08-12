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
#include "U3VCam_Host.h"
#include "U3VCamDriver.h"


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
    U3V_DRV_INITIALIZATION_FAIL = -1,
    U3V_DRV_NOT_INITIALIZED     =  0,
    U3V_DRV_INITIALIZATION_OK   =  1
} T_U3VDriverInitStatus;

typedef enum
{
    U3V_APP_STATE_BUS_ENABLE,
    U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,
    U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH,
    U3V_APP_STATE_OPEN_DEVICE,
    U3V_APP_STATE_SETUP_U3V_CONTROL_IF,
    U3V_APP_STATE_READ_DEVICE_TEXT_DESCR,
    U3V_APP_STATE_GET_STREAM_CAPABILITIES,
    U3V_APP_STATE_SETUP_PIXEL_FORMAT,
    U3V_APP_STATE_SETUP_ACQUISITION_MODE,
    U3V_APP_STATE_SETUP_U3V_STREAM_IF,
    U3V_APP_STATE_GET_CAM_TEMPERATURE,
    U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION,
    U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE,
    U3V_APP_STATE_STOP_IMAGE_ACQ,
    U3V_APP_STATE_ERROR
} T_U3VAppState;

typedef enum
{
    U3V_SI_IMG_TRANSF_STATE_IDLE,
    U3V_SI_IMG_TRANSF_STATE_START,
    U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE,
    U3V_SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE,
    U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE
} T_U3VImgPayldTransfState;

typedef struct
{
	// uint32_t    genCpVersion;                                       //TODO: remove if never used
	// uint32_t    u3vVersion;                                         //TODO: remove if never used
	// char        deviceGuid[U3V_MAX_DESCR_STR_LENGTH];               //TODO: remove if never used
	char        vendorName[U3V_REG_MANUFACTURER_NAME_SIZE];
	char        modelName[U3V_REG_MODEL_NAME_SIZE];
	// char        familyName[U3V_REG_FAMILY_NAME_SIZE];               //TODO: remove if never used
	char        deviceVersion[U3V_REG_DEVICE_VERSION_SIZE];            //TODO: remove if never used
	// char        manufacturerInfo[U3V_REG_MANUFACTURER_INFO_SIZE];   //TODO: remove if never used
	char        serialNumber[U3V_REG_SERIAL_NUMBER_SIZE];
	// char        userDefinedName[U3V_REG_USER_DEFINED_NAME_SIZE];    //TODO: remove if never used
} T_U3VAppDevTextDescr;

typedef struct
{
    T_U3VAppState                       state;
    T_U3VHostHandle                     u3vHostHandle;
    bool                                deviceIsAttached;
    bool                                deviceWasDetached;
    bool                                imgAckRequested;
    bool                                imgAckReqNewBlock;
    bool                                camSwResetRequested;
    T_U3VAppDevTextDescr                camTextDescriptions;
    float                               camTemperature;
    uint32_t                            pixelFormat;
    uint32_t                            payloadSize;
    uint32_t                            acquisitionMode;
    T_U3VStreamIfConfig                 streamConfig;
    T_U3VImgPayldTransfState            appImgTransfState;
    uint32_t                            appImgBlockCounter;
    T_U3VCamDriverPayloadEventCallback  appImgEvtCbk;
    void                                *appImgDataBfr;
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




#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_APP_H
