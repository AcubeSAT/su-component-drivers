//
// Created by mojo on 02/02/22.
//

#include "UsbCamDrv_App.h"
#include "UsbCamDrv_AppCallbacks.h"
#include "UsbCamDrv_Host_U3V.h"
#include "UsbCamDrv_Config.h"
#include "UsbCamDrv_U3V_Control_IF.h"



/********************************************************
* Local function declarations
*********************************************************/

T_U3VCamDriverInitStatus UsbCamDrv_DrvInitStatus(void);

T_U3VCamConnectStatus UsbCamDrv_GetCamConnectionStatus(void);

T_U3VCamDevIDStatus UsbCamDrv_CamDeviceIDisValid(void);

static inline T_U3VCamIDValid CamVIDisValid(void);

static inline T_U3VCamIDValid CamPIDisValid(void);



/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VCamDriverInitStatus UsbCamDrv_InitStatus = U3V_DRV_NOT_INITIALZD;

T_UsbAppData USB_ALIGN UsbAppData;

USB_ALIGN uint8_t prompt[8]  = "\r\nLED : ";       /* debug / to remove */




/********************************************************
* Function definitions
*********************************************************/

void UsbCamDrv_Initialize(void)
{
    T_U3VCamDriverInitStatus DrvSts = U3V_DRV_INITIALZN_OK;

    /* DEBUG XULT - Disable VBUS power */
    VBUS_HOST_EN_PowerDisable();

    /* Switch off LEDs */
    LED0_Off(); // DEBUG XULT board
    LED1_Off(); // DEBUG XULT board

    /* Initialize the USB application state machine elements */
    UsbAppData.state                            = USB_APP_STATE_BUS_ENABLE;
    // UsbAppData.cdcHostLineCoding.dwDTERate      = APP_HOST_CDC_BAUDRATE_SUPPORTED;       //todo rework
    // UsbAppData.cdcHostLineCoding.bDataBits      = (uint8_t)APP_HOST_CDC_NO_OF_DATA_BITS;
    // UsbAppData.cdcHostLineCoding.bParityType    = (uint8_t)APP_HOST_CDC_PARITY_TYPE;
    // UsbAppData.cdcHostLineCoding.bCharFormat    = (uint8_t)APP_HOST_CDC_STOP_BITS;
    // UsbAppData.controlLineState.dtr             = 0u;
    // UsbAppData.controlLineState.carrier         = 0u;
    UsbAppData.deviceIsAttached                 = false;
    UsbAppData.deviceWasDetached                = false;
    UsbAppData.readTransferDone                 = false;
    UsbAppData.writeTransferDone                = false;
    UsbAppData.controlRequestDone               = false;

    UsbCamDrv_InitStatus = DrvSts;
}


T_U3VCamDriverInitStatus UsbCamDrv_DrvInitStatus(void)
{
    return UsbCamDrv_InitStatus;
}


