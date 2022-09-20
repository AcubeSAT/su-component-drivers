
#ifndef COMPONENT_DRIVERS_U3VCAM_APP_H
#define COMPONENT_DRIVERS_U3VCAM_APP_H


#include "U3VCam_Host.h"
#include "U3VCamDriver.h"


#ifdef __cplusplus
extern "C" {
#endif



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
	uint8_t     vendorName[U3V_REG_MANUFACTURER_NAME_SIZE];
	uint8_t     modelName[U3V_REG_MODEL_NAME_SIZE];
	uint8_t     deviceVersion[U3V_REG_DEVICE_VERSION_SIZE];
	uint8_t     serialNumber[U3V_REG_SERIAL_NUMBER_SIZE];
} T_U3VAppDevTextDescr;

typedef struct
{
    T_U3VAppState                       state;
    T_U3VHostHandle                     u3vHostHandle;
    bool                                deviceIsAttached;
    bool                                deviceWasDetached;
    bool                                imgAcqRequested;
    bool                                imgAcqReqNewBlock;
    bool                                camSwResetRequested;
    T_U3VAppDevTextDescr                camTextDescriptions;
    float                               camTemperature;
    uint32_t                            pixelFormat;
    uint32_t                            payloadSize;
    uint32_t                            acquisitionMode;
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



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_APP_H
