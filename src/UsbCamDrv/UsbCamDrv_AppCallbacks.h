//
// Created by mojo on 8/12/21.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H
#define COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H

// #include "usb_host.h"
#include "usb/usb_host_cdc.h"
#include "definitions.h"
#include "UsbCamDrv_Private.h"


#ifdef __cplusplus
extern "C" {
#endif


/*** Macro definitions ***/


/*** Type definitions ***/


/*** Constant declarations ***/


/*** Variable declarations ***/


/*** Function declarations ***/

USB_HOST_EVENT_RESPONSE UsbApp_USBHostEventHandler(USB_HOST_EVENT event,                    \
                                                   void *pEventData,                        \
                                                   uintptr_t context);                      \


void UsbApp_USBHostCDCAttachEventListener(USB_HOST_CDC_OBJ cdcObj, uintptr_t context);



USB_HOST_CDC_EVENT_RESPONSE UsbApp_USBHostCDCEventHandler(USB_HOST_CDC_HANDLE cdcHandle,    \
                                                          USB_HOST_CDC_EVENT event,         \
                                                          void *pEventData,                 \
                                                          uintptr_t context);


// void UsbApp_HostGenicammAttachEventListener(USB_HOST_GENICAM_OBJ gcamObj, uintptr_t context);

// USB_HOST_GENICAM_EVENT_RESPONSE 
// UsbApp_USBHostGenicamEventHandler(USB_HOST_GENICAM_HANDLE gcamHandle,
                                //   USB_HOST_GENICAM_EVENT event,
                                //   void *pEventData,
                                //   uintptr_t);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_APPCALLBACKS_H
