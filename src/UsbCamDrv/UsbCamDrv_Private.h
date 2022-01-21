//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
#define COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H

#include "UsbCamDrv_Main.h"

#ifdef __cplusplus
extern "C" {
#endif


/*** Macro definitions ***/



/*** Type definitions ***/

typedef enum
{
    CAM_DISCONNECTED     = -1,
    CAM_STATUS_UNKNOWN   =  0,
    CAM_CONNECTED        =  1
} T_CamConnectionStatus;

typedef enum
{
    CAM_DEV_ID_ERROR     = 0,
    CAM_DEV_ID_OK        = 1
} T_CamUSBDevIDStatus;



typedef bool  CamIDValid_t;

/*** Constant declarations ***/



/*** Variable declarations ***/



/*** Function declarations ***/

T_CamConnectionStatus UsbCamDrv_GetCamConnectionStatus(void);
T_CamUSBDevIDStatus UsbCamDrv_USBCamDeviceIDisValid(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_PRIVATE_H
