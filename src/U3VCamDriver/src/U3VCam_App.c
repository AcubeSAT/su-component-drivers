//
// Created by fomarko on 02/02/22.
//

#include "U3VCam_App.h"



/********************************************************
* Local function declarations
*********************************************************/

static T_U3VDriverInitStatus _U3VCamDriver_DrvInitStatus(void);

static USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context);

static void _USBHostU3VAttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vObjHandle, T_U3VHostEvent event, void *pEventData, uintptr_t context);


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VDriverInitStatus U3VDriver_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_U3VAppData USB_ALIGN U3VAppData;

uint32_t    u3vAppStMchStepbits;    //TODO: maybe remove, debug bitfield monitor var for U3V App State Machine


/********************************************************
* Function definitions
*********************************************************/

void U3VCamDriver_Initialize(void)
{
    T_U3VDriverInitStatus DrvSts = U3V_DRV_INITIALIZATION_OK;

    LED1_Off(); //TODO: remove on integration, XULT board specific

    U3VAppData.state                = U3V_APP_STATE_BUS_ENABLE;
    U3VAppData.deviceIsAttached     = false;
    U3VAppData.deviceWasDetached    = false;

    U3VAppData.camTemperature       = 0.F;
    U3VAppData.pixelFormat          = 0UL;
    U3VAppData.payloadSize          = 0UL;
    U3VAppData.acquisitionMode      = 0UL;
    U3VAppData.imgAckRequested      = false;
    U3VAppData.camSwResetRequested  = false;
    U3VAppData.appImgTransfState    = U3V_SI_IMG_TRANSF_STATE_IDLE;
    U3VAppData.appImgBlockCounter   = 0UL;
    U3VAppData.appImgEvtCbk         = NULL;
    U3VAppData.appImgDataBfr        = NULL;


    U3VDriver_InitStatus = DrvSts;

    u3vAppStMchStepbits = 0x0UL;
}


static T_U3VDriverInitStatus _U3VCamDriver_DrvInitStatus(void)
{
    return U3VDriver_InitStatus;
}


