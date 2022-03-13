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
    return USB_HOST_EVENT_RESPONSE_NONE;
}


void _USBHostU3VAttachEventListenerCbk(T_U3VHostObject u3vObj, uintptr_t context)
{
    /* This function gets called when the U3V device is attached. Update the application data 
     * structure to let the application know that this device is attached. */
    T_UsbU3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_UsbU3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vObj = u3vObj;
}


void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context)
{
    /* This function gets called when the U3V device is detached. Update the application data 
     * structure to let the application know that this device is detached. */

    T_UsbU3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_UsbU3VAppData*)context;

    pUsbU3VAppData->deviceWasDetached = true;
}


T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle,
                                                  T_U3VHostEvent event,
                                                  void *pEventData,
                                                  uintptr_t context) // todo rework
{
    /* This function is called when a U3V Host event has occurred. A pointer to this function is 
     * registered after opening the device. See the call to USB_U3VHost_EventHandlerSet(). */

    T_U3VHostEventWriteCompleteData    *writeCompleteEventData;
    T_U3VHostEventReadCompleteData     *readCompleteEventData;
    T_UsbU3VAppData                    *pUsbU3VAppData;

    pUsbU3VAppData = (T_UsbU3VAppData*)context;

    switch (event)
    {
        case U3V_HOST_EVENT_WRITE_COMPLETE:
            /* This means an application requested write has completed */
            pUsbU3VAppData->writeTransferDone = true;
            writeCompleteEventData = (T_U3VHostEventWriteCompleteData *)(pEventData);
            pUsbU3VAppData->writeTransferResult = writeCompleteEventData->result;
            break;
            
        case U3V_HOST_EVENT_READ_COMPLETE:
            /* This means an application requested write has completed */
            pUsbU3VAppData->readTransferDone = true;
            readCompleteEventData = (T_U3VHostEventReadCompleteData *)(pEventData);
            pUsbU3VAppData->readTransferResult = readCompleteEventData->result;
            break;
            
        default:
            break;
    }
    return U3V_HOST_EVENT_RESPONE_NONE;
}
