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

typedef uintptr_t T_U3VControlInterfHandle; 		/* uintptr_t for ControlInterfaceObj */



/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VControlInterfHandle *pU3vCtrlIf, T_U3VHostObject u3vInstObj);

T_U3VHostResult U3VHost_CtrlIf_ReadMemory(T_U3VControlInterfHandle u3vCtrlIf,
										  T_U3VHostTransferHandle *transferHandle,
										  uint32_t memAddress,
										  size_t transfSize,
										  uint32_t *bytesRead,
										  void *buffer);

T_U3VHostResult U3VHost_CtrlIf_WriteMemory(T_U3VControlInterfHandle u3vCtrlIf,
										   T_U3VHostTransferHandle *transferHandle,
										   uint32_t memAddress,
										   size_t transfSize,
										   uint32_t *bytesWritten,
										   const void *buffer);

void U3VHost_CtrlIf_InterfaceDestroy(T_U3VControlInterfHandle *pU3vCtrlIf);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H
