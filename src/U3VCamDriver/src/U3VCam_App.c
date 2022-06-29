//
// Created by fomarko on 02/02/22.
//

#include "U3VCam_App.h"
#include "U3VCam_Host.h"
#include "U3VCam_Config.h"
// #include "U3VCam_ControlIF.h"



/********************************************************
* Local function declarations
*********************************************************/

T_U3VCamDriverInitStatus U3VCamDriver_DrvInitStatus(void);

T_U3VCamConnectStatus U3VCamDriver_GetCamConnectionStatus(void);

T_U3VCamDevIDStatus U3VCamDriver_CamDeviceIDisValid(void);

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

T_U3VCamDriverInitStatus U3VCamDriver_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_U3VAppData USB_ALIGN U3VAppData;

T_U3VStreamIfConfig streamConfigVals;

/********************************************************
* Function definitions
*********************************************************/

void U3VCamDriver_Initialize(void)
{
    T_U3VCamDriverInitStatus DrvSts = U3V_DRV_INITIALIZATION_OK;

    /* DEBUG XULT - Disable VBUS power */
    VBUS_HOST_EN_PowerDisable();

    /* Switch off LEDs */
    LED0_Off(); // DEBUG XULT board
    LED1_Off(); // DEBUG XULT board

    /* Initialize the USB application state machine elements */
    U3VAppData.state                            = U3V_APP_STATE_BUS_ENABLE;
    U3VAppData.deviceIsAttached                 = false;
    U3VAppData.deviceWasDetached                = false;

    U3VCamDriver_InitStatus = DrvSts;
}


T_U3VCamDriverInitStatus U3VCamDriver_DrvInitStatus(void)
{
    return U3VCamDriver_InitStatus;
}


