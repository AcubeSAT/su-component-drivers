//
// Created by mojo on 8/12/21.
//

#include "UsbCamDrv_AppCallbacks.h"
#include "UsbCamDrv_Config.h"


/*** Local function declarations (static) ***/


/*** Constants & Variable declarations ***/


/*** Function definitions ***/

USB_HOST_EVENT_RESPONSE UsbApp_USBHostEventHandler(USB_HOST_EVENT event,                           \
                                                   void *pEventData,                               \
                                                   uintptr_t context)   
{
    /* This function is called by the USB Host whenever a USB Host Layer event has occurred. 
     * In this example we only handle the device unsupported event. */

    switch (event)
    {
        case USB_HOST_EVENT_DEVICE_UNSUPPORTED:
            /* The attached device is not supported for some reason */
            break;
            
        default:
            break;
    }
    return(USB_HOST_EVENT_RESPONSE_NONE);
}


void UsbApp_USBHostCDCAttachEventListener(USB_HOST_CDC_OBJ cdcObj, uintptr_t context)
{
    /* This function gets called when the CDC device is attached. Update the application data 
     * structure to let the application know that this device is attached. */
    USB_APP_DATA *pAppDataStr;
    pAppDataStr = (USB_APP_DATA*)context;

    pAppDataStr->deviceIsAttached = true;
    pAppDataStr->cdcObj = cdcObj;
}


USB_HOST_CDC_EVENT_RESPONSE UsbApp_USBHostCDCEventHandler(USB_HOST_CDC_HANDLE cdcHandle,    \
                                                          USB_HOST_CDC_EVENT event,         \
                                                          void *pEventData,                 \
                                                          uintptr_t context)
{
    /* This function is called when a CDC Host event has occurred. A pointer to this function is 
     * registered after opening the device. See the call to USB_HOST_CDC_EventHandlerSet(). */

    USB_HOST_CDC_EVENT_ACM_SET_LINE_CODING_COMPLETE_DATA * setLineCodingEventData;
    USB_HOST_CDC_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE_DATA * setControlLineStateEventData;
    USB_HOST_CDC_EVENT_WRITE_COMPLETE_DATA * writeCompleteEventData;
    USB_HOST_CDC_EVENT_READ_COMPLETE_DATA * readCompleteEventData;
    USB_APP_DATA *pAppDataStr;

    pAppDataStr = (USB_APP_DATA*)context;
    
    switch(event)
    {
        case USB_HOST_CDC_EVENT_ACM_SET_LINE_CODING_COMPLETE:        
            /* This means the application requested Set Line Coding request is complete. */
            setLineCodingEventData = (USB_HOST_CDC_EVENT_ACM_SET_LINE_CODING_COMPLETE_DATA *)(pEventData);
            pAppDataStr->controlRequestDone = true;
            pAppDataStr->controlRequestResult = setLineCodingEventData->result;
            break;
            
        case USB_HOST_CDC_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE:
            /* This means the application requested Set Control Line State request has completed. */
            setControlLineStateEventData = (USB_HOST_CDC_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE_DATA *)(pEventData);
            pAppDataStr->controlRequestDone = true;
            pAppDataStr->controlRequestResult = setControlLineStateEventData->result;
            break;
            
        case USB_HOST_CDC_EVENT_WRITE_COMPLETE:
            /* This means an application requested write has completed */
            pAppDataStr->writeTransferDone = true;
            writeCompleteEventData = (USB_HOST_CDC_EVENT_WRITE_COMPLETE_DATA *)(pEventData);
            pAppDataStr->writeTransferResult = writeCompleteEventData->result;
            break;
            
        case USB_HOST_CDC_EVENT_READ_COMPLETE:
            /* This means an application requested write has completed */
            pAppDataStr->readTransferDone = true;
            readCompleteEventData = (USB_HOST_CDC_EVENT_READ_COMPLETE_DATA *)(pEventData);
            pAppDataStr->readTransferResult = readCompleteEventData->result;
            break;
            
        case USB_HOST_CDC_EVENT_DEVICE_DETACHED:
            /* The device was detached */
            pAppDataStr->deviceWasDetached = true;
			/* Switch off LED  */
            LED1_Off(); // DEBUG XULT board
            break;
            
        default:
            break;
    }
    return(USB_HOST_CDC_EVENT_RESPONE_NONE);
}
