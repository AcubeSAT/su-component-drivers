//
// Created by mojo on 8/12/21.
//

#include "UsbCamDrv_Main.h"
#include "UsbCamDrv_Private.h"
#include "UsbCamDrv_AppCallbacks.h"
#include "UsbCamDrv_Config.h"


/*** Local function declarations (static) ***/

T_CamConnectionStatus UsbCamDrv_GetCamConnectionStatus(void);

T_CamUSBDevIDStatus UsbCamDrv_USBCamDeviceIDisValid(void);

static inline CamIDValid_t CamVIDisValid(void);

static inline CamIDValid_t CamPIDisValid(void);




/*** Constants & Variable declarations ***/

T_UsbCamDrvInitStatus UsbCamDrv_InitStatus = DRV_NOT_INITIALZD;

USB_APP_DATA USB_ALIGN UsbAppData;

USB_ALIGN uint8_t prompt[8]  = "\r\nLED : ";       /* This is the string that will written to the file */



/*** Function definitions ***/

void UsbCamDrv_Initialize(void)
{
    T_UsbCamDrvInitStatus DrvSts = DRV_INITIALZN_OK;

    /* DEBUG XULT - Disable VBUS power */
    VBUS_HOST_EN_PowerDisable();

    /* DEBUG XULT - Switch off LEDs */
    LED0_Off();
    LED1_Off();
    
    /* Initialize the USB application state machine elements */
    UsbAppData.state                            = USB_APP_STATE_BUS_ENABLE;
    UsbAppData.cdcHostLineCoding.dwDTERate      = APP_HOST_CDC_BAUDRATE_SUPPORTED;
    UsbAppData.cdcHostLineCoding.bDataBits      = (uint8_t)APP_HOST_CDC_NO_OF_DATA_BITS;
    UsbAppData.cdcHostLineCoding.bParityType    = (uint8_t)APP_HOST_CDC_PARITY_TYPE;
    UsbAppData.cdcHostLineCoding.bCharFormat    = (uint8_t)APP_HOST_CDC_STOP_BITS;
    UsbAppData.controlLineState.dtr             = 0u;
    UsbAppData.controlLineState.carrier         = 0u;
    UsbAppData.deviceIsAttached                 = false;
    UsbAppData.deviceWasDetached                = false;
    UsbAppData.readTransferDone                 = false;
    UsbAppData.writeTransferDone                = false;
    UsbAppData.controlRequestDone               = false;

    UsbCamDrv_InitStatus = DrvSts;
}


T_UsbCamDrvInitStatus UsbCamDrv_DrvInitStatus(void)
{
    return UsbCamDrv_InitStatus;
}


