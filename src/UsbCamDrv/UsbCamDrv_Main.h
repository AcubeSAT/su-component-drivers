//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_MAIN_H
#define COMPONENT_DRIVERS_USBCAMDRV_MAIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"

#ifdef __cplusplus
extern "C" {
#endif


/*** Macro definitions ***/



/*** Type definitions ***/

typedef enum
{
    USB_CAM_DRV_NOT_INITD   = -2,
    USB_CAM_DRV_ERROR       = -1,
    USB_CAM_DRV_OK          =  0
} T_UsbCameraDriver_Status;


typedef enum
{
    DRV_INITIALZN_FAIL   = -1,
    DRV_NOT_INITIALZD    =  0,
    DRV_INITIALZN_OK     =  1
} T_UsbCamDrvInitStatus;



/*** Constant declarations ***/



/*** Variable declarations ***/



/*** Function declarations ***/

void UsbCamDrv_Initialize(void);

T_UsbCamDrvInitStatus UsbCamDrv_DrvInitStatus(void);

void UsbCamDrv_Tasks(void); 

T_UsbCameraDriver_Status UsbCamDrv_AcquireNewImage(void *params);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_MAIN_H