void U3VCamDriver_Tasks(void)
{
    T_U3VHostResult result1, result2;

    if (U3VAppData.camSwResetRequested)
    {
        if ((U3VDriver_InitStatus == U3V_DRV_INITIALIZATION_OK) && (U3VAppData.state > U3V_APP_STATE_SETUP_U3V_CONTROL_IF))
        {
            result1 = U3VHost_WriteMemRegIntegerValue(U3VAppData.u3vHostHandle, U3V_MEM_REG_INT_DEVICE_RESET, 0x1UL);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.camSwResetRequested = false;
                U3VAppData.deviceWasDetached = true;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
        }
    }

    if (U3VAppData.deviceWasDetached)
    {
        U3VAppData.state                = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
        U3VAppData.deviceWasDetached    = false;
        U3VAppData.camTemperature       = 0.F;
        U3VAppData.pixelFormat          = 0UL;
        U3VAppData.payloadSize          = 0UL;
        U3VAppData.acquisitionMode      = 0UL;
        U3VAppData.camSwResetRequested  = false;
        // U3VAppData.imgAckRequested      = false;  //TODO: decide if this stays (case reset on error with requested true?)
        U3VAppData.appImgTransfState    = U3V_SI_IMG_TRANSF_STATE_IDLE;
        U3VAppData.appImgBlockCounter   = 0UL;

        U3VHost_CtrlIf_InterfaceDestroy(U3VAppData.u3vHostHandle);

        u3vAppStMchStepbits = 0x0003UL;

        LED1_Off(); //TODO: remove on integration, XULT board specific
    }

    switch (U3VAppData.state)
    {
        case U3V_APP_STATE_BUS_ENABLE:
            u3vAppStMchStepbits = 0x0001UL;
            (void)USB_HOST_EventHandlerSet(_USBHostEventHandlerCbk, (uintptr_t)0);
            result1 = U3VHost_AttachEventHandlerSet(_USBHostU3VAttachEventListenerCbk, (uintptr_t)&U3VAppData);
            (void)USB_HOST_BusEnable(USB_HOST_BUS_ALL);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;
        
        case U3V_APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            u3vAppStMchStepbits |= 0x0002UL;
            if(USB_HOST_RESULT_TRUE == USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                U3VAppData.state = U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case U3V_APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            u3vAppStMchStepbits |= 0x0004UL;
            if(U3VAppData.deviceIsAttached)
            {
                U3VAppData.state = U3V_APP_STATE_OPEN_DEVICE;
                U3VAppData.deviceIsAttached = false;
            }
            break;
            
        case U3V_APP_STATE_OPEN_DEVICE:
            u3vAppStMchStepbits |= 0x0008UL;
            U3VAppData.u3vHostHandle = U3VHost_Open(U3VAppData.u3vHostHandle);
            if(U3VAppData.u3vHostHandle != U3V_HOST_HANDLE_INVALID)
            {
                result1 = U3VHost_DetachEventHandlerSet(U3VAppData.u3vHostHandle,
                                                        _USBHostU3VDetachEventListenerCbk,
                                                        (uintptr_t)&U3VAppData);
                result2 = U3VHost_EventHandlerSet(U3VAppData.u3vHostHandle,
                                                  _USBHostU3VEventHandlerCbk,
                                                  (uintptr_t)&U3VAppData);
                if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
                {
                    U3VAppData.state = U3V_APP_STATE_SETUP_U3V_CONTROL_IF;
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_ERROR;
                }
            }
            break;

        case U3V_APP_STATE_SETUP_U3V_CONTROL_IF:
            u3vAppStMchStepbits |= 0x0010UL;
            result1 = U3VHost_CtrlIf_InterfaceCreate(U3VAppData.u3vHostHandle);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_READ_DEVICE_TEXT_DESCR;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_READ_DEVICE_TEXT_DESCR:
            u3vAppStMchStepbits |= 0x0020UL;
            result1 = U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_MANUFACTURER_NAME,
                                                    U3VAppData.camTextDescriptions.vendorName);
            result1 |= U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                     U3V_MEM_REG_STRING_MODEL_NAME,
                                                     U3VAppData.camTextDescriptions.modelName);
            result2 = U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_DEVICE_VERSION,
                                                    U3VAppData.camTextDescriptions.deviceVersion);
            result2 |= U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                     U3V_MEM_REG_STRING_SERIAL_NUMBER,
                                                     U3VAppData.camTextDescriptions.serialNumber);

            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                U3VAppData.state = U3V_APP_STATE_GET_STREAM_CAPABILITIES;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_GET_STREAM_CAPABILITIES:
            u3vAppStMchStepbits |= 0x0040UL;
            result1 = U3VHost_GetStreamCapabilities(U3VAppData.u3vHostHandle);
            if ((result1 == U3V_HOST_RESULT_SUCCESS))
            {
                U3VAppData.state = U3V_APP_STATE_SETUP_PIXEL_FORMAT;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_PIXEL_FORMAT:
            u3vAppStMchStepbits |= 0x0080UL;
            result1 = U3VHost_ReadMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                     U3V_MEM_REG_INT_PIXELFORMAT,
                                                     &U3VAppData.pixelFormat);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                if (U3VAppData.pixelFormat != U3VCamRegisterCfgTable.pixelFormatCtrlVal_Int_Sel)
                {
                    result2 = U3VHost_WriteMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                              U3V_MEM_REG_INT_PIXELFORMAT,
                                                              U3VCamRegisterCfgTable.pixelFormatCtrlVal_Int_Sel);
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_SETUP_ACQUISITION_MODE;
                }
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_ACQUISITION_MODE:
            u3vAppStMchStepbits |= 0x0100UL;
            result1 = U3VHost_ReadMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                     U3V_MEM_REG_INT_ACQUISITION_MODE,
                                                     &U3VAppData.acquisitionMode);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                if (U3VAppData.acquisitionMode != U3V_ACQUISITION_MODE_SINGLE_FRAME)
                {
                    result2 = U3VHost_WriteMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                              U3V_MEM_REG_INT_ACQUISITION_MODE,
                                                              U3V_ACQUISITION_MODE_SINGLE_FRAME);
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_SETUP_U3V_STREAM_IF;
                }
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_SETUP_U3V_STREAM_IF:
            u3vAppStMchStepbits |= 0x0200UL;
            result1 = U3VHost_ReadMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                     U3V_MEM_REG_INT_PAYLOAD_SIZE,
                                                     &U3VAppData.payloadSize);
            /* setup stream config data */
            {
                U3VAppData.streamConfig.imageSize = (uint64_t)U3VAppData.payloadSize;
                U3VAppData.streamConfig.blockPadding = (uint32_t)U3V_TARGET_ARCH_BYTE_ALIGNMENT;
                U3VAppData.streamConfig.blockSize = (uint64_t)U3V_IN_BUFFER_MAX_SIZE;
                U3VAppData.streamConfig.maxLeaderSize = (uint32_t)U3V_LEADER_MAX_SIZE;
                U3VAppData.streamConfig.maxTrailerSize = (uint32_t)U3V_TRAILER_MAX_SIZE;
            }
            result2 = U3VHost_SetupStreamTransferParams(U3VAppData.u3vHostHandle, &U3VAppData.streamConfig);
            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                U3VAppData.state = U3V_APP_STATE_GET_CAM_TEMPERATURE;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_GET_CAM_TEMPERATURE:
            u3vAppStMchStepbits |= 0x0400UL;
            result1 = U3VHost_ReadMemRegFloatValue(U3VAppData.u3vHostHandle,
                                                   U3V_MEM_REG_FLOAT_TEMPERATURE,
                                                   &U3VAppData.camTemperature);
            if (result1 == U3V_HOST_RESULT_SUCCESS)
            {
                U3VAppData.state = U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION;
                LED1_On(); //TODO: remove on integration, DEBUG XULT board specific
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION:
            u3vAppStMchStepbits |= 0x0800UL;
            if (U3VAppData.imgAckRequested)
            {
                result1 = U3VHost_StreamChControl(U3VAppData.u3vHostHandle, true);
                result2 = U3VHost_AcquisitionStart(U3VAppData.u3vHostHandle);

                if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
                {
                    U3VAppData.appImgTransfState = U3V_SI_IMG_TRANSF_STATE_START;
                    U3VAppData.state = U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE;
                }
                else
                {
                    U3VAppData.state = U3V_APP_STATE_ERROR;
                }
            }
            break;

        case U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE:
            u3vAppStMchStepbits |= 0x1000UL;
            if ((U3VAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_START) ||
                (U3VAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE) ||
                (U3VAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE))
            {
                if (U3VAppData.imgAckRequested && U3VAppData.imgAckReqNewBlock)
                {
                    U3VAppData.imgAckReqNewBlock = false;
                    result1 = U3VHost_StartImgPayldTransfer(U3VAppData.u3vHostHandle,
                                                            U3VAppData.appImgDataBfr,
                                                            (size_t)U3V_IN_BUFFER_MAX_SIZE); /* Leader and Trailer will be less but no problem with this req */

                    if (result1 != U3V_HOST_RESULT_SUCCESS)
                    {
                        U3VAppData.state = U3V_APP_STATE_ERROR;
                    }
                }
            }
            else if (U3VAppData.appImgTransfState == U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE)
            {
                U3VAppData.imgAckRequested = false;
                U3VAppData.imgAckReqNewBlock = false;
                U3VAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_STOP_IMAGE_ACQ:
            u3vAppStMchStepbits |= 0x2000UL;
            result1 = U3VHost_AcquisitionStop(U3VAppData.u3vHostHandle);
            result2 = U3VHost_StreamChControl(U3VAppData.u3vHostHandle, false);
            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                U3VAppData.appImgTransfState = U3V_SI_IMG_TRANSF_STATE_IDLE;
                U3VAppData.state = U3V_APP_STATE_GET_CAM_TEMPERATURE;
                //TODO: return on idle or power down and exit?
            }
            else
            {
                // U3VAppData.state = U3V_APP_STATE_ERROR;
                //TODO: decide if should simply power down at this case or handle as error
            }
            break;

        case U3V_APP_STATE_ERROR:
        default:
            u3vAppStMchStepbits |= 0x40000000UL;
            /* An error has occurred */
            //TODO: error handling...? request power reset?
            break;
    }
}


