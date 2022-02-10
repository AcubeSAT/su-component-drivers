//
// Created by mojo on 8/12/21.
//

#include "UsbCamDrv_AppCallbacks.h"
#include "UsbCamDrv_Config.h"



/********************************************************
* Local function declarations
*********************************************************/


/********************************************************
* Constant & Variable declarations
*********************************************************/


/********************************************************
* Function definitions
*********************************************************/

USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context)
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
    return (USB_HOST_EVENT_RESPONSE_NONE);
}


void _USBHostU3VAttachEventListenerCbk(T_UsbHostU3VObject u3vObj, uintptr_t context)
{
    /* This function gets called when the U3V device is attached. Update the application data 
     * structure to let the application know that this device is attached. */
    T_UsbAppData *pUsbAppData;
    pUsbAppData = (T_UsbAppData*)context;

    pUsbAppData->deviceIsAttached = true;
    pUsbAppData->u3vObj = u3vObj;
}


T_UsbHostU3VEventResponse _USBHostU3VEventHandlerCbk(T_UsbHostU3VHandle  u3vHandle,
                                                     T_UsbHostU3VEvent   event,
                                                     void               *pEventData,
                                                     uintptr_t           context)         //todo rework
{
    /* This function is called when a U3V Host event has occurred. A pointer to this function is 
     * registered after opening the device. See the call to USB_HostU3V_EventHandlerSet(). */

    T_USB_HOST_U3V_EVENT_ACM_SET_LINE_CODING_COMPLETE_DATA        *setLineCodingEventData;
    T_USB_HOST_U3V_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE_DATA *setControlLineStateEventData;
    T_USB_HOST_U3V_EVENT_WRITE_COMPLETE_DATA                      *writeCompleteEventData;
    T_USB_HOST_U3V_EVENT_READ_COMPLETE_DATA                       *readCompleteEventData;
    T_UsbAppData                                                  *pUsbAppData;

    pUsbAppData = (T_UsbAppData*)context;

    switch (event)
    {
        case USB_HOST_U3V_EVENT_ACM_SET_LINE_CODING_COMPLETE:        
            /* This means the application requested Set Line Coding request is complete. */
            setLineCodingEventData = (T_USB_HOST_U3V_EVENT_ACM_SET_LINE_CODING_COMPLETE_DATA *)(pEventData);
            pUsbAppData->controlRequestDone = true;
            pUsbAppData->controlRequestResult = setLineCodingEventData->result;
            break;
            
        case USB_HOST_U3V_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE:
            /* This means the application requested Set Control Line State request has completed. */
            setControlLineStateEventData = (T_USB_HOST_U3V_EVENT_ACM_SET_CONTROL_LINE_STATE_COMPLETE_DATA *)(pEventData);
            pUsbAppData->controlRequestDone = true;
            pUsbAppData->controlRequestResult = setControlLineStateEventData->result;
            break;
            
        case USB_HOST_U3V_EVENT_WRITE_COMPLETE:
            /* This means an application requested write has completed */
            pUsbAppData->writeTransferDone = true;
            writeCompleteEventData = (T_USB_HOST_U3V_EVENT_WRITE_COMPLETE_DATA *)(pEventData);
            pUsbAppData->writeTransferResult = writeCompleteEventData->result;
            break;
            
        case USB_HOST_U3V_EVENT_READ_COMPLETE:
            /* This means an application requested write has completed */
            pUsbAppData->readTransferDone = true;
            readCompleteEventData = (T_USB_HOST_U3V_EVENT_READ_COMPLETE_DATA *)(pEventData);
            pUsbAppData->readTransferResult = readCompleteEventData->result;
            break;
            
        case USB_HOST_U3V_EVENT_DEVICE_DETACHED:
            /* The device was detached */
            pUsbAppData->deviceWasDetached = true;
			/* Switch off LED  */
            LED1_Off(); // DEBUG XULT board
            break;
            
        default:
            break;
    }
    return (USB_HOST_U3V_EVENT_RESPONE_NONE);
}
