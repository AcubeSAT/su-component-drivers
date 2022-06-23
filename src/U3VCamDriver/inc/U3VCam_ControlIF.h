//
// Created by fomarko on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_CONTROLIF_H
#define COMPONENT_DRIVERS_U3VCAM_CONTROLIF_H


#include "U3VCam_Host.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_U3VControlIfHandle; 		/* uintptr_t for U3VControlIfObj */



/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VControlIfHandle *pU3vCtrlIf, T_U3VHostObject u3vInstObj);

T_U3VHostResult U3VHost_CtrlIf_ReadMemory(T_U3VControlIfHandle u3vCtrlIf,
										  T_U3VHostTransferHandle *transferHandle,
										  uint64_t memAddress,
										  size_t transfSize,
										  uint32_t *bytesRead,
										  void *buffer);

T_U3VHostResult U3VHost_CtrlIf_WriteMemory(T_U3VControlIfHandle u3vCtrlIf,
										   T_U3VHostTransferHandle *transferHandle,
										   uint64_t memAddress,
										   size_t transfSize,
										   uint32_t *bytesWritten,
										   const void *buffer);

void U3VHost_CtrlIf_InterfaceDestroy(T_U3VControlIfHandle *pU3vCtrlIf);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONTROLIF_H
