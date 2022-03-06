//
// Created by mojo on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "UsbCamDrv_U3V_IF_SharedTypes.h"
#include "UsbCamDrv_DeviceClassSpec_U3V.h"
#include "UsbCamDrv_Host_U3V_Local.h"
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


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

int32_t U3VCtrlIf_IntfCreate(T_UsbHostU3VInstanceObj *u3v);

int32_t U3VCtrlIf_HandleMessage(T_UsbHostU3VInstanceObj *u3vDev, uint8_t request, uint8_t requesttype, uint16_t value, uint16_t index, void *usr_data, uint16_t size);

T_U3VHostResult U3VCtrlIf_ReadMemory(T_UsbHostU3VInstanceObj *u3vInstance,
									 T_U3VHostTransferHandle *transferHandle,
									 uint32_t memAddress,
									 size_t transfSize,
									 uint32_t *bytesRead,
									 void *buffer);

T_U3VHostResult U3VCtrlIf_WriteMemory(T_UsbHostU3VInstanceObj *u3vInstance,
									  T_U3VHostTransferHandle *transferHandle,
									  uint32_t memAddress,
									  size_t transfSize,
									  uint32_t *bytesWritten,
									  const void *buffer);

void U3VCtrlIf_IntfDestroy(T_UsbHostU3VInstanceObj *u3vDev);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_H
