#pragma once

#include "U3VCam_Host.h"
#include "U3VCamDriver.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Type definitions
*******************************************************************************/

/**
 * U3V Cam Driver initialization state.
 * 
 */
typedef enum
{
    U3V_DRV_INITIALIZATION_FAIL = -1,
    U3V_DRV_NOT_INITIALIZED     =  0,
    U3V_DRV_INITIALIZATION_OK   =  1
} T_U3VDriverInitStatus;

/**
 * U3V App state.
 * 
 * App states of internal state machine that handles U3V driver operations.
 */
typedef enum
{
    U3V_APP_STATE_BUS_ENABLE,
    U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,
    U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH,
    U3V_APP_STATE_OPEN_DEVICE,
    U3V_APP_STATE_SETUP_U3V_CONTROL_IF,
    U3V_APP_STATE_READ_DEVICE_TEXT_DESCR,
    U3V_APP_STATE_GET_STREAM_CAPABILITIES,
    U3V_APP_STATE_SETUP_IMG_PRESET,
    U3V_APP_STATE_SETUP_PIXEL_FORMAT,
    U3V_APP_STATE_SETUP_ACQUISITION_MODE,
    U3V_APP_STATE_SETUP_U3V_STREAM_IF,
    U3V_APP_STATE_GET_CAM_TEMPERATURE,
    U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION,
    U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE,
    U3V_APP_STATE_STOP_IMAGE_ACQ,
    U3V_APP_STATE_ERROR
} T_U3VAppState;

/**
 * U3V App image payload transfer state.
 * 
 */
typedef enum
{
    U3V_SI_IMG_TRANSF_STATE_IDLE,
    U3V_SI_IMG_TRANSF_STATE_START,
    U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE,
    U3V_SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE,
    U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE
} T_U3VImgPayldTransfState;

/**
 * U3V App device text descriptors struct.
 * 
 */
typedef struct
{
	uint8_t     vendorName[U3V_REG_MANUFACTURER_NAME_SIZE];
	uint8_t     modelName[U3V_REG_MODEL_NAME_SIZE];
	uint8_t     deviceVersion[U3V_REG_DEVICE_VERSION_SIZE];
	uint8_t     serialNumber[U3V_REG_SERIAL_NUMBER_SIZE];
} T_U3VAppDevTextDescr;

/**
 * U3V App image config preset load struct.
 * 
 */
typedef struct
{
    uint32_t                    regVal;
    T_U3VCamDriverImagePreset   reqstdPreset;
} T_U3VAppImagePresetLoad;

/**
 * U3V App data struct.
 * 
 * This struct holds all data handled by the U3V App localy.
 */
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
    T_U3VAppImagePresetLoad             imgPresetLoad;
    uint32_t                            pixelFormat;
    uint32_t                            payloadSize;
    uint32_t                            acquisitionMode;
    T_U3VImgPayldTransfState            appImgTransfState;
    uint32_t                            appImgBlockCounter;
    T_U3VCamDriverPayloadEventCallback  appImgEvtCbk;
    T_U3VCamDriverErrorCallback         appErrorCbk;
    void                                *appImgDataBfr;
} T_U3VAppData;

/**
 * U3V App driver error IDs.
 * 
 * These error IDs are used for error reporting by the local U3V App state 
 * machine, when the execution of a step fails.
 */
typedef enum
{
    U3V_DRV_ERR_UNKNOWN,
    U3V_DRV_ERR_SW_RESET_REQ_FAIL,
    U3V_DRV_ERR_BUS_ENABLE_FAIL,
    U3V_DRV_ERR_OPEN_DEVICE_FAIL,
    U3V_DRV_ERR_SETUP_CTRL_IF_FAIL,
    U3V_DRV_ERR_READ_TEXT_DESCR_FAIL,
    U3V_DRV_ERR_GET_STREAM_CPBL_FAIL,
    U3V_DRV_ERR_SET_IMG_PRESET_FAIL,
    U3V_DRV_ERR_SET_PIXEL_FORMAT_FAIL,
    U3V_DRV_ERR_SET_ACQ_MODE_FAIL,
    U3V_DRV_ERR_GET_PAYLD_SIZE_FAIL,
    U3V_DRV_ERR_SETUP_STREAM_IF_FAIL,
    U3V_DRV_ERR_GET_CAM_TEMP_FAIL,
    U3V_DRV_ERR_START_IMG_ACQ_FAIL,
    U3V_DRV_ERR_START_IMG_TRANSF_FAIL,
    U3V_DRV_ERR_IMG_TRANSF_STATE_FAIL,
    U3V_DRV_ERR_STOP_IMG_ACQ_FAIL
} T_U3VCamDriverErrorID;

/**
 * Sets up a callback to handle internal driver errors
 * @param callback callback that receives the error id
 * @return The driver status, which indicates failure if not U3V_CAM_DRV_OK
 */
T_U3VCamDriverStatus U3VCamDriver_SetErrorCallback(T_U3VCamDriverErrorCallback callback);


#ifdef __cplusplus
}
#endif //__cplusplus

