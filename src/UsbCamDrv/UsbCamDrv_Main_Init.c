//
// Created by mojo on 14/01/22.
//

#include "UsbCamDrv_Main.h"
#include "UsbCamDrv_Config.h"

/*** Local function declarations (static) ***/



/*** Constants & Variable declarations ***/

T_UsbCamDrvInitStatus UsbCamDrv_InitStatus = DRV_NOT_INITIALZD;


//USB_HOST_DEVICE_OBJ_HANDLE UsbCamDrv_objHandle;


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


