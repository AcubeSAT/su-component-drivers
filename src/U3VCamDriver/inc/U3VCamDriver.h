//
// Created by fomarko on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_U3VCAMDRIVER_H
#define COMPONENT_DRIVERS_U3VCAMDRIVER_H


#include <stdint.h>


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
    U3V_CAM_DRV_GET_TEXT_SERIAL_NUMBER,
    U3V_CAM_DRV_GET_TEXT_MANUFACTURER_NAME,
    U3V_CAM_DRV_GET_TEXT_MODEL_NAME,
    U3V_CAM_DRV_GET_TEXT_DEVICE_VERSION,
} T_U3VCamDriverDeviceDescriptorTextType;

typedef enum
{
    U3V_CAM_DRV_IMG_LEADER_DATA = 1,
    U3V_CAM_DRV_IMG_PAYLOAD_DATA,
    U3V_CAM_DRV_IMG_TRAILER_DATA,
} T_U3VCamDriverImageAcqPayloadEvent;

typedef void (*T_U3VCamDriverPayloadEventCallback) (T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, size_t blockSize, uint32_t blockCnt);

/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

void U3VCamDriver_Initialize(void);

void U3VCamDriver_Tasks(void);

T_U3VCamDriverStatus U3VCamDriver_SetImagePayldTransfParams(T_U3VCamDriverPayloadEventCallback callback, void *imgData); //buffer size must be U3V_IN_BUFFER_MAX_SIZE at least, you can use U3VCamDriver_GetImagePayldMaxBlockSize()

T_U3VCamDriverStatus U3VCamDriver_RequestNewImagePayloadBlock(void);

void U3VCamDriver_CancelImageAcqRequest(void);

T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDeviceDescriptorTextType textType, void *buffer); //buffer size must be 64bytes at least

T_U3VCamDriverStatus U3VCamDriver_GetDeviceTemperature(float *temperatureC); //temperature in celcius with 2 decimals resolution

T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void);

size_t U3VCamDriver_GetImagePayldMaxBlockSize(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAMDRIVER_H
