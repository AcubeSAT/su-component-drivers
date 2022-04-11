//
// Created by mojo on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "UsbCamDrv_Host_U3V.h"
#include "osal/osal.h"

#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_U3VControlChannelHandle; 		/* uintptr_t for U3VControlChannelObj */



/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

T_U3VHostResult U3VHost_CtrlCh_InterfaceCreate(T_U3VControlChannelHandle *pU3vCtrlCh, T_U3VHostObject u3vInstObj);

T_U3VHostResult U3VHost_CtrlCh_ReadMemory(T_U3VControlChannelHandle u3vCtrlCh,
										  T_U3VHostTransferHandle *transferHandle,
										  uint32_t memAddress,
										  size_t transfSize,
										  uint32_t *bytesRead,
										  void *buffer);

T_U3VHostResult U3VHost_CtrlCh_WriteMemory(T_U3VControlChannelHandle u3vCtrlCh,
										   T_U3VHostTransferHandle *transferHandle,
										   uint32_t memAddress,
										   size_t transfSize,
										   uint32_t *bytesWritten,
										   const void *buffer);

void U3VHost_CtrlCh_InterfaceDestroy(T_U3VControlChannelHandle *pU3vCtrlCh);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H
