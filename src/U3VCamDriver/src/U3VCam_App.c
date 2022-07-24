//
// Created by fomarko on 02/02/22.
//

#include "U3VCam_App.h"
#include "U3VCam_Host.h"
#include "U3VCam_Config.h"
#include "system/dma/sys_dma.h"
#include "U3VCamDriver.h"


/********************************************************
* Local function declarations
*********************************************************/

T_U3VDriverInitStatus U3VCamDriver_DrvInitStatus(void);

T_U3VCamConnectStatus U3VCamDriver_GetCamConnectionStatus(void);

T_U3VCamDevIDStatus U3VCamDriver_CamDeviceIDisValid(void);

static inline T_U3VCamIDValid CamVIDisValid(void);

static inline T_U3VCamIDValid CamPIDisValid(void);

/*** App Callbacks ***/
static USB_HOST_EVENT_RESPONSE _USBHostEventHandlerCbk(USB_HOST_EVENT event, void *pEventData, uintptr_t context);

static void _USBHostU3VAttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static void _USBHostU3VDetachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context);

static T_U3VHostEventResponse _USBHostU3VEventHandlerCbk(T_U3VHostHandle u3vObjHandle,
                                                         T_U3VHostEvent event,
                                                         void *pEventData,
                                                         uintptr_t context);

static SYS_DMA_CHANNEL_CALLBACK _USBHostU3VDmaTransfCbk(SYS_DMA_TRANSFER_EVENT dmaEvent, uintptr_t context);


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VDriverInitStatus U3VDriver_InitStatus = U3V_DRV_NOT_INITIALIZED;

T_U3VAppData USB_ALIGN U3VAppData;

T_U3VStreamIfConfig streamConfigVals;

uint32_t    u3vAppStMchStepbits;    //debug bitfield monitor var for U3V App State Machine

uint8_t     imgBfrDst[U3V_IN_BUFFER_MAX_SIZE]; //TODO: debug remove


/********************************************************
* Function definitions
*********************************************************/

void U3VCamDriver_Initialize(void)
{
    T_U3VDriverInitStatus DrvSts = U3V_DRV_INITIALIZATION_OK;

    /* DEBUG XULT - Disable VBUS power */
    VBUS_HOST_EN_PowerDisable();

    /* Switch off LEDs */
    LED0_Off(); // DEBUG XULT board
    LED1_Off(); // DEBUG XULT board

    /* Initialize the USB application state machine elements */
    U3VAppData.state                            = U3V_APP_STATE_BUS_ENABLE;
    U3VAppData.deviceIsAttached                 = false;
    U3VAppData.deviceWasDetached                = false;

    U3VDriver_InitStatus = DrvSts;

    SYS_DMA_DataWidthSetup(U3V_DMA_CH_SEL, SYS_DMA_WIDTH_8_BIT);
    SYS_DMA_AddressingModeSetup(U3V_DMA_CH_SEL,
                                SYS_DMA_SOURCE_ADDRESSING_MODE_INCREMENTED, 
                                SYS_DMA_DESTINATION_ADDRESSING_MODE_INCREMENTED);
    SYS_DMA_ChannelCallbackRegister(U3V_DMA_CH_SEL, _USBHostU3VDmaTransfCbk, 0);

    u3vAppStMchStepbits = 0x0UL;
}


