
#include "U3VCam_Host.h"
#include "U3VCam_Host_Local.h"

#include "FreeRTOS.h"
#include "task.h"



/********************************************************
* Local function declarations
*********************************************************/

static void U3VHost_Initialize(void *data);

static void U3VHost_Deinitialize(void);

static void U3VHost_Reinitialize(void *msdInitData);

static void U3VHost_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                 USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                 USB_DEVICE_DESCRIPTOR *deviceDescriptor);

static void U3VHost_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void U3VHost_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void U3VHost_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
                                    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                    size_t nInterfaces,
                                    uint8_t *descriptor);

static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE U3VHost_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                              USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                              void *eventData,
                                                                              uintptr_t context);

static void U3VHost_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static void U3VHost_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static USB_HOST_DEVICE_EVENT_RESPONSE U3VHost_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                 USB_HOST_DEVICE_EVENT event,
                                                                 void *eventData,
                                                                 uintptr_t context);

static int32_t U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle);

static int32_t U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle);

static int32_t U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static T_U3VHostResult U3VHost_HostToU3VResultsMap(USB_HOST_RESULT hostResult);

static T_U3VHostResult U3VHost_CtrlIfReadMemory(T_U3VControlIfObj *u3vCtrlIf,
                                                T_U3VHostTransferHandle *transferHandle,
                                                uint64_t memAddress,
                                                size_t transfSize,
                                                uint32_t *bytesRead,
                                                void *buffer);

static T_U3VHostResult U3VHost_CtrlIfWriteMemory(T_U3VControlIfObj *u3vCtrlIf,
                                                 T_U3VHostTransferHandle *transferHandle,
                                                 uint64_t memAddress,
                                                 size_t transfSize,
                                                 uint32_t *bytesWritten,
                                                 const void *buffer);

static void U3VHost_CtrlIfTransferReqCompleteCbk(T_U3VHostHandle u3vHostHandle, T_U3VHostEvent transfEvent, void *transfData);

static inline void U3VHost_CtrlIfClearObjData(T_U3VControlIfObj *pCtrlIfObj);

static inline uint32_t U3VHost_LCMui32(uint32_t n1, uint32_t n2); /* least common multiplier calc restricted to 32bit size max */


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VHostInstanceObj gUSBHostU3VObj[U3V_HOST_INSTANCES_NUMBER];

T_U3VHostAttachListenerObj gUSBHostU3VAttachListener[U3V_HOST_ATTACH_LISTENERS_NUMBER];

USB_HOST_CLIENT_DRIVER gUSBHostU3VClientDriver =
{
    .initialize             = U3VHost_Initialize,
    .deinitialize           = U3VHost_Deinitialize,
    .reinitialize           = U3VHost_Reinitialize,
    .interfaceAssign        = U3VHost_InterfaceAssign,
    .interfaceRelease       = U3VHost_InterfaceRelease,
    .interfaceEventHandler  = U3VHost_InterfaceEventHandler,
    .interfaceTasks         = U3VHost_InterfaceTasks,
    .deviceEventHandler     = U3VHost_DeviceEventHandler,
    .deviceAssign           = U3VHost_DeviceAssign,
    .deviceRelease          = U3VHost_DeviceRelease,
    .deviceTasks            = U3VHost_DeviceTasks
};


/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context)
{
    T_U3VHostResult result = U3V_HOST_RESULT_FAILURE;
    T_U3VHostAttachListenerObj *attachListener;

    if (eventHandler == NULL)
    {
        result = U3V_HOST_RESULT_INVALID_PARAMETER;
    }
    else
    {
        for (uint32_t iterator = 0UL; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
        {
            if (!gUSBHostU3VAttachListener[iterator].inUse)
            {
                attachListener = &gUSBHostU3VAttachListener[iterator];
                attachListener->inUse = true;
                attachListener->eventHandler = eventHandler;
                attachListener->context = context;
                result = U3V_HOST_RESULT_SUCCESS;
                break;
            }
        }
    }

    return result;
}


T_U3VHostHandle U3VHost_Open(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostHandle result = U3V_HOST_HANDLE_INVALID;
    T_U3VHostInstanceObj *u3vInstance;

    /* Single client only */
    if (u3vObjHandle != 0UL)
    {
        u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
        if ((u3vInstance->inUse) && (u3vInstance->state == U3V_HOST_STATE_READY))
        {
            result = (T_U3VHostHandle)(u3vObjHandle);
        }
    }

    return result;
}


T_U3VHostResult U3VHost_EventHandlerSet(T_U3VHostHandle handle, T_U3VHostEventHandler eventHandler, uintptr_t context)
{
    T_U3VHostResult result = U3V_HOST_RESULT_HANDLE_INVALID;
    T_U3VHostInstanceObj *u3vInstance = (T_U3VHostInstanceObj *)(handle);

    if (u3vInstance != NULL)
    {
        u3vInstance->eventHandler = eventHandler;
        u3vInstance->context = context;
        result = U3V_HOST_RESULT_SUCCESS;
    }

    return result;
}


T_U3VHostResult U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle, T_U3VHostDetachEventHandler detachEventHandler, uintptr_t context)
{
    T_U3VHostResult result = U3V_HOST_RESULT_HANDLE_INVALID;
    T_U3VHostInstanceObj *u3vInstance = (T_U3VHostInstanceObj *)(handle);

    if (u3vInstance != NULL)
    {
        u3vInstance->detachEventHandler = detachEventHandler;
        u3vInstance->context = context;
        result = U3V_HOST_RESULT_SUCCESS;
    }

    return result;
}