T_U3VCamDriverStatus U3VCamDriver_SetImagePayloadTransferCfg(T_U3VCamDriverPayloadEventCallback callback, void *imgData)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;
    DrvSts = (_U3VCamDriver_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? DrvSts : U3V_CAM_DRV_NOT_INITD;

    if (DrvSts != U3V_CAM_DRV_NOT_INITD)
    {
        if ((callback != NULL) && (imgData != NULL) && (!U3VAppData.imgAckRequested))
        {
            U3VAppData.appImgEvtCbk = callback;
            U3VAppData.appImgDataBfr = imgData;
        }
        else
        {
            DrvSts = U3V_CAM_DRV_ERROR;
        }
    }

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_RequestNewImagePayloadBlock(void)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (_U3VCamDriver_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? DrvSts : U3V_CAM_DRV_NOT_INITD;

    //TODO: see if other checks needed
    if ((U3VAppData.appImgDataBfr != NULL) && (U3VAppData.appImgEvtCbk != NULL))
    {
        if (!U3VAppData.imgAckRequested)
        {
            U3VAppData.imgAckRequested = true;
            U3VAppData.imgAckReqNewBlock = true;
        }
        else
        {
            /* image acquisition already requested */
            U3VAppData.imgAckReqNewBlock = true;
        }
    }
    else
    {
        DrvSts = U3V_CAM_DRV_ERROR;
    }

    return DrvSts;
}


void U3VCamDriver_CancelImageAcquisitionRequest(void)
{
    U3VAppData.imgAckRequested = false;
    U3VAppData.imgAckReqNewBlock = false;
    if (U3VAppData.state == U3V_APP_STATE_WAIT_TO_ACQUIRE_IMAGE)
    {
        U3VAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
    }
}


T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDeviceDescriptorTextType textType, void *buffer)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;
    char *lclBuffer;
    size_t size;

    DrvSts = (_U3VCamDriver_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? DrvSts : U3V_CAM_DRV_NOT_INITD;

    switch (textType)
    {
        case U3V_CAM_DRV_GET_TEXT_SERIAL_NUMBER:
            lclBuffer = U3VAppData.camTextDescriptions.serialNumber;
            size = (size_t)U3V_REG_SERIAL_NUMBER_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MANUFACTURER_NAME:
            lclBuffer = U3VAppData.camTextDescriptions.vendorName;
            size = (size_t)U3V_REG_MANUFACTURER_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MODEL_NAME:
            lclBuffer = U3VAppData.camTextDescriptions.modelName;
            size = (size_t)U3V_REG_MODEL_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_DEVICE_VERSION:
            lclBuffer = U3VAppData.camTextDescriptions.deviceVersion;
            size = (size_t)U3V_REG_DEVICE_VERSION_SIZE;
            break;

        default:
            lclBuffer = NULL;
            size = (size_t)0U;
            break;
    }
    if ((buffer != NULL) && (lclBuffer != NULL))
    {
        memcpy((void *)buffer, (void *)lclBuffer, size);
    }

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_GetDeviceTemperature(float *temperatureC)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (_U3VCamDriver_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? DrvSts : U3V_CAM_DRV_NOT_INITD;

    if (temperatureC != NULL)
    {
        *temperatureC = U3VAppData.camTemperature;
    }

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (_U3VCamDriver_DrvInitStatus() == U3V_DRV_INITIALIZATION_OK) ? DrvSts : U3V_CAM_DRV_NOT_INITD;

    U3VAppData.camSwResetRequested = true;

    return DrvSts;
}


size_t U3VCamDriver_GetPayloadBlockSize(void)
{
    return (size_t)U3V_IN_BUFFER_MAX_SIZE;
}


static USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context)
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


static void _USBHostU3VAttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context)
{
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vHostHandle = u3vObjHandle;
}


static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vHandle, uintptr_t context)
{
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;
    pUsbU3VAppData->deviceWasDetached = true;
}


static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vHandle, T_U3VHostEvent event, void *pEventData, uintptr_t context)
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
            if (U3VAppData.appImgEvtCbk != NULL)
            {
                U3VAppData.appImgEvtCbk(appPldTransfEvent,
                                        (void *)U3VAppData.appImgDataBfr,
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

