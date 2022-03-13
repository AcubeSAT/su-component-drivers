//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H
#define COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H

#include "definitions.h"
#include "UsbCamDrv_Host_U3V.h"


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

USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context);

void _USBHostU3VAttachEventListenerCbk(T_U3VHostObject u3vObj, uintptr_t context);

void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context);

T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle,
                                                  T_U3VHostEvent event,
                                                  void *pEventData,
                                                  uintptr_t context);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H