void U3VCamDriver_Tasks(void)
{
    T_U3VHostResult result;

    if (U3VAppData.camSwResetRequested)
    {
        if ((U3VCamDriver_InitStatus == U3V_DRV_INITIALIZATION_OK) && (U3VAppData.state > U3V_APP_STATE_SETUP_U3V_CONTROL_CH))
        {
            U3VAppData.camSwResetRequested = false;
            result = U3VHost_CamSwReset(U3VAppData.u3vObj);
            U3VAppData.deviceWasDetached = true;
        }
    }

    if (U3VAppData.deviceWasDetached)
    {
        U3VAppData.state                = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
        U3VAppData.deviceWasDetached    = false;

        U3VHost_CtrlIf_InterfaceDestroy(&U3VAppData.controlChHandle);

        /* Switch off LED  */
        LED1_Off(); // DEBUG XULT board
    }

    switch (U3VAppData.state)
    {
        case U3V_APP_STATE_BUS_ENABLE:
            USB_HOST_EventHandlerSet(_USBHostEventHandlerCbk, (uintptr_t)0);
            U3VHost_AttachEventHandlerSet(_USBHostU3VAttachEventListenerCbk, (uintptr_t)&U3VAppData);
            USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            U3VAppData.state = U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            break;
        
        case U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            if(USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                U3VAppData.state = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            if(U3VAppData.deviceIsAttached)
            {
                U3VAppData.state = U3V_APP_STATE_OPEN_DEVICE;
                U3VAppData.deviceIsAttached = false;
            }
            break;
            
        case U3V_APP_STATE_OPEN_DEVICE:
            U3VAppData.u3vHostHandle = U3VHost_Open(U3VAppData.u3vObj);
            if(U3VAppData.u3vHostHandle != U3V_HOST_HANDLE_INVALID)
            {
                U3VHost_DetachEventHandlerSet(U3VAppData.u3vHostHandle, _USBHostU3VDetachEventListenerCbk, (uintptr_t)&U3VAppData);
                U3VHost_EventHandlerSet(U3VAppData.u3vHostHandle, _USBHostU3VEventHandlerCbk , (uintptr_t)&U3VAppData);
                U3VAppData.state = U3V_APP_STATE_SETUP_U3V_CONTROL_CH;
                LED1_On();  // DEBUG XULT board
            }
            break;

        case U3V_APP_STATE_SETUP_U3V_CONTROL_CH:
            result = U3VHost_CtrlIf_InterfaceCreate(&U3VAppData.controlChHandle, U3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_GET_U3V_MANIFEST;
            }
            break;

        case U3V_APP_STATE_GET_U3V_MANIFEST: //TODO: remove, Manifest is imported as fixed params
            // result = U3VHost_GetManifestFile(UsbU3VAppData.u3vObj);
            // if (result == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_GET_CAM_TEMPERATURE;
            }
            break;

        case U3V_APP_STATE_GET_CAM_TEMPERATURE:
            result = U3VHost_GetCamTemperature(U3VAppData.u3vObj, &U3VAppData.camTemperature);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_GET_STREAM_CAPABILITIES;
            }
            break;

        case U3V_APP_STATE_GET_STREAM_CAPABILITIES:
            result = U3VHost_GetStreamCapabilities(U3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_SETUP_PIXEL_FORMAT;
            }
            break;
        
        case U3V_APP_STATE_SETUP_PIXEL_FORMAT:
            result = U3VHost_GetPixelFormat(U3VAppData.u3vObj, &U3VAppData.pixelFormat);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                if (U3VAppData.pixelFormat != U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel)
                {
                    /* set correct pixel format */
                    result = U3VHost_SetPixelFormat(U3VAppData.u3vObj,
                                                        U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel);
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_SETUP_ACQUISITION_MODE;
                }
            }
            break;

        case U3V_APP_STATE_SETUP_ACQUISITION_MODE:
            result = U3VHost_GetAcquisitionMode(U3VAppData.u3vObj, &U3VAppData.acquisitionMode);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                if (U3VAppData.acquisitionMode != U3V_ACQUISITION_MODE_SINGLE_FRAME)
                {
                    /* set correct acquisition mode */
                    (void)U3VHost_SetAcquisitionMode(U3VAppData.u3vObj, U3V_ACQUISITION_MODE_SINGLE_FRAME);
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_SETUP_STREAM;
                }
            }
            break;
        
        case U3V_APP_STATE_SETUP_STREAM:    //TODO: find bug of spinlocking
            result = U3VHost_GetImgPayloadSize(U3VAppData.u3vObj, &U3VAppData.payloadSize);
            streamConfigVals.imageSize = U3VAppData.payloadSize;
            streamConfigVals.blockPadding = 8U;
            streamConfigVals.blockSize = 512U;
            streamConfigVals.maxLeaderSize = 256U;
            streamConfigVals.maxTrailerSize = 256U;
            result |= U3VHost_SetupStreamTransferParams(U3VAppData.u3vObj, &streamConfigVals);
            // result |= U3VHost_ResetStreamCh(U3VAppData.u3vObj);
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION;
            }
            break;

        case U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION:
            if (U3VAppData.acquisitionRequested)
            {
                result = U3VHost_StreamChControl(U3VAppData.u3vObj, true);
                result |= U3VHost_AcquisitionStart(U3VAppData.u3vObj);
                if (result == U3V_HOST_RESULT_SUCCESS)
                {
                    U3VAppData.acquisitionRequested = false;
                    U3VAppData.state = U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE;
                    U3VAppData.imgPayloadContainer.imgPldTransfSt = SI_IMG_TRANSF_STATE_START;
                }
            }
            break;

        case U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE:
            if ((U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_START)||
                (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_LEADER_COMPLETE)||
                (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE))
            {
                result = U3VHost_StartImgPayldTransfer(U3VAppData.u3vObj,
                                                           (void *)&U3VAppData.imgPayloadContainer.imgPldBfr1,
                                                           (size_t)U3V_IN_BUFFER_MAX_SIZE);
                //TODO: check result and react when not OK
            }    
            else if (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_TRAILER_COMPLETE)
            {
                U3VAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
            }
            else
            {
                /* error, unspecified state */
            }

            break;

        case U3V_APP_STATE_STOP_IMAGE_ACQ:
            result = U3VHost_AcquisitionStop(U3VAppData.u3vObj); //TODO: fix bug of host busy
            result |= U3VHost_StreamChControl(U3VAppData.u3vObj, false); //TODO: >>
            if (result == U3V_HOST_RESULT_SUCCESS)
            {
                // U3VAppData.state = U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION;
                U3VAppData.state = U3V_APP_STATE_NOP; //debug temporary state
                //TODO: return on U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION or close power and end?
            }

        case U3V_APP_STATE_NOP:
            //debug state, do nothing...
            result = result;
            break;

        case U3V_APP_STATE_ERROR:
            /* An error has occurred */
            // error handling...?
            break;
            
        default:
            break;
    }
}


T_U3VCamDriverStatus U3VCamDriver_AcquireNewImage(void *params)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;
    
    // if (U3VAppData.state == U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION)
    // {
    //     U3VAppData.acquisitionRequested = true;
    // }
    // else
    // {
    //     DrvSts = U3V_CAM_DRV_NOT_INITD;
    // }
    // //todo: investigate if more needed

    U3VAppData.acquisitionRequested = true;

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void)
{
       T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;

    U3VAppData.camSwResetRequested = true;

    return DrvSts;
}


T_U3VCamConnectStatus U3VCamDriver_GetCamConnectionStatus(void)
{
    T_U3VCamConnectStatus CamStatus = U3V_CAM_STATUS_UNKNOWN;

    //todo
    if (1)
    {
        CamStatus = U3V_CAM_CONNECTED;
    }
    return CamStatus;
}


T_U3VCamDevIDStatus U3VCamDriver_CamDeviceIDisValid(void)
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
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vObj = u3vObj;
}


static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context)
{
    /* This function gets called when the U3V device is detached. Update the application data 
     * structure to let the application know that this device is detached. */

    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;

    pUsbU3VAppData->deviceWasDetached = true;
}


static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle,
                                                         T_U3VHostEvent event,
                                                         void *pEventData,
                                                         uintptr_t context)
{
    T_U3VHostEventReadCompleteData  *readCompleteEventData;
    T_U3VAppData                 *pUsbU3VAppData;
    T_U3VSiGenericPacket            *pckLeaderOrTrailer;
    T_U3VHostResult                 result;
    size_t                          length;

    pUsbU3VAppData = (T_U3VAppData*)context;
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