T_U3VHostResult U3VHost_GetStreamCapabilities(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint64_t sbrmAddress;
    uint64_t u3vCapability;
    uint64_t sirmAddress;
    uint32_t siInfo;
    uint32_t deviceByteAlignment;

    u3vResult = (u3vObjHandle == 0U) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         U3V_ABRM_SBRM_ADDRESS_OFS,
                                         sizeof(sbrmAddress),
                                         &bytesRead,
                                         (void *)&sbrmAddress);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sbrmAddress + U3V_SBRM_U3VCP_CAPABILITY_OFS,
                                         sizeof(u3vCapability),
                                         &bytesRead,
                                         (void *)&u3vCapability);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    if (u3vCapability & U3V_SIRM_AVAILABLE_MASK)
    {
        u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                             NULL,
                                             sbrmAddress + U3V_SBRM_SIRM_ADDRESS_OFS,
                                             sizeof(sirmAddress),
                                             &bytesRead,
                                             (void *)&sirmAddress);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            return u3vResult;
        }
        else
        {
            u3vInstance->u3vDevInfo.sirmAddr = sirmAddress;
        }

        u3vInstance->u3vDevInfo.hostByteAlignment = U3V_TARGET_ARCH_BYTE_ALIGNMENT;

        u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                             NULL,
                                             sirmAddress + U3V_SIRM_INFO_OFS,
                                             sizeof(siInfo),
                                             &bytesRead,
                                             (void *)&siInfo);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            return u3vResult;
        }
        else
        {
            deviceByteAlignment = 1U << ((siInfo & U3V_SIRM_INFO_ALIGNMENT_MASK) >> U3V_SIRM_INFO_ALIGNMENT_SHIFT);
            u3vInstance->u3vDevInfo.transferAlignment = U3VHost_LCMui32(deviceByteAlignment, u3vInstance->u3vDevInfo.hostByteAlignment);
        }
        
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_ReadMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t *pReadValue)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint32_t regValue;
    uint64_t regAddr;

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (u3vObjHandle == 0U)     ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (integerReg == 0U)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (pReadValue == NULL)     ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (integerReg)
        {
            case U3V_MEM_REG_INT_PIXELFORMAT:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.colorCodingID_Reg;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, (void *)&regValue);
                /* value is stored on high byte (bits 24 to 31) */
                regValue = (regValue >> 24U) & 0xFFUL;
                break;

            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.payloadSizeVal_Reg;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, (void *)&regValue);
                /* redundant assignment */
                regValue = regValue;
                break;

            case U3V_MEM_REG_INT_ACQUISITION_MODE:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.acquisitionMode_Reg;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, (void *)&regValue);
                /* redundant assignment */
                regValue = regValue;
                break;

            case U3V_MEM_REG_INT_DEVICE_RESET:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.deviceReset_Reg;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, (void *)&regValue);
                /* value is stored in MSB (bit 31) */
                regValue = (regValue >> 31U) & 0xFFUL;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            if (bytesRead == sizeof(regValue))
            {
                *pReadValue = regValue;
            }
            else
            {
                u3vResult = U3V_HOST_RESULT_ABORTED;
            }
        }
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_WriteMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t regVal)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesWritten;
    uint32_t regValue;
    uint64_t regAddr;

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (u3vObjHandle == 0U)     ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (integerReg == 0U)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (integerReg)
        {
            case U3V_MEM_REG_INT_PIXELFORMAT:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.colorCodingID_Reg;
                /* value is stored on high byte (bits 24 to 31) */
                regValue = (regVal << 24U) & 0xFF000000UL;
                break;

            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.payloadSizeVal_Reg;
                regValue = regVal;
                break;

            case U3V_MEM_REG_INT_ACQUISITION_MODE:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.acquisitionMode_Reg;
                regValue = regVal;
                break;

            case U3V_MEM_REG_INT_DEVICE_RESET:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.deviceReset_Reg;
                regValue = regVal;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesWritten, (void *)&regValue);
            if (bytesWritten != sizeof(regValue))
            {
                u3vResult = U3V_HOST_RESULT_ABORTED;
            }
        }
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_ReadMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat floatReg, float *const pReadValue)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint32_t regValue;
    float floatRetVal;
    uint64_t regAddr;

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (u3vObjHandle == 0U)     ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (floatReg == 0U)         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (pReadValue == NULL)     ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (floatReg)
        {
            case U3V_MEM_REG_FLOAT_TEMPERATURE:
                regAddr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.temperature_Reg;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, (void *)&regValue);
                /* Calculate temperature in Celcius */
                floatRetVal = ((float)(regValue & 0xFFFUL) / 10.0F) - 273.15F;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            if (bytesRead = sizeof(regValue))
            {
                *pReadValue = floatRetVal;
            }
            else
            {
                u3vResult = U3V_HOST_RESULT_ABORTED;
            }
        }
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_ReadMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString stringReg, void *pReadBfr)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint32_t stringSize;
    uint8_t stringBfr[U3V_REG_MANUFACTURER_NAME_SIZE];
    uint64_t regAddr;

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (u3vObjHandle == 0U)     ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (stringReg == 0U)        ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (pReadBfr == NULL)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (stringReg)
        {            
            case U3V_MEM_REG_STRING_MANUFACTURER_NAME:
                regAddr = U3V_ABRM_MANUFACTURER_NAME_OFS;
                stringSize = U3V_REG_MANUFACTURER_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_MODEL_NAME:
                regAddr = U3V_ABRM_MODEL_NAME_OFS;
                stringSize = U3V_REG_MODEL_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_FAMILY_NAME:
                regAddr = U3V_ABRM_FAMILY_NAME_OFS;
                stringSize = U3V_REG_FAMILY_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_DEVICE_VERSION:
                regAddr = U3V_ABRM_DEVICE_VERSION_OFS;
                stringSize = U3V_REG_DEVICE_VERSION_SIZE;
                break;

            case U3V_MEM_REG_STRING_MANUFACTURER_INFO:
                regAddr = U3V_ABRM_MANUFACTURER_INFO_OFS;
                stringSize = U3V_REG_MANUFACTURER_INFO_SIZE;
                break;

            case U3V_MEM_REG_STRING_SERIAL_NUMBER:
                regAddr = U3V_ABRM_SERIAL_NUMBER_OFS;
                stringSize = U3V_REG_SERIAL_NUMBER_SIZE;
                break;

            case U3V_MEM_REG_STRING_USER_DEFINED_NAME:
                regAddr = U3V_ABRM_USER_DEFINED_NAME_OFS;
                stringSize = U3V_REG_USER_DEFINED_NAME_SIZE;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, stringSize, &bytesRead, (void *)stringBfr);

            if (u3vResult == U3V_HOST_RESULT_SUCCESS)
            {
                if (bytesRead = stringSize)
                {
                    memcpy((void *)pReadBfr, (void *)stringBfr, stringSize);
                }
                else
                {
                    u3vResult = U3V_HOST_RESULT_ABORTED;
                }
            }
        }
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_AcquisitionStart(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesWritten;
    uint64_t acqStartRegAdr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.acquisitionStart_Reg;
    uint32_t acqStartCmdVal = U3V_ACQ_START_CMD_VAL;

    u3vResult = (u3vObjHandle == 0U) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance, NULL, acqStartRegAdr, sizeof(acqStartCmdVal), &bytesWritten, (void *)&acqStartCmdVal);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_AcquisitionStop(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesWritten;
    uint64_t acqStopRegAdr = u3vCamRegisterCfg.camRegBaseAddress + u3vCamRegisterCfg.acquisitionStop_Reg;
    uint32_t acqStopCmdVal = U3V_ACQ_STOP_CMD_VAL;

    u3vResult = (u3vObjHandle == 0U) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance, NULL, acqStopRegAdr, sizeof(acqStopCmdVal), &bytesWritten, (void *)&acqStopCmdVal);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_SetupStreamIfTransfer(T_U3VHostHandle u3vObjHandle, uint32_t imgPayloadSize)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t siRequiredLeaderSize;
    uint64_t siRequiredPayloadSize;
    uint32_t siRequiredTrailerSize;
    uint32_t bytesRead;

    u3vResult = (u3vObjHandle == 0U)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (imgPayloadSize == 0UL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    uint64_t sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    /* image payload size, never overflows 32bit unsigned int, usually a few MBs */
    uint32_t u32ImageSize = imgPayloadSize;
    uint32_t siMaxLeaderSize = (uint32_t)U3V_LEADER_MAX_SIZE;
    uint32_t siMaxTrailerSize = (uint32_t)U3V_TRAILER_MAX_SIZE;
    /* transfer size is the size of each payload block */
    uint32_t siPayloadTransfSize = U3V_IN_BUFFER_MAX_SIZE;
    /* transfer count is the total count of payload blocks, minus the transf1 & transf2 */
    uint32_t siPayloadTransfCount = u32ImageSize / siPayloadTransfSize;
    /* transfer1 size is the remainder of the total payload with padding of U3V_TARGET_ARCH_BYTE_ALIGNMENT, sent as an extra payload block */
    uint32_t siPayloadFinalTransf1Size = ((u32ImageSize % siPayloadTransfSize) / U3V_TARGET_ARCH_BYTE_ALIGNMENT) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    /* transfer2 size is the remainder of the transfer1 block payload, padded to U3V_TARGET_ARCH_BYTE_ALIGNMENT and rounded up, sent (if > 0) as the final payload block */
    uint32_t siPayloadFinalTransf2Size = u32ImageSize - siPayloadTransfCount * siPayloadTransfSize - siPayloadFinalTransf1Size;
    /* if transfer2 padding is not an integer, round up to padding */
    if ((siPayloadFinalTransf2Size % U3V_TARGET_ARCH_BYTE_ALIGNMENT) > 0UL)
    {
        siPayloadFinalTransf2Size = ((siPayloadFinalTransf2Size / U3V_TARGET_ARCH_BYTE_ALIGNMENT) + 1UL) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + U3V_SIRM_REQ_LEADER_SIZE_OFS,
                                         sizeof(siRequiredLeaderSize),
                                         &bytesRead,
                                         (void *)&siRequiredLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + U3V_SIRM_REQ_PAYLOAD_SIZE_OFS,
                                         sizeof(siRequiredPayloadSize),
                                         &bytesRead,
                                         (void *)&siRequiredPayloadSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + U3V_SIRM_REQ_TRAILER_SIZE_OFS,
                                         sizeof(siRequiredTrailerSize),
                                         &bytesRead,
                                         (void *)&siRequiredTrailerSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    if (((uint32_t)siRequiredPayloadSize != u32ImageSize) || /* casting fron 64bit to 32bit does not lead to loss of data, size is a few thousand blocks normally */
        (siRequiredLeaderSize > siMaxLeaderSize) ||
        (siRequiredTrailerSize > siMaxTrailerSize))
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_MAX_LEADER_SIZE_OFS,
                                          sizeof(siMaxLeaderSize),
                                          &bytesRead,
                                          (void *)&siMaxLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_MAX_TRAILER_SIZE_OFS,
                                          sizeof(siMaxTrailerSize),
                                          &bytesRead,
                                          (void *)&siMaxTrailerSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_PAYLOAD_SIZE_OFS,
                                          sizeof(siPayloadTransfSize),
                                          &bytesRead,
                                          (void *)&siPayloadTransfSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_PAYLOAD_COUNT_OFS,
                                          sizeof(siPayloadTransfCount),
                                          &bytesRead,
                                          (void *)&siPayloadTransfCount);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_TRANSFER1_SIZE_OFS,
                                          sizeof(siPayloadFinalTransf1Size),
                                          &bytesRead,
                                          (void *)&siPayloadFinalTransf1Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_TRANSFER2_SIZE_OFS,
                                          sizeof(siPayloadFinalTransf2Size),
                                          &bytesRead,
                                          (void *)&siPayloadFinalTransf2Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_StreamChControl(T_U3VHostHandle u3vObjHandle, bool enable)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;

    u3vResult = (u3vObjHandle == 0U)   ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL)    ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    uint64_t sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    uint32_t siControlCmd = (enable) ? U3V_SI_CTRL_CMD_ENABLE : U3V_SI_CTRL_CMD_DISABLE;

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_CONTROL_OFS,
                                          sizeof(siControlCmd),
                                          &bytesRead,
                                          (void *)&siControlCmd);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_StartImgPayldTransfer(T_U3VHostHandle u3vObjHandle, void *imgBfr, size_t size)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    T_U3VHostTransferHandle tempTransferHandle;

    u3vResult = (u3vObjHandle == 0U)    ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (imgBfr == NULL)        ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (size == 0U)            ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (ctrlIfInstance == NULL)    ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    hostResult = USB_HOST_DeviceTransfer(u3vInstance->streamIfHandle.bulkInPipeHandle,
                                         &tempTransferHandle,
                                         imgBfr,
                                         size,
                                         (uintptr_t)U3V_HOST_EVENT_IMG_PLD_RECEIVED);
    u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

    return u3vResult;
}


