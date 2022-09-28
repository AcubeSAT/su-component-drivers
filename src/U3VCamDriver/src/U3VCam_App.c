
#include "U3VCam_App.h"



/********************************************************
* Local function declarations
*********************************************************/

static T_U3VDriverInitStatus U3VApp_DrvInitStatus(void);

static USB_HOST_EVENT_RESPONSE U3VApp_USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context);

static void U3VApp_AttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static void U3VApp_DetachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static T_U3VHostEventResponse U3VApp_HostEventHandlerCbk(T_U3VHostHandle u3vObjHandle, T_U3VHostEvent event, void *pEventData, uintptr_t context);


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VDriverInitStatus u3vDriver_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_U3VAppData u3vAppData;


/********************************************************
* Function definitions
*********************************************************/

void U3VCamDriver_Initialize(void)
{
    T_U3VDriverInitStatus drvSts = U3V_DRV_INITIALIZATION_OK;

    LED1_Off(); //TODO: remove on integration, XULT board specific

    u3vAppData.state                = U3V_APP_STATE_BUS_ENABLE;
    u3vAppData.deviceIsAttached     = false;
    u3vAppData.deviceWasDetached    = false;
    u3vAppData.camTemperature       = 0.F;
    u3vAppData.pixelFormat          = 0UL;
    u3vAppData.payloadSize          = 0UL;
    u3vAppData.acquisitionMode      = 0UL;
    u3vAppData.imgAcqRequested      = false;
    u3vAppData.camSwResetRequested  = false;
    u3vAppData.appImgTransfState    = U3V_SI_IMG_TRANSF_STATE_IDLE;
    u3vAppData.appImgBlockCounter   = 0UL;
    u3vAppData.appImgEvtCbk         = NULL;
    u3vAppData.appImgDataBfr        = NULL;

    u3vDriver_InitStatus = drvSts;
}


