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

T_U3VCamDriverStatus U3VCamDriver_AcquireNewImage(void *params);

T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAMDRIVER_H
