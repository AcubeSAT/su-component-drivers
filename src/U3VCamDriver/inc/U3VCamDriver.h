//
// Created by fomarko on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_U3VCAMDRIVER_H
#define COMPONENT_DRIVERS_U3VCAMDRIVER_H




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
} T_U3VCamDriverDevDescrTextType;

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

T_U3VCamDriverStatus U3VCamDriver_AcquireNewImage(void *params); //TODO: maybe add callback with MEMR interfcace with data bfr + size args

T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDevDescrTextType textType, void *buffer); //buffer size must be 64bytes at least

T_U3VCamDriverStatus U3VCamDriver_GetDeviceTemperature(float *temperatureC); //temperatureC in celcius with 2 decimals resolution

T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAMDRIVER_H