void UsbCamDrv_Tasks(void)
{
    /* Check the application's current state. */
    T_U3VHostResult result;

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
            /* In this state the application enables the USB Host Bus. Note how the U3V Attach 
             * event handler are registered before the bus is enabled. */
            USB_HOST_EventHandlerSet(_USBHostEventHandlerCbk, (uintptr_t)0);
            USB_U3VHost_AttachEventHandlerSet(_USBHostU3VAttachEventListenerCbk, (uintptr_t)&UsbAppData);
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
            /* In this state the application is waiting for the device to be attached */
            if(UsbAppData.deviceIsAttached)
            {
                /* A device is attached. We can open this device */
                UsbAppData.state = USB_APP_STATE_OPEN_DEVICE;
                UsbAppData.deviceIsAttached = false;
            }
            break;
            
        case USB_APP_STATE_OPEN_DEVICE:
            /* In this state the application opens the attached device */
            UsbAppData.u3vHostHandle = USB_U3VHost_Open(UsbAppData.u3vObj);
            if(UsbAppData.u3vHostHandle != U3V_HOST_HANDLE_INVALID)
            {
                /* The driver was opened successfully. Set the event handler and then go to the next state. */
                USB_U3VHost_EventHandlerSet(UsbAppData.u3vHostHandle, _USBHostU3VEventHandlerCbk, (uintptr_t)&UsbAppData);
                UsbAppData.state = USB_APP_STATE_SETUP_U3V_CONTROL_IF; //todo rework
                LED1_On();  // DEBUG XULT board
            }
            break;

        case USB_APP_STATE_SETUP_U3V_CONTROL_IF:
            /* Setting up the U3V device control function interface */
            result = U3VCtrlIf_IntfCreate(UsbAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                /* We wait for the set line coding to complete */
                UsbAppData.state = USB_APP_STATE_SET_LINE_CODING;
            }
            break;
            
        case USB_APP_STATE_SET_LINE_CODING:
            /* Here we set the Line coding. The control request done flag will be set to true when the control request has completed. */
            UsbAppData.controlRequestDone = false;
            // result = USB_HOST_CDC_ACM_LineCodingSet(UsbAppData.u3vHostHandle, NULL, &UsbAppData.cdcHostLineCoding);  //todo rework
            
            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                /* We wait for the set line coding to complete */
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_SET_LINE_CODING;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_LINE_CODING:
            
            if(UsbAppData.controlRequestDone)
            {
                if(UsbAppData.controlRequestResult != U3V_HOST_RESULT_SUCCESS)
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
            // result = USB_HOST_CDC_ACM_ControlLineStateSet(UsbAppData.u3vHostHandle, NULL, &UsbAppData.controlLineState);    //todo rework

            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                /* We wait for the set line coding to complete */
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE:
            /* Here we wait for the control line state set request to complete */
            if(UsbAppData.controlRequestDone)
            {
                if(UsbAppData.controlRequestResult != U3V_HOST_RESULT_SUCCESS)
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
            /* The prompt is sent to the device here. The write transfer done flag is updated in the event handler. */
            UsbAppData.writeTransferDone = false;
            result = USB_U3VHost_Write(UsbAppData.u3vHostHandle, NULL, (void *)prompt, 8);

            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE:
            /* Here we check if the write transfer is done */
            if(UsbAppData.writeTransferDone)
            {
                if(UsbAppData.writeTransferResult == U3V_HOST_RESULT_SUCCESS)
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
            result = USB_U3VHost_Read(UsbAppData.u3vHostHandle, NULL, UsbAppData.inDataArray, 8);
            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbAppData.state = USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE;
            }
            break;
           
        case USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE:
            /* Wait for data from device. If the data has arrived, then toggle the LED. */
            if(UsbAppData.readTransferDone)
            {
                if(UsbAppData.readTransferResult == U3V_HOST_RESULT_SUCCESS)
                {
                   if ( UsbAppData.inDataArray[0] == '1')
                   {
                       LED1_On();   // DEBUG XULT board
                   }
                   else
                   {
                       LED1_Off();  // DEBUG XULT board
                   }

                    /* Send the prompt to the device and wait for data again */
                    UsbAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            break;

        case USB_APP_STATE_ERROR:
            /* An error has occurred */
            // error handling...?
            break;
            
        default:
            break;
    }
}


T_U3VCamDriverStatus UsbCamDrv_AcquireNewImage(void *params)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (UsbCamDrv_DrvInitStatus()          == U3V_DRV_INITIALZN_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    DrvSts = (UsbCamDrv_GetCamConnectionStatus() == U3V_CAM_CONNECTED) ? DrvSts : U3V_CAM_DRV_ERROR;

    //todo

    return DrvSts;
}


T_U3VCamConnectStatus UsbCamDrv_GetCamConnectionStatus(void)
{
    T_U3VCamConnectStatus CamStatus = U3V_CAM_STATUS_UNKNOWN;

    //todo
    if (1)
    {
        CamStatus = U3V_CAM_CONNECTED;
    }
    return CamStatus;
}


T_U3VCamDevIDStatus UsbCamDrv_CamDeviceIDisValid(void)
{
    T_U3VCamDevIDStatus CamDevIDSts = U3V_CAM_DEV_ID_OK;

    CamDevIDSts = (CamPIDisValid()) ? CamDevIDSts : U3V_CAM_DEV_ID_ERROR;
    CamDevIDSts = (CamVIDisValid()) ? CamDevIDSts : U3V_CAM_DEV_ID_ERROR;

    return CamDevIDSts;
}


static inline T_U3VCamIDValid CamVIDisValid(void)
{
    //todo
    if (1)
    {
        return true;
    }
    return false;
}


static inline T_U3VCamIDValid CamPIDisValid(void)
{
    //todo
    if (1)
    {
        return true;
    }
    return false;
}
