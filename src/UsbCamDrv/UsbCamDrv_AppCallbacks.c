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
                                                  uintptr_t context)
{
    T_U3VHostEventReadCompleteData  *readCompleteEventData;
    T_UsbU3VAppData                 *pUsbU3VAppData;
    T_U3VSiGenericPacket            *pckLeaderOrTrailer;
    T_U3VHostResult                 result;
    size_t                          length;

    pUsbU3VAppData = (T_UsbU3VAppData*)context;
    readCompleteEventData = (T_U3VHostEventReadCompleteData *)(pEventData);

    switch (event)
    {
        case U3V_HOST_EVENT_IMG_PLD_RECEIVED:
            pckLeaderOrTrailer = (T_U3VSiGenericPacket*)pUsbU3VAppData->imgPayloadContainer.imgPldBfr1;
            result = readCompleteEventData->result; //TODO: remove
            length = readCompleteEventData->length; //TODO: remove

            if (pckLeaderOrTrailer->magicKey == U3V_LEADER_MGK_PREFIX)
            {
                /* Img Leader packet received */
                pUsbU3VAppData->imgPayloadContainer.imgPldTransfSt = SI_IMG_TRANSF_STATE_LEADER_COMPLETE;
            }
            else if (pckLeaderOrTrailer->magicKey == U3V_TRAILER_MGK_PREFIX)
            {
                /* Img Trailer packet received, end of transfer */
                pUsbU3VAppData->imgPayloadContainer.imgPldTransfSt = SI_IMG_TRANSF_STATE_TRAILER_COMPLETE;
            }
            else
            {
                /* Img Payload block with Image data */
                //TODO: block counter, recall & initiate DMA transf
            }

        case U3V_HOST_EVENT_WRITE_COMPLETE:
        case U3V_HOST_EVENT_READ_COMPLETE:
        default:
            break;
    }
    return U3V_HOST_EVENT_RESPONE_NONE;
}
