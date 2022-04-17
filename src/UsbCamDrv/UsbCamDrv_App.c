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

T_U3VCamDriverInitStatus UsbCamDrv_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_UsbU3VAppData USB_ALIGN UsbU3VAppData;



/********************************************************
* Function definitions
*********************************************************/

void UsbCamDrv_Initialize(void)
{
    T_U3VCamDriverInitStatus DrvSts = U3V_DRV_INITIALIZATION_OK;

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
        UsbU3VAppData.deviceWasDetached    = false;

        U3VHost_CtrlCh_InterfaceDestroy(&UsbU3VAppData.controlChHandle);

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
                USB_U3VHost_DetachEventHandlerSet(UsbU3VAppData.u3vHostHandle, _USBHostU3VDetachEventListenerCbk, (uintptr_t)&UsbU3VAppData);
                USB_U3VHost_EventHandlerSet(UsbU3VAppData.u3vHostHandle, _USBHostU3VEventHandlerCbk, (uintptr_t)&UsbU3VAppData);
                UsbU3VAppData.state = USB_APP_STATE_SETUP_U3V_CONTROL_CH;
                LED1_On();  // DEBUG XULT board
            }
            break;

        case USB_APP_STATE_SETUP_U3V_CONTROL_CH:
            result = U3VHost_CtrlCh_InterfaceCreate(&UsbU3VAppData.controlChHandle, UsbU3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_GET_U3V_MANIFEST;
            }
            break;

        case USB_APP_STATE_GET_U3V_MANIFEST:
            result = U3V_HOST_RESULT_SUCCESS;       //todo: impl GetManifest function
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_GET_STREAM_CAPABILITIES;
            }
            break;

        case USB_APP_STATE_GET_STREAM_CAPABILITIES:
            result = USB_U3VHost_GetStreamCapabilities(UsbU3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_WAIT;
            }
            break;

        case USB_APP_STATE_WAIT:
            //debug state, do nothing...
            result = result;
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

    DrvSts = (UsbCamDrv_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
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
