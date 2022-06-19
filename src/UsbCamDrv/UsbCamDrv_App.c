//
// Created by mojo on 02/02/22.
//

#include "UsbCamDrv_App.h"
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

/*** App Callbacks ***/
static USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context);

static void _USBHostU3VAttachEventListenerCbk(T_U3VHostObject u3vObj, uintptr_t context);

static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context);

static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle,
                                                         T_U3VHostEvent event,
                                                         void *pEventData,
                                                         uintptr_t context);

/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VCamDriverInitStatus UsbCamDrv_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_UsbU3VAppData USB_ALIGN UsbU3VAppData;

T_U3VStreamConfig streamConfigVals;

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

    UsbCamDrv_InitStatus = DrvSts;
}


T_U3VCamDriverInitStatus UsbCamDrv_DrvInitStatus(void)
{
    return UsbCamDrv_InitStatus;
}


void UsbCamDrv_Tasks(void)
{
    T_U3VHostResult result;

    if (UsbU3VAppData.camSwResetRequested)
    {
        if ((UsbCamDrv_InitStatus == U3V_DRV_INITIALIZATION_OK) && (UsbU3VAppData.state > USB_APP_STATE_SETUP_U3V_CONTROL_CH))
        {
            UsbU3VAppData.camSwResetRequested = false;
            result = USB_U3VHost_CamSwReset(UsbU3VAppData.u3vObj);
            UsbU3VAppData.deviceWasDetached = true;
        }
    }

    if (UsbU3VAppData.deviceWasDetached)
    {
        UsbU3VAppData.state                = USB_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
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
                USB_U3VHost_EventHandlerSet(UsbU3VAppData.u3vHostHandle, _USBHostU3VEventHandlerCbk , (uintptr_t)&UsbU3VAppData);
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

        case USB_APP_STATE_GET_U3V_MANIFEST: //TODO: remove, Manifest is imported as fixed params
            // result = USB_U3VHost_GetManifestFile(UsbU3VAppData.u3vObj);
            // if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_GET_CAM_TEMPERATURE;
            }
            break;

        case USB_APP_STATE_GET_CAM_TEMPERATURE:
            result = USB_U3VHost_GetCamTemperature(UsbU3VAppData.u3vObj, &UsbU3VAppData.camTemperature);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_GET_STREAM_CAPABILITIES;
            }
            break;

        case USB_APP_STATE_GET_STREAM_CAPABILITIES:
            result = USB_U3VHost_GetStreamCapabilities(UsbU3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_SETUP_PIXEL_FORMAT;
            }
            break;
        
        case USB_APP_STATE_SETUP_PIXEL_FORMAT:
            result = USB_U3VHost_GetPixelFormat(UsbU3VAppData.u3vObj, &UsbU3VAppData.pixelFormat);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                if (UsbU3VAppData.pixelFormat != U3V_CamRegAdrLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel)
                {
                    /* set correct pixel format */
                    result = USB_U3VHost_SetPixelFormat(UsbU3VAppData.u3vObj,
                                                        U3V_CamRegAdrLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel);
                }
                else
                {
                    UsbU3VAppData.state = USB_APP_STATE_SETUP_ACQUISITION_MODE;
                }
            }
            break;

        case USB_APP_STATE_SETUP_ACQUISITION_MODE:
            result = USB_U3VHost_GetAcquisitionMode(UsbU3VAppData.u3vObj, &UsbU3VAppData.acquisitionMode);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                if (UsbU3VAppData.acquisitionMode != U3V_ACQUISITION_MODE_SINGLE_FRAME)
                {
                    /* set correct acquisition mode */
                    (void)USB_U3VHost_SetAcquisitionMode(UsbU3VAppData.u3vObj, U3V_ACQUISITION_MODE_SINGLE_FRAME);
                }
                else
                {
                    UsbU3VAppData.state = USB_APP_STATE_SETUP_STREAM;
                }
            }
            break;
        
        case USB_APP_STATE_SETUP_STREAM:    //TODO: find bug of spinlocking
            result = USB_U3VHost_GetImgPayloadSize(UsbU3VAppData.u3vObj, &UsbU3VAppData.payloadSize);
            streamConfigVals.imageSize = UsbU3VAppData.payloadSize;
            streamConfigVals.blockPadding = 8U;
            streamConfigVals.blockSize = 512U;
            streamConfigVals.maxLeaderSize = 256U;
            streamConfigVals.maxTrailerSize = 256U;
            result |= USB_U3VHost_SetupStreamTransferParams(UsbU3VAppData.u3vObj, &streamConfigVals);
            // result |= USB_U3VHost_ResetStreamCh(UsbU3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                UsbU3VAppData.state = USB_APP_STATE_READY_TO_START_IMG_ACQUISITION;
            }
            break;

        case USB_APP_STATE_READY_TO_START_IMG_ACQUISITION:
            if (UsbU3VAppData.acquisitionRequested)
            {
                result = USB_U3VHost_StreamChControl(UsbU3VAppData.u3vObj, true);
                result |= USB_U3VHost_AcquisitionStart(UsbU3VAppData.u3vObj);
                if (result == U3V_HOST_RESULT_SUCCESS)
                {
                    UsbU3VAppData.acquisitionRequested = false;
                    UsbU3VAppData.state = USB_APP_STATE_WAIT_TO_ACQUIRE_IMAGE;
                    UsbU3VAppData.imgPayloadContainer.imgPldTransfSt = SI_IMG_TRANSF_STATE_START;
                }
            }
            break;

        case USB_APP_STATE_WAIT_TO_ACQUIRE_IMAGE:
            if ((UsbU3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_START)||
                (UsbU3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_LEADER_COMPLETE)||
                (UsbU3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE))
            {
                result = USB_U3VHost_StartImgPayldTransfer(UsbU3VAppData.u3vObj,
                                                           (void *)&UsbU3VAppData.imgPayloadContainer.imgPldBfr1,
                                                           (size_t)U3V_IN_BUFFER_MAX_SIZE);
                //TODO: check result and react when not OK
            }    
            else if (UsbU3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_TRAILER_COMPLETE)
            {
                UsbU3VAppData.state = USB_APP_STATE_STOP_IMAGE_ACQ;
            }
            else
            {
                /* error, unspecified state */
            }

            break;

        case USB_APP_STATE_STOP_IMAGE_ACQ:
            result = USB_U3VHost_AcquisitionStop(UsbU3VAppData.u3vObj); //TODO: fix bug of host busy
            result |= USB_U3VHost_StreamChControl(UsbU3VAppData.u3vObj, false); //TODO: >>
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                // UsbU3VAppData.state = USB_APP_STATE_READY_TO_START_IMG_ACQUISITION;
                UsbU3VAppData.state = USB_APP_STATE_NOP; //debug temporary state
                //TODO: return on USB_APP_STATE_READY_TO_START_IMG_ACQUISITION or close power and end?
            }

        case USB_APP_STATE_NOP:
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
    DrvSts = (UsbCamDrv_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;
    
    // if (UsbU3VAppData.state == USB_APP_STATE_READY_TO_START_IMG_ACQUISITION)
    // {
    //     UsbU3VAppData.acquisitionRequested = true;
    // }
    // else
    // {
    //     DrvSts = U3V_CAM_DRV_NOT_INITD;
    // }
    // //todo: investigate if more needed

    UsbU3VAppData.acquisitionRequested = true;

    return DrvSts;
}


T_U3VCamDriverStatus UsbCamDrv_CamSwReset(void)
{
       T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (UsbCamDrv_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    DrvSts = (UsbCamDrv_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;

    UsbU3VAppData.camSwResetRequested = true;

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


static USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context)
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


static void _USBHostU3VAttachEventListenerCbk(T_U3VHostObject u3vObj, uintptr_t context)
{
    /* This function gets called when the U3V device is attached. Update the application data 
     * structure to let the application know that this device is attached. */
    T_UsbU3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_UsbU3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vObj = u3vObj;
}


static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context)
{
    /* This function gets called when the U3V device is detached. Update the application data 
     * structure to let the application know that this device is detached. */

    T_UsbU3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_UsbU3VAppData*)context;

    pUsbU3VAppData->deviceWasDetached = true;
}


static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle,
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
            break;

        case U3V_HOST_EVENT_WRITE_COMPLETE:
        case U3V_HOST_EVENT_READ_COMPLETE:
        default:
            break;
    }
    return U3V_HOST_EVENT_RESPONE_NONE;
}