void UsbCamDrv_Tasks(void)
{
    //to do

    /* Check the application's current state. */
    USB_HOST_CDC_RESULT result;

    if (UsbAppData.deviceWasDetached)
    {
        /* This means the device is not attached. Reset the application state */
        UsbAppData.state                = USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
        UsbAppData.readTransferDone     = false;
        UsbAppData.writeTransferDone    = false;
        UsbAppData.controlRequestDone   = false;
        UsbAppData.deviceWasDetached    = false;
    }

    switch (UsbAppData.state)
    {
        case USB_APP_STATE_BUS_ENABLE:
        
            /* In this state the application enables the USB Host Bus. Note
             * how the CDC Attach event handler are registered before the bus
             * is enabled. */

            USB_HOST_EventHandlerSet(UsbApp_USBHostEventHandler, (uintptr_t)0);
            USB_HOST_CDC_AttachEventHandlerSet(UsbApp_USBHostCDCAttachEventListener, (uintptr_t)&UsbAppData);
            USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            UsbAppData.state = USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            break;
        
        case USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            
            /* In this state we wait for the Bus enable to complete */
            if(USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            
            /* In this state the application is waiting for the device to be
             * attached */
            if(UsbAppData.deviceIsAttached)
            {
                /* A device is attached. We can open this device */
                UsbAppData.state = USB_APP_STATE_OPEN_DEVICE;
                UsbAppData.deviceIsAttached = false;
            }
            break;
            
        case USB_APP_STATE_OPEN_DEVICE:
            
            /* In this state the application opens the attached device */
            UsbAppData.cdcHostHandle = USB_HOST_CDC_Open(UsbAppData.cdcObj);
            if(UsbAppData.cdcHostHandle != USB_HOST_CDC_HANDLE_INVALID)
            {
                /* The driver was opened successfully. Set the event handler
                 * and then go to the next state. */
                USB_HOST_CDC_EventHandlerSet(UsbAppData.cdcHostHandle, UsbApp_USBHostCDCEventHandler, (uintptr_t)&UsbAppData);
                UsbAppData.state = USB_APP_STATE_SET_LINE_CODING;
            }
            break;
            
        case USB_APP_STATE_SET_LINE_CODING:
            
            /* Here we set the Line coding. The control request done flag will
             * be set to true when the control request has completed. */
            
            UsbAppData.controlRequestDone = false;
            result = USB_HOST_CDC_ACM_LineCodingSet(UsbAppData.cdcHostHandle, NULL, &UsbAppData.cdcHostLineCoding);
            
            if(result == USB_HOST_CDC_RESULT_SUCCESS)
            {
                /* We wait for the set line coding to complete */
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_SET_LINE_CODING;
            }
                            
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_LINE_CODING:
            
            if(UsbAppData.controlRequestDone)
            {
                if(UsbAppData.controlRequestResult != USB_HOST_CDC_RESULT_SUCCESS)
                {
                    /* The control request was not successful. */
                    UsbAppData.state = USB_APP_STATE_ERROR;
                }
                else
                {
                    /* Next we set the Control Line State */
                    UsbAppData.state = USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE;
                }
            }
            break;
            
        case USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE:
            
            /* Here we set the control line state */
            UsbAppData.controlRequestDone = false;
            result = USB_HOST_CDC_ACM_ControlLineStateSet(UsbAppData.cdcHostHandle, NULL, 
                    &UsbAppData.controlLineState);
            
            if(result == USB_HOST_CDC_RESULT_SUCCESS)
            {
                /* We wait for the set line coding to complete */
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE;
            }
            
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE:
            
            /* Here we wait for the control line state set request to complete */
            if(UsbAppData.controlRequestDone)
            {
                if(UsbAppData.controlRequestResult != USB_HOST_CDC_RESULT_SUCCESS)
                {
                    /* The control request was not successful. */
                    UsbAppData.state = USB_APP_STATE_ERROR;
                }
                else
                {
                    /* Next we set the Control Line State */
                    UsbAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            
            break;
            
        case USB_APP_STATE_SEND_PROMPT_TO_DEVICE:
            
            /* The prompt is sent to the device here. The write transfer done
             * flag is updated in the event handler. */
            
            UsbAppData.writeTransferDone = false;
            result = USB_HOST_CDC_Write(UsbAppData.cdcHostHandle, NULL, ( void * )prompt, 8);
            
            if(result == USB_HOST_CDC_RESULT_SUCCESS)
            {
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE:
            
            /* Here we check if the write transfer is done */
            if(UsbAppData.writeTransferDone)
            {
                if(UsbAppData.writeTransferResult == USB_HOST_CDC_RESULT_SUCCESS)
                {
                    /* Now to get data from the device */
                    UsbAppData.state = USB_APP_STATE_GET_DATA_FROM_DEVICE;
                }
                else
                {
                    /* Try sending the prompt again. */
                    UsbAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            
            break;
            
        case USB_APP_STATE_GET_DATA_FROM_DEVICE:
            
            /* Here we request data from the device */
            UsbAppData.readTransferDone = false;
            result = USB_HOST_CDC_Read(UsbAppData.cdcHostHandle, NULL, UsbAppData.inDataArray, 8);
            if(result == USB_HOST_CDC_RESULT_SUCCESS)
            {
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE;
            }
            break;
           
        case USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE:
            
            /* Wait for data from device. If the data has arrived, then toggle
             * the LED. */
            if(UsbAppData.readTransferDone)
            {
                if(UsbAppData.readTransferResult == USB_HOST_CDC_RESULT_SUCCESS)
                {
                   if ( UsbAppData.inDataArray[0] == '1')
                   {
                       /* DEBUG XULT - Switch on LED  */
                       LED1_On();
                   }
                   else
                   {
                       /* DEBUG XULT - Switch off LED  */
                       LED1_Off();
                   }

                    /* Send the prompt to the device and wait
                     * for data again */
                    UsbAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            
        case USB_APP_STATE_ERROR:
            /* An error has occurred */
            // error handling...
            break;
            
        default:
            break;
    }

}


T_UsbCameraDriver_Status UsbCamDrv_AcquireNewImage(void *params)
{
    T_UsbCameraDriver_Status DrvSts = USB_CAM_DRV_OK;

    DrvSts = (UsbCamDrv_DrvInitStatus() == DRV_INITIALZN_OK)       ? DrvSts : USB_CAM_DRV_NOT_INITD;
    DrvSts = (UsbCamDrv_GetCamConnectionStatus() == CAM_CONNECTED) ? DrvSts : USB_CAM_DRV_ERROR;

    // to do

    return DrvSts;
}


T_CamConnectionStatus UsbCamDrv_GetCamConnectionStatus(void)
{
    T_CamConnectionStatus CamStatus = CAM_STATUS_UNKNOWN;

    // to do
    if (1)
    {
        CamStatus = CAM_CONNECTED;
    }
    // to do

    return CamStatus;
}


T_CamUSBDevIDStatus UsbCamDrv_USBCamDeviceIDisValid(void)
{
    T_CamUSBDevIDStatus CamUSBDevIDSts = CAM_DEV_ID_OK;

    CamUSBDevIDSts = (CamPIDisValid()) ? CamUSBDevIDSts : CAM_DEV_ID_ERROR;
    CamUSBDevIDSts = (CamVIDisValid()) ? CamUSBDevIDSts : CAM_DEV_ID_ERROR;

    return CamUSBDevIDSts;
}


static inline CamIDValid_t CamVIDisValid(void)
{
    // to do
    if (1)
    {
        return true;
    }
    return false;
}


static inline CamIDValid_t CamPIDisValid(void)
{
    // to do
    if (1)
    {
        return true;
    }
    return false;
}