T_U3VDriverInitStatus U3VCamDriver_DrvInitStatus(void)
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
        // U3VAppData.acquisitionRequested = false;

        U3VHost_CtrlIf_InterfaceDestroy(U3VAppData.u3vHostHandle);

        u3vAppStMchStepbits = 0x0003UL;
        /* Switch off LED of DEBUG XULT board */
        LED1_Off(); //TODO: remove on integration
    }

    switch (U3VAppData.state)
    {
        case U3V_APP_STATE_BUS_ENABLE:
            u3vAppStMchStepbits = 0x0001UL;
            (void) USB_HOST_EventHandlerSet(_USBHostEventHandlerCbk, (uintptr_t)0);
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
                                                    U3VAppData.camTextDescr.vendorName);
            result1 |= U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_MODEL_NAME,
                                                    U3VAppData.camTextDescr.modelName);
            result2 = U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_DEVICE_VERSION,
                                                    U3VAppData.camTextDescr.deviceVersion);
            result2 |= U3VHost_ReadMemRegStringValue(U3VAppData.u3vHostHandle,
                                                    U3V_MEM_REG_STRING_SERIAL_NUMBER,
                                                    U3VAppData.camTextDescr.serialNumber);

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
                if (U3VAppData.pixelFormat != U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel)
                {
                    /* set correct pixel format */
                    result2 = U3VHost_WriteMemRegIntegerValue(U3VAppData.u3vHostHandle,
                                                             U3V_MEM_REG_INT_PIXELFORMAT,
                                                             U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].pixelFormatCtrlVal_Int_Sel);
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
                    /* set correct acquisition mode */
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
                streamConfigVals.imageSize = U3VAppData.payloadSize;
                streamConfigVals.blockPadding = U3V_TARGET_ARCH_BYTE_ALIGNMENT;
                streamConfigVals.blockSize = U3V_IN_BUFFER_MAX_SIZE;
                streamConfigVals.maxLeaderSize = 256U;
                streamConfigVals.maxTrailerSize = 256U;
            }
            result2 = U3VHost_SetupStreamTransferParams(U3VAppData.u3vHostHandle, &streamConfigVals);
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
                LED1_On(); // DEBUG XULT board - turn LED1 ON to show cam is ready for image acq
            }
            else
            {
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION:
            u3vAppStMchStepbits |= 0x0800UL;
            if (U3VAppData.acquisitionRequested)
            {
                result1 = U3VHost_StreamChControl(U3VAppData.u3vHostHandle, true);
                result2 = U3VHost_AcquisitionStart(U3VAppData.u3vHostHandle);

                if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
                {
                    // U3VAppData.acquisitionRequested = false;
                    U3VAppData.imgPayloadContainer.imgPldTransfSt = SI_IMG_TRANSF_STATE_START;
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
            if ((U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_START)||
                (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_LEADER_COMPLETE)||
                (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_PAYLOAD_BLOCKS_COMPLETE))
            {
                result1 = U3VHost_StartImgPayldTransfer(U3VAppData.u3vHostHandle,
                                                       (void *)&U3VAppData.imgPayloadContainer.imgPldBfr1,
                                                       (size_t)U3V_IN_BUFFER_MAX_SIZE);
                if ((result1 != U3V_HOST_RESULT_SUCCESS) && (result1 != U3V_HOST_RESULT_BUSY))
                {
                    U3VAppData.state = U3V_APP_STATE_ERROR;
                }                  
            }    
            else if (U3VAppData.imgPayloadContainer.imgPldTransfSt == SI_IMG_TRANSF_STATE_TRAILER_COMPLETE)
            {
                U3VAppData.acquisitionRequested = false;
                U3VAppData.state = U3V_APP_STATE_STOP_IMAGE_ACQ;
            }
            else
            {
                /* error, unspecified state */
                U3VAppData.state = U3V_APP_STATE_ERROR;
            }
            break;

        case U3V_APP_STATE_STOP_IMAGE_ACQ:
            u3vAppStMchStepbits |= 0x2000UL;
            result1 = U3VHost_AcquisitionStop(U3VAppData.u3vHostHandle);
            result2 = U3VHost_StreamChControl(U3VAppData.u3vHostHandle, false);
            if ((result1 == U3V_HOST_RESULT_SUCCESS) && (result2 == U3V_HOST_RESULT_SUCCESS))
            {
                U3VAppData.state = U3V_APP_STATE_NOP; //debug temporary state
                //TODO: return on U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION or power down and exit?
            }
            else
            {
                // U3VAppData.state = U3V_APP_STATE_ERROR;
                //TODO: device if should simply power down at this case or handle as error
            }
            break;

        case U3V_APP_STATE_NOP:
            u3vAppStMchStepbits |= 0x80000000UL;
            //debug state, wait here do nothing...
            break;

        case U3V_APP_STATE_ERROR:
        default:
            u3vAppStMchStepbits |= 0x40000000UL;
            /* An error has occurred */
            //TODO: error handling...?
            break;
    }
}


T_U3VCamDriverStatus U3VCamDriver_AcquireNewImage(void *params)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    // DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;
    
    // if (U3VAppData.state == U3V_APP_STATE_READY_TO_START_IMG_ACQUISITION)
    // {
    //     U3VAppData.acquisitionRequested = true;
    // }
    // else
    // {
    //     DrvSts = U3V_CAM_DRV_NOT_INITD;
    // }
    // //TODO: investigate if more needed

    U3VAppData.acquisitionRequested = true;

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDevDescrTextType textType, void *buffer)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;
    char *lclBuffer;
    size_t size;

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    // DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;

    switch (textType)
    {
        case U3V_CAM_DRV_GET_TEXT_SERIAL_NUMBER:
            lclBuffer = U3VAppData.camTextDescr.serialNumber;
            size = (size_t)U3V_REG_SERIAL_NUMBER_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MANUFACTURER_NAME:
            lclBuffer = U3VAppData.camTextDescr.vendorName;
            size = (size_t)U3V_REG_MANUFACTURER_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_MODEL_NAME:
            lclBuffer = U3VAppData.camTextDescr.modelName;
            size = (size_t)U3V_REG_MODEL_NAME_SIZE;
            break;

        case U3V_CAM_DRV_GET_TEXT_DEVICE_VERSION:
            lclBuffer = U3VAppData.camTextDescr.deviceVersion;
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

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    // DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;

    if (temperatureC != NULL)
    {
        *temperatureC = U3VAppData.camTemperature;
    }

    return DrvSts;
}


