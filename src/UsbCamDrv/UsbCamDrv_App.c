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

T_UsbU3VAppData USB_ALIGN UsbU3VAppData;

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
    UsbU3VAppData.state                            = USB_APP_STATE_BUS_ENABLE;

    UsbU3VAppData.deviceIsAttached                 = false;
    UsbU3VAppData.deviceWasDetached                = false;
    UsbU3VAppData.readTransferDone                 = false;
    UsbU3VAppData.writeTransferDone                = false;
    UsbU3VAppData.controlRequestDone               = false;

    UsbCamDrv_InitStatus = DrvSts;
}


T_U3VCamDriverInitStatus UsbCamDrv_DrvInitStatus(void)
{
    return UsbCamDrv_InitStatus;
}


void UsbCamDrv_Tasks(void)
{
    T_U3VHostResult result;

    if (UsbU3VAppData.deviceWasDetached)
    {
        UsbU3VAppData.state                = USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
        UsbU3VAppData.readTransferDone     = false;
        UsbU3VAppData.writeTransferDone    = false;
        UsbU3VAppData.controlRequestDone   = false;
        UsbU3VAppData.deviceWasDetached    = false;

        U3VCtrlIf_IntfDestroy(&UsbU3VAppData.controlIfHandle);

        /* Switch off LED  */
        LED1_Off(); // DEBUG XULT board
    }

    switch (UsbU3VAppData.state)
    {
        case USB_APP_STATE_BUS_ENABLE:
            USB_HOST_EventHandlerSet(_USBHostEventHandlerCbk, (uintptr_t)0);
            USB_U3VHost_AttachEventHandlerSet(_USBHostU3VAttachEventListenerCbk, (uintptr_t)&UsbU3VAppData);
            USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            break;
        
        case USB_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            if(USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            if(UsbU3VAppData.deviceIsAttached)
            {
                UsbU3VAppData.state = USB_APP_STATE_OPEN_DEVICE;
                UsbU3VAppData.deviceIsAttached = false;
            }
            break;
            
        case USB_APP_STATE_OPEN_DEVICE:
            UsbU3VAppData.u3vHostHandle = USB_U3VHost_Open(UsbU3VAppData.u3vObj);
            if(UsbU3VAppData.u3vHostHandle != U3V_HOST_HANDLE_INVALID)
            {
                USB_U3VHost_EventHandlerSet(UsbU3VAppData.u3vHostHandle, _USBHostU3VEventHandlerCbk, (uintptr_t)&UsbU3VAppData);
                UsbU3VAppData.state = USB_APP_STATE_SETUP_U3V_CONTROL_IF;
                LED1_On();  // DEBUG XULT board
            }
            break;

        case USB_APP_STATE_SETUP_U3V_CONTROL_IF:
            result = U3VCtrlIf_IntfCreate(&UsbU3VAppData.controlIfHandle, UsbU3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_SET_LINE_CODING; //todo rework
            }
            break;
            
        case USB_APP_STATE_SET_LINE_CODING:
            UsbU3VAppData.controlRequestDone = false;
            // result = USB_HOST_CDC_ACM_LineCodingSet(UsbU3VAppData.u3vHostHandle, NULL, &UsbU3VAppData.cdcHostLineCoding);  //todo rework
            
            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_SET_LINE_CODING;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_LINE_CODING:
            
            if(UsbU3VAppData.controlRequestDone)
            {
                if(UsbU3VAppData.controlRequestResult != U3V_HOST_RESULT_SUCCESS)
                {
                    UsbU3VAppData.state = USB_APP_STATE_ERROR;
                }
                else
                {
                    UsbU3VAppData.state = USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE;
                }
            }
            break;
            
        case USB_APP_STATE_SEND_SET_CONTROL_LINE_STATE:
            UsbU3VAppData.controlRequestDone = false;
            // result = USB_HOST_CDC_ACM_ControlLineStateSet(UsbU3VAppData.u3vHostHandle, NULL, &UsbU3VAppData.controlLineState);    //todo rework

            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE:
            if(UsbU3VAppData.controlRequestDone)
            {
                if(UsbU3VAppData.controlRequestResult != U3V_HOST_RESULT_SUCCESS)
                {
                    UsbU3VAppData.state = USB_APP_STATE_ERROR;
                }
                else
                {
                    UsbU3VAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            break;
            
        case USB_APP_STATE_SEND_PROMPT_TO_DEVICE:
            UsbU3VAppData.writeTransferDone = false;
            result = USB_U3VHost_Write(UsbU3VAppData.u3vHostHandle, NULL, (void *)prompt, 8);

            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE;
            }
            break;
            
        case USB_APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE:
            if(UsbU3VAppData.writeTransferDone)
            {
                if(UsbU3VAppData.writeTransferResult == U3V_HOST_RESULT_SUCCESS)
                {
                    UsbU3VAppData.state = USB_APP_STATE_GET_DATA_FROM_DEVICE;
                }
                else
                {
                    UsbU3VAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
                }
            }
            break;
            
        case USB_APP_STATE_GET_DATA_FROM_DEVICE:
            UsbU3VAppData.readTransferDone = false;
            result = USB_U3VHost_Read(UsbU3VAppData.u3vHostHandle, NULL, UsbU3VAppData.inDataArray, 8);
            if(result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE;
            }
            break;
           
        case USB_APP_STATE_WAIT_FOR_DATA_FROM_DEVICE:
            if(UsbU3VAppData.readTransferDone)
            {
                if(UsbU3VAppData.readTransferResult == U3V_HOST_RESULT_SUCCESS)
                {
                   if ( UsbU3VAppData.inDataArray[0] == '1')
                   {
                       LED1_On();   // DEBUG XULT board
                   }
                   else
                   {
                       LED1_Off();  // DEBUG XULT board
                   }
                    UsbU3VAppData.state = USB_APP_STATE_SEND_PROMPT_TO_DEVICE;
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