void U3VCamDriver_Tasks(void)
{
    T_U3VHostResult result1, result2;

    if (u3vAppData.camSwResetRequested)
    {
        if ((u3vDriver_InitStatus == U3V_DRV_INITIALIZATION_OK) && (u3vAppData.state > U3V_APP_STATE_SETUP_U3V_CONTROL_IF))
        {
            result1 = U3VHost_WriteMemRegIntegerValue(u3vAppData.u3vHostHandle, U3V_MEM_REG_INT_DEVICE_RESET, 0x1UL);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                u3vAppData.camSwResetRequested = false;
                u3vAppData.deviceWasDetached = true;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_SW_RESET_REQ_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
        }
    }

    if (u3vAppData.deviceWasDetached)
    {
        u3vAppData.state                = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
        u3vAppData.deviceWasDetached    = false;
        u3vAppData.camTemperature       = 0.F;
        u3vAppData.pixelFormat          = 0UL;
        u3vAppData.payloadSize          = 0UL;
        u3vAppData.acquisitionMode      = 0UL;
        u3vAppData.camSwResetRequested  = false;
        // U3VAppData.imgAcqRequested      = false;  //TODO: decide if this stays (case reset on error with requested true?)
        u3vAppData.appImgTransfState    = U3V_SI_IMG_TRANSF_STATE_IDLE;
        u3vAppData.appImgBlockCounter   = 0UL;

        U3VHost_CtrlIf_InterfaceDestroy(u3vAppData.u3vHostHandle);

        LED1_Off(); //TODO: remove on integration, XULT board specific
    }

    switch (u3vAppData.state)
    {
        case U3V_APP_STATE_BUS_ENABLE:
            (void)USB_HOST_EventHandlerSet(U3VApp_USBHostEventHandlerCbk, (uintptr_t)0);
            result1 = U3VHost_AttachEventHandlerSet(U3VApp_AttachEventListenerCbk, (uintptr_t)&u3vAppData);
            (void)USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                u3vAppData.state = U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_BUS_ENABLE_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;
        
        case U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            if(USB_HOST_RESULT_TRUE == USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                u3vAppData.state = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            if(u3vAppData.deviceIsAttached)
            {
                u3vAppData.state = U3V_APP_STATE_OPEN_DEVICE;
                u3vAppData.deviceIsAttached = false;
            }
            break;
            
        case U3V_APP_STATE_OPEN_DEVICE:
            u3vAppData.u3vHostHandle = U3VHost_Open(u3vAppData.u3vHostHandle);
            if(u3vAppData.u3vHostHandle != U3V_HOST_HANDLE_INVALID)
            {
                result1 = U3VHost_DetachEventHandlerSet(u3vAppData.u3vHostHandle, U3VApp_DetachEventListenerCbk, (uintptr_t)&u3vAppData);
                result2 = U3VHost_EventHandlerSet(u3vAppData.u3vHostHandle, U3VApp_HostEventHandlerCbk, (uintptr_t)&u3vAppData);
                if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
                {
                    u3vAppData.state = U3V_APP_STATE_SETUP_U3V_CONTROL_IF;
                }
                else
                {
                    U3V_REPORT_ERROR(U3V_DRV_ERR_OPEN_DEVICE_FAIL);
                    u3vAppData.state = U3V_APP_STATE_ERROR;
                }
            }
            break;

        case U3V_APP_STATE_SETUP_U3V_CONTROL_IF:
            result1 = U3VHost_CtrlIf_InterfaceCreate(u3vAppData.u3vHostHandle);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                u3vAppData.state = U3V_APP_STATE_READ_DEVICE_TEXT_DESCR;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_SETUP_CTRL_IF_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_READ_DEVICE_TEXT_DESCR:
            result1 = U3VHost_ReadMemRegStringValue(u3vAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_MANUFACTURER_NAME,
                                                    u3vAppData.camTextDescriptions.vendorName);
            result1 |= U3VHost_ReadMemRegStringValue(u3vAppData.u3vHostHandle,
                                                     U3V_MEM_REG_STRING_MODEL_NAME,
                                                     u3vAppData.camTextDescriptions.modelName);
            result2 = U3VHost_ReadMemRegStringValue(u3vAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_DEVICE_VERSION,
                                                    u3vAppData.camTextDescriptions.deviceVersion);
            result2 |= U3VHost_ReadMemRegStringValue(u3vAppData.u3vHostHandle,
                                                     U3V_MEM_REG_STRING_SERIAL_NUMBER,
                                                     u3vAppData.camTextDescriptions.serialNumber);
            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                u3vAppData.state = U3V_APP_STATE_GET_STREAM_CAPABILITIES;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_READ_TEXT_DESCR_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_GET_STREAM_CAPABILITIES:
            result1 = U3VHost_GetStreamCapabilities(u3vAppData.u3vHostHandle);
            if ((result1 == U3V_HOST_RESULT_SUCCESS))
            {
                u3vAppData.state = U3V_APP_STATE_SETUP_PIXEL_FORMAT;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_GET_STREAM_CPBL_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_PIXEL_FORMAT:
            result1 = U3VHost_ReadMemRegIntegerValue(u3vAppData.u3vHostHandle, U3V_MEM_REG_INT_PIXELFORMAT, &u3vAppData.pixelFormat);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                if (u3vAppData.pixelFormat != U3VHost_GetSelectedPixelFormat())
                {
                    result2 = U3VHost_WriteMemRegIntegerValue(u3vAppData.u3vHostHandle,
                                                              U3V_MEM_REG_INT_PIXELFORMAT,
                                                              U3VHost_GetSelectedPixelFormat());
                }
                else
                {
                    u3vAppData.state = U3V_APP_STATE_SETUP_ACQUISITION_MODE;
                }
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_SET_PIXEL_FORMAT_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_ACQUISITION_MODE:
            result1 = U3VHost_ReadMemRegIntegerValue(u3vAppData.u3vHostHandle, U3V_MEM_REG_INT_ACQUISITION_MODE, &u3vAppData.acquisitionMode);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                if (u3vAppData.acquisitionMode != U3V_ACQUISITION_MODE_SINGLE_FRAME)
                {
                    result2 = U3VHost_WriteMemRegIntegerValue(u3vAppData.u3vHostHandle,
                                                              U3V_MEM_REG_INT_ACQUISITION_MODE,
                                                              U3V_ACQUISITION_MODE_SINGLE_FRAME);
                }
                else
                {
                    u3vAppData.state = U3V_APP_STATE_SETUP_U3V_STREAM_IF;
                }
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_SET_ACQ_MODE_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_U3V_STREAM_IF:
            result1 = U3VHost_ReadMemRegIntegerValue(u3vAppData.u3vHostHandle, U3V_MEM_REG_INT_PAYLOAD_SIZE, &u3vAppData.payloadSize);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
               result2 = U3VHost_SetupStreamIfTransfer(u3vAppData.u3vHostHandle, u3vAppData.payloadSize);
               if (result2 == U3V_HOST_RESULT_SUCCESS)
               {
                    u3vAppData.state = U3V_APP_STATE_GET_CAM_TEMPERATURE;
               }
               else
               {
                   U3V_REPORT_ERROR(U3V_DRV_ERR_SETUP_STREAM_IF_FAIL);
                   u3vAppData.state = U3V_APP_STATE_ERROR;
               }
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_GET_PAYLD_SIZE_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_GET_CAM_TEMPERATURE:
            result1 = U3VHost_ReadMemRegFloatValue(u3vAppData.u3vHostHandle, U3V_MEM_REG_FLOAT_TEMPERATURE, &u3vAppData.camTemperature);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                u3vAppData.state = U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION;
                LED1_On(); //TODO: remove on integration, DEBUG XULT board specific
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_GET_CAM_TEMP_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION:
            if (u3vAppData.imgAcqRequested)
            {
                result1 = U3VHost_StreamChControl(u3vAppData.u3vHostHandle, true);
                result2 = U3VHost_AcquisitionStart(u3vAppData.u3vHostHandle);
                if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
                {
                    u3vAppData.appImgTransfState = U3V_SI_IMG_TRANSF_STATE_START;
                    u3vAppData.state = U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE;
                }
                else
                {
                    U3V_REPORT_ERROR(U3V_DRV_ERR_START_IMG_ACQ_FAIL);
                    u3vAppData.state = U3V_APP_STATE_ERROR;
                }
            }
            break;

        case U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE:
            if ((u3vAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_START) ||
                (u3vAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE) ||
                (u3vAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE))
            {
                if (u3vAppData.imgAcqRequested && u3vAppData.imgAcqReqNewBlock)
                {
                    u3vAppData.imgAcqReqNewBlock = false;
                    /* size of transfer request for Leader and Trailer packes is much smaller, but there is no issue
                     * with the following size argument being greater, those packes will arrive with their own size */
                    result1 = U3VHost_StartImgPayldTransfer(u3vAppData.u3vHostHandle, u3vAppData.appImgDataBfr, (size_t)U3V_IN_BUFFER_MAX_SIZE);
                    if (result1 != U3V_HOST_RESULT_SUCCESS)
                    {
                        U3V_REPORT_ERROR(U3V_DRV_ERR_START_IMG_TRANSF_FAIL);
                        u3vAppData.state = U3V_APP_STATE_ERROR;
                    }
                }
            }
            else if (u3vAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE)
            {
                u3vAppData.imgAcqRequested = false;
                u3vAppData.imgAcqReqNewBlock = false;
                u3vAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_IMG_TRANSF_STATE_FAIL);
                u3vAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_STOP_IMAGE_ACQ:
            result1 = U3VHost_AcquisitionStop(u3vAppData.u3vHostHandle);
            result2 = U3VHost_StreamChControl(u3vAppData.u3vHostHandle, false);
            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                u3vAppData.appImgTransfState = U3V_SI_IMG_TRANSF_STATE_IDLE;
                u3vAppData.state = U3V_APP_STATE_GET_CAM_TEMPERATURE;
                //TODO: return on idle or power down and exit?
            }
            else
            {
                U3V_REPORT_ERROR(U3V_DRV_ERR_STOP_IMG_ACQ_FAIL);
                //TODO: decide if should simply power down at this case or handle as error
            }
            break;

        case U3V_APP_STATE_ERROR:
        default:
            /* An error has occurred */
            //TODO: error handling...? request power reset?
            break;
    }
}


T_U3VCamDriverStatus U3VCamDriver_SetImagePayldTransfParams(T_U3VCamDriverPayloadEventCallback callback, void *imgDataBfr)
{
    T_U3VCamDriverStatus drvSts = U3V_CAM_DRV_OK;
    drvSts = (U3VApp_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? drvSts : U3V_CAM_DRV_NOT_INITD;

    if (drvSts != U3V_CAM_DRV_OK)
    {
        return drvSts;
    }

    if ((callback != NULL) && (imgDataBfr != NULL) && (!u3vAppData.imgAcqRequested))
    {
        u3vAppData.appImgEvtCbk = callback;
        u3vAppData.appImgDataBfr = imgDataBfr;
    }
    else
    {
        drvSts = U3V_CAM_DRV_ERROR;
    }

    return drvSts;
}


T_U3VCamDriverStatus U3VCamDriver_RequestNewImagePayloadBlock(void)
{
    T_U3VCamDriverStatus drvSts = U3V_CAM_DRV_OK;
    drvSts = (U3VApp_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? drvSts : U3V_CAM_DRV_NOT_INITD;

    if (drvSts != U3V_CAM_DRV_OK)
    {
        return drvSts;
    }

    //TODO: see if other checks needed
    if ((u3vAppData.appImgDataBfr != NULL) && (u3vAppData.appImgEvtCbk != NULL))
    {
        if (!u3vAppData.imgAcqRequested)
        {
            u3vAppData.imgAcqRequested = true;
            u3vAppData.imgAcqReqNewBlock = true;
        }
        else
        {
            /* image acquisition already requested */
            u3vAppData.imgAcqReqNewBlock = true;
        }
    }
    else
    {
        drvSts = U3V_CAM_DRV_ERROR;
    }

    return drvSts;
}


void U3VCamDriver_CancelImageAcqRequest(void)
{
    u3vAppData.imgAcqRequested = false;
    u3vAppData.imgAcqReqNewBlock = false;
    if (u3vAppData.state == U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE)
    {
        u3vAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
    }
}


T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDeviceDescriptorTextType textType, void *buffer)
{
    T_U3VCamDriverStatus drvSts = U3V_CAM_DRV_OK;
    uint8_t *lclBuffer;
    size_t size;

    drvSts = (U3VApp_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? drvSts : U3V_CAM_DRV_NOT_INITD;

    if (drvSts != U3V_CAM_DRV_OK)
    {
        return drvSts;
    }

    switch (textType)
    {
        case U3V_CAM_DRV_GET_TEXT_SERIAL_NUMBER:
            lclBuffer = u3vAppData.camTextDescriptions.serialNumber;
            size = (size_t)U3V_REG_SERIAL_NUMBER_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MANUFACTURER_NAME:
            lclBuffer = u3vAppData.camTextDescriptions.vendorName;
            size = (size_t)U3V_REG_MANUFACTURER_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MODEL_NAME:
            lclBuffer = u3vAppData.camTextDescriptions.modelName;
            size = (size_t)U3V_REG_MODEL_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_DEVICE_VERSION:
            lclBuffer = u3vAppData.camTextDescriptions.deviceVersion;
            size = (size_t)U3V_REG_DEVICE_VERSION_SIZE;
            break;

        default:
            lclBuffer = NULL;
            size = (size_t)0U;
            break;
    }
    
    if ((buffer != NULL) && (lclBuffer != NULL))
    {
        memcpy(buffer, (void *)lclBuffer, size);
    }

    return drvSts;
}


T_U3VCamDriverStatus U3VCamDriver_GetDeviceTemperature(float *temperatureC)
{
    T_U3VCamDriverStatus drvSts = U3V_CAM_DRV_OK;

    drvSts = (U3VApp_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? drvSts : U3V_CAM_DRV_NOT_INITD;
    
    if (drvSts != U3V_CAM_DRV_OK)
    {
        return drvSts;
    }

    if (temperatureC != NULL)
    {
        *temperatureC = u3vAppData.camTemperature;
    }

    return drvSts;
}


T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void)
{
    T_U3VCamDriverStatus drvSts = U3V_CAM_DRV_OK;

    drvSts = (U3VApp_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? drvSts : U3V_CAM_DRV_NOT_INITD;

    if (drvSts != U3V_CAM_DRV_OK)
    {
        return drvSts;
    }

    u3vAppData.camSwResetRequested = true;

    return drvSts;
}


size_t U3VCamDriver_GetImagePayldMaxBlockSize(void)
{
    return (size_t)U3V_IN_BUFFER_MAX_SIZE;
}


/********************************************************
* Local function definitions
*********************************************************/

static T_U3VDriverInitStatus U3VApp_DrvInitStatus(void)
{
    return u3vDriver_InitStatus;
}


static USB_HOST_EVENT_RESPONSE U3VApp_USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context)
{
    switch (event)
    {
        case USB_HOST_EVENT_DEVICE_UNSUPPORTED:
            break;
            
        default:
            /* no action */
            break;
    }
    return USB_HOST_EVENT_RESPONSE_NONE;
}


static void U3VApp_AttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context)
{
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vHostHandle = u3vObjHandle;
}


static void U3VApp_DetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context)
{
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;
    pUsbU3VAppData->deviceWasDetached = true;
}


static T_U3VHostEventResponse U3VApp_HostEventHandlerCbk(T_U3VHostHandle u3vHandle, T_U3VHostEvent event, void *pEventData, uintptr_t context)
{
    T_U3VHostEventReadCompleteData  *readCompleteEventData;
    T_U3VAppData                    *pUsbU3VAppData;
    T_U3VSiGenericPacket            *pckLeaderOrTrailer;

    pUsbU3VAppData = (T_U3VAppData*)context;
    readCompleteEventData = (T_U3VHostEventReadCompleteData *)(pEventData);
    T_U3VCamDriverImageAcqPayloadEvent appPldTransfEvent;

    switch (event)
    {
        case U3V_HOST_EVENT_IMG_PLD_RECEIVED:
            pckLeaderOrTrailer = (T_U3VSiGenericPacket*)pUsbU3VAppData->appImgDataBfr;
            pUsbU3VAppData->appImgBlockCounter++;
            if (pckLeaderOrTrailer->magicKey == U3V_LEADER_MGK_PREFIX)
            {
                /* Img Leader packet received */
                pUsbU3VAppData->appImgTransfState = U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE;
                appPldTransfEvent = U3V_CAM_DRV_IMG_LEADER_DATA;
                pUsbU3VAppData->appImgBlockCounter = 0UL;
            }
            else if (pckLeaderOrTrailer->magicKey == U3V_TRAILER_MGK_PREFIX)
            {
                /* Img Trailer packet received, end of transfer */
                pUsbU3VAppData->appImgTransfState = U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE;
                appPldTransfEvent = U3V_CAM_DRV_IMG_TRAILER_DATA;
            }
            else
            {
                /* Img Payload block with Image data */
                appPldTransfEvent = U3V_CAM_DRV_IMG_PAYLOAD_DATA;
            }
            if (u3vAppData.appImgEvtCbk != NULL)
            {
                u3vAppData.appImgEvtCbk(appPldTransfEvent,
                                        (void *)u3vAppData.appImgDataBfr,
                                        readCompleteEventData->length,
                                        pUsbU3VAppData->appImgBlockCounter);
            }
            break;

        case U3V_HOST_EVENT_WRITE_COMPLETE:
        case U3V_HOST_EVENT_READ_COMPLETE:
        default:
            break;
    }
    return U3V_HOST_EVENT_RESPONE_NONE;
}