T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    uint64_t sbrmAddress;
    uint32_t bytesRead;
    uint32_t maxResponse;
    uint32_t cmdBfrSize;
    uint32_t ackBfrSize;

    u3vResult = (u3vObjHandle == 0UL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = &u3vInstance->controlIfObj;

    if(OSAL_MUTEX_Create(&(ctrlIfInst->readWriteLock)) != OSAL_RESULT_TRUE)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    ctrlIfInst->transfReqCompleteCbk = U3VHost_CtrlIfTransferReqCompleteCbk;

    ctrlIfInst->u3vTimeout = U3V_REQ_TIMEOUT_MS;
    ctrlIfInst->maxAckTransfSize = sizeof(ctrlIfInst->ackBuffer);
    ctrlIfInst->maxCmdTransfSize = sizeof(ctrlIfInst->cmdBuffer);

    /* requestId, maxRequestId are preincremented, with overflow the unsigned will start again from 0 */
    ctrlIfInst->requestId = 0xFFFFU;
    ctrlIfInst->maxRequestId = 0xFFFFU;
    ctrlIfInst->ctrlIntfHandle = &u3vInstance->controlIfHandle;

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                         U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                         &bytesRead,
                                         &maxResponse);

    u3vResult = (bytesRead != U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }
    
    ctrlIfInst->u3vTimeout = U3VDRV_MAX(U3V_REQ_TIMEOUT_MS, maxResponse);

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         U3V_ABRM_SBRM_ADDRESS_OFS,
                                         U3V_REG_SBRM_ADDRESS_SIZE,
                                         &bytesRead,
                                         &sbrmAddress);

    u3vResult = (bytesRead != U3V_REG_SBRM_ADDRESS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         sbrmAddress + U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                         sizeof(uint32_t),
                                         &bytesRead,
                                         &cmdBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->maxCmdTransfSize = U3VDRV_MIN(ctrlIfInst->maxCmdTransfSize, cmdBfrSize);

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         sbrmAddress + U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                         sizeof(uint32_t),
                                         &bytesRead,
                                         &ackBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->maxAckTransfSize = U3VDRV_MIN(ctrlIfInst->maxAckTransfSize, ackBfrSize);
    
    return u3vResult;
}


