//
// Created by mojo on 8/12/21.
//

#include "UsbCamDrv_private.h"
#include "UsbCamDrv_main.h"


/* Local function declarations (static)*/

static inline CamIDValid_t CamVIDisValid(void);
static inline CamIDValid_t CamPIDisValid(void);


/* Constants & Variable declarations */

static T_UsbCamDrvInitStatus UsbCamDrv_InitStatus = DRV_NOT_INITIALZD;



/* Function definitions */

T_UsbCameraDriver_Status UsbCamDrv_AcquireNewImage(void *params)
{
    T_UsbCameraDriver_Status DrvSts = USB_CAM_DRV_OK;

    DrvSts = (UsbCamDrv_InitStatus == DRV_INITIALZN_OK)            ? DrvSts : USB_CAM_DRV_NOT_INITD;
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
