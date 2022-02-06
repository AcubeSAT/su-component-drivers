//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
#define COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H


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
    U3V_CAM_DISCONNECTED     = -1,
    U3V_CAM_STATUS_UNKNOWN   =  0,
    U3V_CAM_CONNECTED        =  1
} T_U3VCamConnectStatus;


typedef enum
{
    U3V_CAM_DEV_ID_ERROR     = 0,
    U3V_CAM_DEV_ID_OK        = 1
} T_U3VCamDevIDStatus;


typedef bool  T_U3VCamIDValid;


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

#endif //COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