void U3VHost_CtrlIf_InterfaceDestroy(T_U3VHostHandle u3vObjHandle)
{
    T_U3VControlIfObj *u3vCtrlIntfObj = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;

    if (u3vObjHandle != 0UL)
    {
        u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
        u3vCtrlIntfObj = (T_U3VControlIfObj *)(&u3vInstance->controlIfObj);
        U3VHost_CtrlIfClearObjData(u3vCtrlIntfObj);
    }
    else
    {
        /* invalid handle */
    }
}


uint32_t U3VHost_GetSelectedPixelFormat(void)
{
    return u3vCamRegisterCfg.pixelFormatCtrlVal_Int_Sel;
}


/********************************************************
* Local function definitions
*********************************************************/

static void U3VHost_Initialize(void * data)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;

    for (uint32_t iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        /* Set the pipes handles to invalid */
        u3vInstance = &gUSBHostU3VObj[iterator];

        u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
        u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->controlPipeHandle = USB_HOST_CONTROL_PIPE_HANDLE_INVALID;

        u3vInstance->controlIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;

        u3vInstance->eventIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstance->eventIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;   /* N/A */

        u3vInstance->streamIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstance->streamIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;  /* N/A */

    }
}


static void U3VHost_Deinitialize(void)
{
    /* N/A */
}