T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void)
{
    T_U3VCamDriverStatus DrvSts = U3V_CAM_DRV_OK;

    DrvSts = (U3VCamDriver_DrvInitStatus()          == U3V_DRV_INITIALIZATION_OK)  ? DrvSts : U3V_CAM_DRV_NOT_INITD;
    // DrvSts = (U3VCamDriver_GetCamConnectionStatus() == U3V_CAM_CONNECTED)          ? DrvSts : U3V_CAM_DRV_ERROR;

    U3VAppData.camSwResetRequested = true;

    return DrvSts;
}


T_U3VCamConnectStatus U3VCamDriver_GetCamConnectionStatus(void)
{
    T_U3VCamConnectStatus CamStatus = U3V_CAM_STATUS_UNKNOWN;

    //TODO
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
    //TODO
    if (1)
    {
        return true;
    }
    return false;
}


static inline T_U3VCamIDValid CamPIDisValid(void)
{
    //TODO
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


static void _USBHostU3VAttachEventListenerCbk(T_U3VHostHandle u3vObjHandle, uintptr_t context)
{
    /* This function gets called when the U3V device is attached. Update the application data 
     * structure to let the application know that this device is attached. */
    T_U3VAppData *pUsbU3VAppData;
    pUsbU3VAppData = (T_U3VAppData*)context;

    pUsbU3VAppData->deviceIsAttached = true;
    pUsbU3VAppData->u3vHostHandle = u3vObjHandle;
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
    T_U3VAppData                    *pUsbU3VAppData;
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
                if (!SYS_DMA_ChannelIsBusy(U3V_DMA_CH_SEL))
                {
                    if (SYS_DMA_ChannelTransfer(U3V_DMA_CH_SEL,
                                                pUsbU3VAppData->imgPayloadContainer.imgPldBfr1,
                                                imgBfrDst, //TODO: add target dest addr ifaceW
                                                readCompleteEventData->length))
                    {
                        pUsbU3VAppData->imgPayloadContainer.imgPldBfr1St = IMG_PLD_BFR_READ_STATE_ACTV;
                    }
                }
                else
                {
                    pUsbU3VAppData->imgPayloadContainer.imgPldBfr1St = IMG_PLD_BFR_WRITE_STATE_ACTV;
                }
            }
            break;

        case U3V_HOST_EVENT_WRITE_COMPLETE:
        case U3V_HOST_EVENT_READ_COMPLETE:
        default:
            break;
    }
    return U3V_HOST_EVENT_RESPONE_NONE;
}


static SYS_DMA_CHANNEL_CALLBACK _USBHostU3VDmaTransfCbk(SYS_DMA_TRANSFER_EVENT dmaEvent, uintptr_t context)
{
    SYS_DMA_TRANSFER_EVENT event = dmaEvent;
    //TODO sync with request?
}