static void U3VHost_Reinitialize(void * msdInitData)
{
    /* N/A */
}


static void U3VHost_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
                                    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                    size_t nInterfaces,
                                    uint8_t *descriptor)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;
    int32_t u3vInstanceIndex;
    uint32_t iterator;
    USB_INTERFACE_DESCRIPTOR *interfaceDescriptor;
    USB_ENDPOINT_DESCRIPTOR *endpointDescriptor;
    USB_HOST_ENDPOINT_DESCRIPTOR_QUERY endpointDescriptorQuery;
    USB_HOST_INTERFACE_DESCRIPTOR_QUERY interfaceDescriptorQuery;

    /* This is an IAD, the expected number of interfaces is more than one for a U3V device (e.g. Control, Event, Streaming) */
    if (nInterfaces > 1UL) 
    {
        for (iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
        {
            if (!gUSBHostU3VObj[iterator].inUse)
            {
                u3vInstance = &gUSBHostU3VObj[iterator];
                u3vInstance->inUse = true;
                break;
            }
        }

        if (u3vInstance == NULL)
        {
            for (iterator = 0UL; iterator < nInterfaces; iterator++)
            {
                USB_HOST_DeviceInterfaceRelease(interfaces[iterator]);
            }
        }
        else
        {
            u3vInstance->deviceObjHandle = deviceObjHandle;
            u3vInstance->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);

            USB_HOST_DeviceInterfaceQueryContextClear(&interfaceDescriptorQuery);
            interfaceDescriptorQuery.flags = USB_HOST_INTERFACE_QUERY_ANY;

            for (iterator = 0UL; iterator < nInterfaces; iterator++)
            {
                interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)(descriptor),
                                                                                     &interfaceDescriptorQuery);

                if (interfaceDescriptor != NULL)
                {
                    /* Control Interface */
                    if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                        (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                        (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_CONTROL) &&
                        (interfaceDescriptor->bDescriptorType == U3V_DESCRIPTOR_TYPE_INTERFACE))
                    {
                        u3vInstance->controlIfHandle.idNum = interfaceDescriptor->bInterfaceNumber;
                        u3vInstance->controlIfHandle.ifHandle = interfaces[iterator];

                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE | USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->controlIfHandle.ifHandle,
                                                                                                    endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* Get the bulk out endpoint */
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_HOST_TO_DEVICE;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE | USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->controlIfHandle.ifHandle,
                                                                                                     endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make the pipe handle invalid */
                            u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                    }
                    /* Event Interface*/
                    else if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                             (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                             (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_EVENT) &&
                             (interfaceDescriptor->bDescriptorType == U3V_DESCRIPTOR_TYPE_INTERFACE))
                    {
                        u3vInstance->eventIfHandle.idNum = interfaceDescriptor->bInterfaceNumber;
                        u3vInstance->eventIfHandle.ifHandle = interfaces[iterator];

                        /* Get the bulk in endpoint */ 
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->eventIfHandle.ifHandle,
                                                                                                  endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* no EP OUT in Event interface*/
                    }
                    /* Data Streaming Interface */
                    else if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                             (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                             (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_DATASTREAM) &&
                             (interfaceDescriptor->bDescriptorType == U3V_DESCRIPTOR_TYPE_INTERFACE))
                    {
                        u3vInstance->streamIfHandle.ifHandle = interfaces[iterator];
                        u3vInstance->streamIfHandle.idNum = interfaceDescriptor->bInterfaceNumber;

                        /* Get the bulk in endpoint */ 
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->streamIfHandle.ifHandle,
                                                                                                   endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* no EP OUT in Streaming interface*/
                    }
                    
                }
                else
                {
                    /* There have to be at least two interface descriptors */
                }
            }

            if ((u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
            {
                u3vInstance->state = U3V_HOST_STATE_READY;

                for (iterator = 0UL; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
                {
                    if (gUSBHostU3VAttachListener[iterator].inUse)
                    {
                        gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostHandle)u3vInstance,
                                                                         gUSBHostU3VAttachListener[iterator].context);
                    }
                }
            }
            else
            {
                u3vInstance->state = U3V_HOST_STATE_ERROR;
            }
        }
    }
    else
    {
        /* nInterfaces do not match to a U3V device */
    }
}


static void U3VHost_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    int32_t u3vIndex = U3VHost_InterfaceHandleToInstance(interfaceHandle);

    if (u3vIndex >= 0U)
    {
        u3vInstance = &gUSBHostU3VObj[u3vIndex];
        
        if(u3vInstance->inUse)
        {
            u3vInstance->inUse = false;

            if(u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkInPipeHandle);
                u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->controlIfHandle.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkOutPipeHandle);
                u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                USB_HOST_DevicePipeClose(u3vInstance->eventIfHandle.bulkInPipeHandle);
                u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }
            
            if(u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                USB_HOST_DevicePipeClose(u3vInstance->streamIfHandle.bulkInPipeHandle);
                u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->detachEventHandler != NULL)
            {
                u3vInstance->detachEventHandler((T_U3VHostHandle)(u3vInstance), u3vInstance->context);
            }

            u3vInstance->detachEventHandler = NULL;
            u3vInstance->eventHandler = NULL;
            u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
            u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
        }
    }
}


static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE U3VHost_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                              USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                              void *eventData,
                                                                              uintptr_t context)
{
    T_U3VHostInstanceObj *u3vInstance;
    int32_t u3vIndex = U3VHost_InterfaceHandleToInstance(interfaceHandle);
    USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *dataTransferEvent;
    T_U3VHostEvent u3vEvent = (T_U3VHostEvent)(context);
    T_U3VHostEventWriteCompleteData u3vTransferCompleteData;
 
    u3vInstance = &gUSBHostU3VObj[u3vIndex];

    switch (event)
    {
        case USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE:
            dataTransferEvent = (USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *)(eventData);
            u3vTransferCompleteData.transferHandle = dataTransferEvent->transferHandle;
            u3vTransferCompleteData.result = U3VHost_HostToU3VResultsMap(dataTransferEvent->result);
            u3vTransferCompleteData.length = dataTransferEvent->length;

            /* update Control IF transf status indicators */
            if (u3vInstance->controlIfObj.transfReqCompleteCbk != NULL)
            {
                u3vInstance->controlIfObj.transfReqCompleteCbk((T_U3VHostHandle)u3vInstance, u3vEvent, &u3vTransferCompleteData);
            }

            if (u3vInstance->eventHandler != NULL)
            {
                u3vInstance->eventHandler((T_U3VHostHandle)u3vInstance, u3vEvent, &u3vTransferCompleteData, u3vInstance->context);
            }
            break;

        default:
            break;
    }

    return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE;
}


static void U3VHost_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    /* N/A */
}


static USB_HOST_DEVICE_EVENT_RESPONSE U3VHost_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                 USB_HOST_DEVICE_EVENT event,
                                                                 void *eventData,
                                                                 uintptr_t context)
{
   T_U3VHostInstanceObj *u3vInstance = (T_U3VHostInstanceObj *)(context);
   USB_HOST_DEVICE_EVENT_CONFIGURATION_SET_DATA *configSetEventData;

   switch (event)
   {
        case USB_HOST_DEVICE_EVENT_CONFIGURATION_SET:
            configSetEventData = (USB_HOST_DEVICE_EVENT_CONFIGURATION_SET_DATA *)(eventData);
            u3vInstance->hostRequestResult = configSetEventData->result;
            u3vInstance->hostRequestDone = true;
            break;
        
        case USB_HOST_DEVICE_EVENT_CONFIGURATION_DESCRIPTOR_GET_COMPLETE:
            break;
        
        default:
            break;
    }

    return USB_HOST_DEVICE_EVENT_RESPONSE_NONE;
}


static void U3VHost_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                 USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                 USB_DEVICE_DESCRIPTOR *deviceDescriptor)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;

    for (uint32_t iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (!gUSBHostU3VObj[iterator].inUse)
        {
            u3vInstance = &gUSBHostU3VObj[iterator];
            u3vInstance->inUse = true;
            u3vInstance->deviceObjHandle = deviceObjHandle;
            u3vInstance->deviceClientHandle = deviceHandle;
            break;
        }
    }

    if(u3vInstance == NULL)
    {
        USB_HOST_DeviceRelease(deviceHandle);
    }
    else
    {
        if(deviceDescriptor->bNumConfigurations > 0U)
        {
            u3vInstance->state = U3V_HOST_STATE_SET_CONFIGURATION;
            u3vInstance->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);
        }
        else
        {
            u3vInstance->state = U3V_HOST_STATE_ERROR;
        }
    }
}


static void U3VHost_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    int32_t index = U3VHost_DeviceHandleToInstance(deviceHandle);

    if (index >= 0U)
    {
        u3vInstance = &gUSBHostU3VObj[index];
        u3vInstance->inUse = false;

        if (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkInPipeHandle);
            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->controlIfHandle.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkOutPipeHandle);
            u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            USB_HOST_DevicePipeClose(u3vInstance->eventIfHandle.bulkInPipeHandle);
            u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            USB_HOST_DevicePipeClose(u3vInstance->streamIfHandle.bulkInPipeHandle);
            u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->detachEventHandler != NULL)
        {
            u3vInstance->detachEventHandler((T_U3VHostHandle)(u3vInstance),
                                            u3vInstance->context);
        }

        u3vInstance->detachEventHandler = NULL;
        u3vInstance->eventHandler = NULL;
        u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
    }
}


static void U3VHost_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    int32_t index = U3VHost_DeviceHandleToInstance(deviceHandle);
    USB_HOST_RESULT result;
    USB_HOST_REQUEST_HANDLE requestHandle;

   if (index >= 0)
    {
        u3vInstance = &gUSBHostU3VObj[index];

        if (u3vInstance->inUse)
        {
            switch (u3vInstance->state)
            {
                case U3V_HOST_STATE_NOT_READY:
                    break;

                case U3V_HOST_STATE_SET_CONFIGURATION:
                    u3vInstance->hostRequestDone = false;
                    result = USB_HOST_DeviceConfigurationSet(u3vInstance->deviceClientHandle, &requestHandle, 0U, (uintptr_t)(u3vInstance));
                    if (result == USB_HOST_RESULT_SUCCESS)
                    {
                        u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET;
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET:
                    if (u3vInstance->hostRequestDone == true)
                    {
                        if (u3vInstance->hostRequestResult == USB_HOST_RESULT_SUCCESS)
                        {
                            u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_INTERFACES;
                        }
                        else
                        {
                            u3vInstance->state = U3V_HOST_STATE_ERROR;
                        }
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_INTERFACES:
                    if ((u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
                    {
                        u3vInstance->state = U3V_HOST_STATE_READY;
                        for (uint32_t iterator = 0UL; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
                        {
                            if (gUSBHostU3VAttachListener[iterator].inUse)
                            {
                                gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostHandle)u3vInstance,
                                                                                 gUSBHostU3VAttachListener[iterator].context);
                            }
                        }
                    }
                    break;

                case U3V_HOST_STATE_READY:
                    break;

                case U3V_HOST_STATE_ERROR:
                default:
                    break;
            }
        }
    }
}


static int32_t U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle)
{
    int32_t result = -1L;

    for (uint32_t iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceClientHandle == deviceClientHandle)
        {
            result = (int32_t)iterator;
            break;
        }
    }

    return result;
}


static int32_t U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle)
{
    int32_t result = -1L;

    for (uint32_t iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceObjHandle == deviceObjHandle)
        {
            result = (int32_t)iterator;
            break;
        }
    }

    return result;
}


static int32_t U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    int32_t result = -1L;

    for (uint32_t iterator = 0UL; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if ((gUSBHostU3VObj[iterator].controlIfHandle.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].eventIfHandle.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].streamIfHandle.ifHandle == interfaceHandle))
        {
            result = (int32_t)iterator;
            break;
        }
    }

    return result;
}


static T_U3VHostResult U3VHost_HostToU3VResultsMap(USB_HOST_RESULT hostResult)
{
    T_U3VHostResult u3vResult;

    switch (hostResult)
    {
        case USB_HOST_RESULT_SUCCESS:
            u3vResult = U3V_HOST_RESULT_SUCCESS;
            break;

        case USB_HOST_RESULT_FAILURE:
        case USB_HOST_RESULT_PARAMETER_INVALID:
        case USB_HOST_RESULT_PIPE_HANDLE_INVALID:
            u3vResult = U3V_HOST_RESULT_FAILURE;
            break;

        case USB_HOST_RESULT_REQUEST_BUSY:
            u3vResult = U3V_HOST_RESULT_BUSY;
            break;

        case USB_HOST_RESULT_REQUEST_STALLED:
            u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
            break;

        case USB_HOST_RESULT_TRANSFER_ABORTED:
            u3vResult = U3V_HOST_RESULT_ABORTED;
            break;

        default:
            u3vResult = U3V_HOST_RESULT_FAILURE;
            break;
    }

    return u3vResult;
}


static T_U3VHostResult U3VHost_CtrlIfReadMemory(T_U3VControlIfObj *u3vCtrlIf,
                                                T_U3VHostTransferHandle *transferHandle,
                                                uint64_t memAddress,
                                                size_t transfSize,
                                                uint32_t *bytesRead,
                                                void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInterfHandle *ctrlIfHandle = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerRead = 0UL;
    uint32_t totalBytesRead = 0UL;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + transfSize;

    u3vResult = (u3vCtrlIf == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (bytesRead == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer == NULL)    ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize == 0UL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = u3vCtrlIf;
    maxBytesPerRead = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize)                    ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead == 0UL)                                          ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesRead = 0UL;

    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }
    
    while (totalBytesRead < transfSize)
    {
        uint32_t bytesThisIteration = U3VDRV_MIN((uint32_t)(transfSize - totalBytesRead), maxBytesPerRead);
        T_U3VCtrlIfCommand *command = (T_U3VCtrlIfCommand *)(ctrlIfInst->cmdBuffer);
        T_U3VCtrlIfReadMemCmdPayload *payload = (T_U3VCtrlIfReadMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0UL;

        ctrlIfInst->writeReqSts.length = 0UL;
        ctrlIfInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlIfInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
        command->header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        command->header.cmd    = (uint16_t)(U3V_CTRL_READMEM_CMD);
        command->header.length = (uint16_t)sizeof(T_U3VCtrlIfReadMemCmdPayload);

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
        payload->address = memAddress + (uint64_t)totalBytesRead;
        payload->reserved = 0U;
        payload->byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlIfInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlIfInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlIfInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if ((writeRetryCnt * 10UL) > (ctrlIfInst->u3vTimeout))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0UL;

            ctrlIfInst->readReqSts.length = 0UL;
            ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlIfInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + U3VDRV_MAX((size_t)(bytesThisIteration), sizeof(T_U3VCtrlIfPendingAckPayload));
            memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack->header.prefix != U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if ((readRetryCnt * 10UL) > (ctrlIfInst->u3vTimeout))
                {
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
            if (ack->header.cmd == U3V_CTRL_PENDING_ACK)
            {
                pendingAck = (T_U3VCtrlIfPendingAckPayload *)(ack->payload);
                ctrlIfInst->u3vTimeout = U3VDRV_MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }

            /* Acknowledge received successfully */
            reqAcknowledged = true;

        } /* loop until reqAcknowledged == true */
        totalBytesRead += bytesThisIteration;
    } /* loop until totalBytesRead == transfSize */

    if (totalBytesRead != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* Extract the data */
	memcpy(buffer, ack->payload, totalBytesRead);
    *bytesRead = totalBytesRead;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


static T_U3VHostResult U3VHost_CtrlIfWriteMemory(T_U3VControlIfObj *u3vCtrlIf,
                                                 T_U3VHostTransferHandle *transferHandle,
                                                 uint64_t memAddress,
                                                 size_t transfSize,
                                                 uint32_t *bytesWritten,
                                                 const void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInterfHandle *ctrlIfHandle = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerWrite = 0UL;
    uint32_t totalBytesWritten = 0UL;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    T_U3V_CtrlIfWriteMemAckPayload *writeMemAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + U3VDRV_MAX(sizeof(T_U3V_CtrlIfWriteMemAckPayload), sizeof(T_U3VCtrlIfPendingAckPayload));

    u3vResult = (u3vCtrlIf == NULL)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (bytesWritten == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer == NULL)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize == 0UL)    ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    
    ctrlIfInst = u3vCtrlIf;
    maxBytesPerWrite = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize)                    ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite == 0UL)                                         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesWritten = 0UL;
 
    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < transfSize)
    {
        uint32_t bytesThisIteration = U3VDRV_MIN((uint32_t)(transfSize - totalBytesWritten), maxBytesPerWrite);
        T_U3VCtrlIfCommand *command = (T_U3VCtrlIfCommand *)(ctrlIfInst->cmdBuffer);
        T_U3VCtrlIfWriteMemCmdPayload *payload = (T_U3VCtrlIfWriteMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0UL;

        ctrlIfInst->writeReqSts.length = 0UL;
        ctrlIfInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlIfInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
		command->header.flags = (uint16_t)(U3V_CTRL_REQ_ACK);
		command->header.cmd = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        command->header.length = (uint16_t)(sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration);

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
		payload->address = memAddress + (uint64_t)totalBytesWritten;
        cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration;

        memcpy(payload->data, (uint8_t *)(buffer + totalBytesWritten), bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlIfInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlIfInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlIfInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if ((writeRetryCnt * 10UL) > (ctrlIfInst->u3vTimeout))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0UL;

            ctrlIfInst->readReqSts.length = 0UL;
            ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlIfInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }
            
            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);
            writeMemAck = (T_U3V_CtrlIfWriteMemAckPayload *)(ack->payload);

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack->header.prefix != U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if ((readRetryCnt * 10UL) > (ctrlIfInst->u3vTimeout))
                {
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_WRITEMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length != sizeof(T_U3V_CtrlIfWriteMemAckPayload)) && (ack->header.length != 0U)) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length == sizeof(T_U3V_CtrlIfWriteMemAckPayload)) && (writeMemAck->bytesWritten != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
			if (ack->header.cmd == U3V_CTRL_PENDING_ACK) 
            {
                pendingAck = (T_U3VCtrlIfPendingAckPayload *)(ack->payload);
                ctrlIfInst->u3vTimeout = U3VDRV_MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }
            /* Acknowledge received successfully */
			reqAcknowledged = true;
        } /* loop until reqAcknowledged == true */
        totalBytesWritten += bytesThisIteration;
    } /* loop until totalBytesWritten == transfSize */

    if (totalBytesWritten != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    *bytesWritten = totalBytesWritten;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


static void U3VHost_CtrlIfTransferReqCompleteCbk(T_U3VHostHandle u3vHostHandle, T_U3VHostEvent transfEvent, void *transfData)
{
    T_U3VHostEventWriteCompleteData *writeCompleteEventData;
    T_U3VHostEventReadCompleteData  *readCompleteEventData;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VControlIfObj *ctrlIfInstance = NULL;
    u3vInstance = (T_U3VHostInstanceObj *)u3vHostHandle;

    if (u3vInstance != NULL)
    {
        ctrlIfInstance = &u3vInstance->controlIfObj;
        switch (transfEvent)
        {
            case U3V_HOST_EVENT_READ_COMPLETE:
                readCompleteEventData = (T_U3VHostEventReadCompleteData *)transfData;
                ctrlIfInstance->readReqSts = *readCompleteEventData;
                break;

            case U3V_HOST_EVENT_WRITE_COMPLETE:
                writeCompleteEventData = (T_U3VHostEventWriteCompleteData *)transfData;
                ctrlIfInstance->writeReqSts = *writeCompleteEventData;
                break;

            default:
                break;
        }
    }
}


static inline void U3VHost_CtrlIfClearObjData(T_U3VControlIfObj *pCtrlIfObj)
{
    memset(pCtrlIfObj, 0x0U, sizeof(T_U3VControlIfObj));
}


static inline uint32_t U3VHost_LCMui32(uint32_t n1, uint32_t n2)
{
    uint32_t lcm = (n1 > n2) ? n1 : n2;

    if (n1 == 0UL || n2 == 0UL)
    {
        lcm = 0UL;
    }
    else
    {
        while (lcm < 0xFFFFFFFFUL)
        {
            if (lcm % n1 == 0UL && lcm % n2 == 0UL)
            {
                break;
            }
            ++lcm;
        }
    }

    return lcm;
}

