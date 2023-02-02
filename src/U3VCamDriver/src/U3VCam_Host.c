
#include "U3VCam_Host.h"
#include "U3VCam_Host_Local.h"

#include "FreeRTOS.h"
#include "task.h"



/*******************************************************************************
* Local function declarations
*******************************************************************************/

static void U3VHost_Initialize(void *data);

static void U3VHost_Deinitialize(void);

static void U3VHost_Reinitialize(void *data);

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

static uint32_t U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle);

static uint32_t U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle);

static uint32_t U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

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

static inline uint32_t U3VHost_GCDu32(uint32_t n1, uint32_t n2);

static inline uint32_t U3VHost_LCMu32(uint32_t n1, uint32_t n2);


/*******************************************************************************
* Constant & Variable declarations
*******************************************************************************/

static T_U3VHostInstanceObj gUSBHostU3VObj[U3V_HOST_INSTANCES_NUMBER];

static T_U3VHostAttachListenerObj gUSBHostU3VAttachListener[U3V_HOST_ATTACH_LISTENERS_NUMBER];

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


/*******************************************************************************
* Function definitions
*******************************************************************************/

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
        for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
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
    if (u3vObjHandle != 0)
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
                                         (uint64_t)U3V_ABRM_SBRM_ADDRESS_OFS,
                                         sizeof(sbrmAddress),
                                         &bytesRead,
                                         &sbrmAddress);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sbrmAddress + (uint64_t)U3V_SBRM_U3VCP_CAPABILITY_OFS,
                                         sizeof(u3vCapability),
                                         &bytesRead,
                                         &u3vCapability);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    if (u3vCapability & U3V_SIRM_AVAILABLE_MASK)
    {
        u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                             NULL,
                                             sbrmAddress + (uint64_t)U3V_SBRM_SIRM_ADDRESS_OFS,
                                             sizeof(sirmAddress),
                                             &bytesRead,
                                             &sirmAddress);

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
                                             sirmAddress + (uint64_t)U3V_SIRM_INFO_OFS,
                                             sizeof(siInfo),
                                             &bytesRead,
                                             &siInfo);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            return u3vResult;
        }
        else
        {
            deviceByteAlignment = 1U << ((siInfo & U3V_SIRM_INFO_ALIGNMENT_MASK) >> U3V_SIRM_INFO_ALIGNMENT_SHIFT);
            u3vInstance->u3vDevInfo.transferAlignment = U3VHost_LCMu32(deviceByteAlignment, u3vInstance->u3vDevInfo.hostByteAlignment);
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
    u3vResult = (pReadValue == NULL)     ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (integerReg)
        {
            case U3V_MEM_REG_INT_ACQ_MODE:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_ACQ_MODE_REG_ADR;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, &regValue);
                regValue = U3V_GET_ACQ_MODE_CONV(regValue);
                break;

            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_PAYLOAD_SIZE_REG_ADR;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, &regValue);
                regValue = U3V_GET_PAYLOAD_SIZE_CONV(regValue);
                break;

            case U3V_MEM_REG_INT_PIXEL_FORMAT:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, &regValue);
                regValue = U3V_GET_PIXEL_FORMAT_CONV(regValue);
                break;

            /* N/A, fallthrough */
            case U3V_MEM_REG_INT_ACQ_START:
            case U3V_MEM_REG_INT_ACQ_STOP:
            case U3V_MEM_REG_INT_DEVICE_RESET:
            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            if (bytesRead == (uint32_t)sizeof(regValue))
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

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (integerReg)
        {
            case U3V_MEM_REG_INT_ACQ_MODE:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_ACQ_MODE_REG_ADR;
                regValue = U3V_SET_ACQ_MODE_CONV(regVal);
                break;

            case U3V_MEM_REG_INT_ACQ_START:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_ACQ_START_REG_ADR;
                regValue = U3V_SET_ACQ_START_CONV(regVal);
                break;

            case U3V_MEM_REG_INT_ACQ_STOP:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_ACQ_STOP_REG_ADR;
                regValue = U3V_SET_ACQ_STOP_CONV(regVal);
                break;

            case U3V_MEM_REG_INT_DEVICE_RESET:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_DEVICE_RESET_REG_ADR;
                regValue = U3V_SET_DEVICE_RESET_CONV(regVal);
                break;

            case U3V_MEM_REG_INT_PIXEL_FORMAT:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_PIXEL_FORMAT_REG_ADR;
                regValue = U3V_SET_PIXEL_FORMAT_CONV(regVal);
                break;

            /* N/A, fallthrough */
            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesWritten, &regValue);
            if (bytesWritten != (uint32_t)sizeof(regValue))
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
    u3vResult = (pReadValue == NULL)     ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (floatReg)
        {
            case U3V_MEM_REG_FLOAT_TEMPERATURE:
                regAddr = U3V_CAM_CFG_REG_BASE_ADR + U3V_CAM_CFG_TEMPERATURE_REG_ADR;
                u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, sizeof(regValue), &bytesRead, &regValue);
                floatRetVal = U3V_GET_TEMPERATURE_CONV(regValue);
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            if (bytesRead == (uint32_t)sizeof(regValue))
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
    size_t stringSize;
    T_U3VStringBuffer stringBfr;
    uint64_t regAddr;

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    ctrlIfInstance = &u3vInstance->controlIfObj;

    u3vResult = (u3vObjHandle == 0U)     ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (pReadBfr == NULL)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult == U3V_HOST_RESULT_SUCCESS)
    {
        switch (stringReg)
        {            
            case U3V_MEM_REG_STRING_MANUFACTURER_NAME:
                regAddr = (uint64_t)U3V_ABRM_MANUFACTURER_NAME_OFS;
                stringSize = (size_t)U3V_REG_MANUFACTURER_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_MODEL_NAME:
                regAddr = (uint64_t)U3V_ABRM_MODEL_NAME_OFS;
                stringSize = (size_t)U3V_REG_MODEL_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_FAMILY_NAME:
                regAddr = (uint64_t)U3V_ABRM_FAMILY_NAME_OFS;
                stringSize = (size_t)U3V_REG_FAMILY_NAME_SIZE;
                break;

            case U3V_MEM_REG_STRING_DEVICE_VERSION:
                regAddr = (uint64_t)U3V_ABRM_DEVICE_VERSION_OFS;
                stringSize = (size_t)U3V_REG_DEVICE_VERSION_SIZE;
                break;

            case U3V_MEM_REG_STRING_MANUFACTURER_INFO:
                regAddr = (uint64_t)U3V_ABRM_MANUFACTURER_INFO_OFS;
                stringSize = (size_t)U3V_REG_MANUFACTURER_INFO_SIZE;
                break;

            case U3V_MEM_REG_STRING_SERIAL_NUMBER:
                regAddr = (uint64_t)U3V_ABRM_SERIAL_NUMBER_OFS;
                stringSize = (size_t)U3V_REG_SERIAL_NUMBER_SIZE;
                break;

            case U3V_MEM_REG_STRING_USER_DEFINED_NAME:
                regAddr = (uint64_t)U3V_ABRM_USER_DEFINED_NAME_OFS;
                stringSize = (size_t)U3V_REG_USER_DEFINED_NAME_SIZE;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance, NULL, regAddr, stringSize, &bytesRead, stringBfr.asU8);

            if (u3vResult == U3V_HOST_RESULT_SUCCESS)
            {
                if (bytesRead == (uint32_t)stringSize)
                {
                    memcpy(pReadBfr, stringBfr.asChar, stringSize);
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

    u3vResult = (u3vObjHandle == 0)             ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (imgPayloadSize == UINT32_C(0)) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

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

    const uint64_t sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    /* image payload size, never overflows 32bit unsigned int, usually a few MBs */
    const uint32_t u32ImageSize = imgPayloadSize;
    const uint32_t siMaxLeaderSize = (uint32_t)U3V_LEADER_MAX_SIZE;
    const uint32_t siMaxTrailerSize = (uint32_t)U3V_TRAILER_MAX_SIZE;
    /* transfer size is the size of each payload block */
    const uint32_t siPayloadTransfSize = (uint32_t)U3V_PAYLD_BLOCK_MAX_SIZE;
    /* transfer count is the total count of payload blocks, minus the transf1 & transf2 */
    const uint32_t siPayloadTransfCount = u32ImageSize / siPayloadTransfSize;
    /* transfer1 size is the remainder of the total payload with padding of U3V_TARGET_ARCH_BYTE_ALIGNMENT, sent as an extra payload block */
    const uint32_t siPayloadFinalTransf1Size = ((u32ImageSize % siPayloadTransfSize) / U3V_TARGET_ARCH_BYTE_ALIGNMENT) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    /* transfer2 size is the remainder of the transfer1 block payload, padded to U3V_TARGET_ARCH_BYTE_ALIGNMENT and rounded up, sent (if > 0) as the final payload block */
    uint32_t siPayloadFinalTransf2Size = u32ImageSize - siPayloadTransfCount * siPayloadTransfSize - siPayloadFinalTransf1Size;
    /* if transfer2 padding is not an integer, round up to padding */
    if ((siPayloadFinalTransf2Size % U3V_TARGET_ARCH_BYTE_ALIGNMENT) > UINT32_C(0))
    {
        siPayloadFinalTransf2Size = ((siPayloadFinalTransf2Size / U3V_TARGET_ARCH_BYTE_ALIGNMENT) + UINT32_C(1)) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + (uint64_t)U3V_SIRM_REQ_LEADER_SIZE_OFS,
                                         sizeof(siRequiredLeaderSize),
                                         &bytesRead,
                                         &siRequiredLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + (uint64_t)U3V_SIRM_REQ_PAYLOAD_SIZE_OFS,
                                         sizeof(siRequiredPayloadSize),
                                         &bytesRead,
                                         &siRequiredPayloadSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInstance,
                                         NULL,
                                         sirmAddress + (uint64_t)U3V_SIRM_REQ_TRAILER_SIZE_OFS,
                                         sizeof(siRequiredTrailerSize),
                                         &bytesRead,
                                         &siRequiredTrailerSize);

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
                                          sirmAddress + (uint64_t)U3V_SIRM_MAX_LEADER_SIZE_OFS,
                                          sizeof(siMaxLeaderSize),
                                          &bytesRead,
                                          &siMaxLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_MAX_TRAILER_SIZE_OFS,
                                          sizeof(siMaxTrailerSize),
                                          &bytesRead,
                                          &siMaxTrailerSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_PAYLOAD_SIZE_OFS,
                                          sizeof(siPayloadTransfSize),
                                          &bytesRead,
                                          &siPayloadTransfSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_PAYLOAD_COUNT_OFS,
                                          sizeof(siPayloadTransfCount),
                                          &bytesRead,
                                          &siPayloadTransfCount);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_TRANSFER1_SIZE_OFS,
                                          sizeof(siPayloadFinalTransf1Size),
                                          &bytesRead,
                                          &siPayloadFinalTransf1Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_TRANSFER2_SIZE_OFS,
                                          sizeof(siPayloadFinalTransf2Size),
                                          &bytesRead,
                                          &siPayloadFinalTransf2Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_StreamIfControl(T_U3VHostHandle u3vObjHandle, bool enable)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;

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

    const uint64_t sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    const uint32_t siControlCmd = (enable) ? U3V_SI_CTRL_ENABLE_CMD : U3V_SI_CTRL_DISABLE_CMD;

    u3vResult = U3VHost_CtrlIfWriteMemory(ctrlIfInstance,
                                          NULL,
                                          sirmAddress + (uint64_t)U3V_SIRM_CONTROL_OFS,
                                          sizeof(siControlCmd),
                                          &bytesRead,
                                          &siControlCmd);

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

    u3vResult = (ctrlIfInstance == NULL) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
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

    u3vResult = (u3vObjHandle == 0) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

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

    ctrlIfInst->u3vTimeout = (uint32_t)U3V_REQ_TIMEOUT_MS;
    ctrlIfInst->maxAckTransfSize = (uint32_t)U3V_CTRL_IF_ACK_BUFFER_MAX_SIZE;
    ctrlIfInst->maxCmdTransfSize = (uint32_t)U3V_CTRL_IF_CMD_BUFFER_MAX_SIZE;

    /* requestId, maxRequestId are preincremented, with overflow the unsigned will start again from 0 */
    ctrlIfInst->requestId = UINT16_MAX;
    ctrlIfInst->maxRequestId = UINT16_MAX;
    ctrlIfInst->ctrlIntfHandle = &u3vInstance->controlIfHandle;

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         (uint64_t)U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                         (size_t)U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                         &bytesRead,
                                         &maxResponse);

    u3vResult = (bytesRead != (uint32_t)U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }
    
    ctrlIfInst->u3vTimeout = U3VDRV_MAX(U3V_REQ_TIMEOUT_MS, maxResponse);

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         (uint64_t)U3V_ABRM_SBRM_ADDRESS_OFS,
                                         (size_t)U3V_REG_SBRM_ADDRESS_SIZE,
                                         &bytesRead,
                                         &sbrmAddress);

    u3vResult = (bytesRead != (uint32_t)U3V_REG_SBRM_ADDRESS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         sbrmAddress + (uint64_t)U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                         sizeof(cmdBfrSize),
                                         &bytesRead,
                                         &cmdBfrSize);

    u3vResult = (bytesRead != (uint32_t)sizeof(cmdBfrSize)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        U3VHost_CtrlIfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->maxCmdTransfSize = U3VDRV_MIN(ctrlIfInst->maxCmdTransfSize, cmdBfrSize);

    u3vResult = U3VHost_CtrlIfReadMemory(ctrlIfInst,
                                         NULL,
                                         sbrmAddress + (uint64_t)U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                         sizeof(ackBfrSize),
                                         &bytesRead,
                                         &ackBfrSize);

    u3vResult = (bytesRead != (uint32_t)sizeof(ackBfrSize)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

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
    T_U3VHostInstanceObj *u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;

    if (u3vInstance != NULL)
    {
        u3vCtrlIntfObj = &u3vInstance->controlIfObj;
        U3VHost_CtrlIfClearObjData(u3vCtrlIntfObj);
    }
}


/*******************************************************************************
* Local function definitions
*******************************************************************************/

/**
 * U3V Host Initialization function.
 * 
 * Local function to initialize all gUSBHostU3VObj instances handles to default 
 * values, called when the USB Host layer initializes. Used as a callback by the
 * USB Host layer.
 * @param data      (TPL configuration data)
 */
static void U3VHost_Initialize(void * data)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;

    for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
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


/**
 * U3V Host Deinitialization function.
 * 
 * Local function to deinitialize local U3V Host data, called when USB Host 
 * deinitializes. Used as a callback by the USB Host layer.
 * @note Not applicable, used as a placeholder for gUSBHostU3VClientDriver.
 */
static void U3VHost_Deinitialize(void)
{
    /* N/A */
}


/**
 * U3V Host Reinitialization function.
 * 
 * Local function to reinitialize local U3V Host data, called when
 * USB Host layer deinitializes. Used as a callback by the USB Host layer.
 * @param data 
 * @note Not applicable, used as a placeholder for gUSBHostU3VClientDriver.
 */
static void U3VHost_Reinitialize(void * data)
{
    /* N/A */
}


/**
 * U3V Host Interface assignment function.
 *
 * Local function used to assign detected U3V interfaces from connected
 * device to local gUSBHostU3VObj object. Used as a callback by the USB Host
 * layer.
 * @param interfaces
 * @param deviceObjHandle
 * @param nInterfaces
 * @param descriptor
 * @note The expected interface types to assign should only be the 3 interfaces 
 * that the USB 3 Vision standard defines, the Control, Event and Streaming 
 * interfaces. The expected device type shall be an IAD (interface association 
 * device), therefore at least 2 interfaces must be detected in order for the 
 * function to assign them.
 */
static void U3VHost_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
                                    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                    size_t nInterfaces,
                                    uint8_t *descriptor)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;
    uint32_t iterator;
    USB_INTERFACE_DESCRIPTOR *interfaceDescriptor;
    USB_ENDPOINT_DESCRIPTOR *endpointDescriptor;
    USB_HOST_ENDPOINT_DESCRIPTOR_QUERY endpointDescriptorQuery;
    USB_HOST_INTERFACE_DESCRIPTOR_QUERY interfaceDescriptorQuery;

    /* This is an IAD, the expected number of interfaces is at least  *
     * 2 for a U3V device (e.g. Control, Event, Streaming interfaces) */
    if (nInterfaces > UINT32_C(1)) 
    {
        for (iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
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
            for (iterator = UINT32_C(0); iterator < nInterfaces; iterator++)
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

            for (iterator = UINT32_C(0); iterator < nInterfaces; iterator++)
            {
                interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery(descriptor, &interfaceDescriptorQuery);

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
            }

            if ((u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
            {
                u3vInstance->state = U3V_HOST_STATE_READY;

                for (iterator = UINT32_C(0); iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
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
}


/**
 * U3V Host Interface release function.
 *
 * Local function used to release assigned U3V interfaces of gUSBHostU3VObj
 * object. Used as a callback by the USB Host layer.
 * @param interfaceHandle
 */
static void U3VHost_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    const uint32_t index = U3VHost_InterfaceHandleToInstance(interfaceHandle);

    if (index < UINT32_MAX)
    {
        u3vInstance = &gUSBHostU3VObj[index];
        
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


/**
 * U3V Host Interface event handler function.
 * 
 * Local function used as an event handler, called by the USB Host layer when a 
 * transfer related event occurs. Used as a callback by the USB Host layer.
 * @param interfaceHandle 
 * @param event 
 * @param eventData 
 * @param context 
 * @return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE (return N/A)
 */
static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE U3VHost_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                              USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                              void *eventData,
                                                                              uintptr_t context)
{
    T_U3VHostInstanceObj *u3vInstance;
    const uint32_t index = U3VHost_InterfaceHandleToInstance(interfaceHandle);
    const T_U3VHostEvent u3vEvent = (T_U3VHostEvent)(context);
    USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *dataTransferEvent;
    T_U3VHostEventWriteCompleteData u3vTransferCompleteData;
 
    u3vInstance = &gUSBHostU3VObj[index];

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

        /* not used cases, fallthrough */
        case USB_HOST_DEVICE_INTERFACE_EVENT_SET_INTERFACE_COMPLETE:
        case USB_HOST_DEVICE_INTERFACE_EVENT_PIPE_HALT_CLEAR_COMPLETE:
        default:
            break;
    }

    return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE;
}


/**
 * U3V Host Interface tasks function.
 *
 * Local function used as a routine to handle interface related tasks. Used as a
 * callback by the USB Host layer.
 * @param interfaceHandle
 * @note Not applicable, used as a placeholder for gUSBHostU3VClientDriver.
 */
static void U3VHost_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    /* N/A */
}


/**
 * U3V Host device event handler function. 
 * 
 * Local function used as device event handler, called by the USB Host layer
 * when a device related (configuration) event occurs. Used as a callback by the
 * USB Host layer.
 * @param deviceHandle 
 * @param event 
 * @param eventData 
 * @param context 
 * @return USB_HOST_DEVICE_EVENT_RESPONSE   (return N/A)
 */
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


/**
 * U3V Host device assignment function.
 *
 * Local function used for device assignment, called by the USB Host layer
 * when a device with the corresponding TPL data has been detected. Used as
 * a callback by the USB Host layer.
 * @param deviceHandle
 * @param deviceObjHandle
 * @param deviceDescriptor
 */
static void U3VHost_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                 USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                 USB_DEVICE_DESCRIPTOR *deviceDescriptor)
{
    T_U3VHostInstanceObj *u3vInstance = NULL;

    for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
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


/**
 * U3V Host device release function.
 *
 * Local function used for device release, called by the USB Host layer when a 
 * previously connected device has been detached. Used as a callback by the USB
 * Host layer.
 * @param deviceHandle 
 */
static void U3VHost_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    const uint32_t index = U3VHost_DeviceHandleToInstance(deviceHandle);

    if (index < UINT32_MAX)
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


/**
 * U3V Host device tasks function.
 *
 * Local function used as a device tasks routine, called by the USB Host layer
 * periodically. This function handles the configuration stages of an attaced
 * device. Used as a callback by the USB Host layer.
 * @param deviceHandle 
 */
static void U3VHost_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    T_U3VHostInstanceObj *u3vInstance;
    const uint32_t index = U3VHost_DeviceHandleToInstance(deviceHandle);
    USB_HOST_RESULT result;
    USB_HOST_REQUEST_HANDLE requestHandle;

   if (index < UINT32_MAX)
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
                        u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_CONFIG_SET;
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_CONFIG_SET:
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
                        for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
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
                    /* do nothing */
                    break;

                case U3V_HOST_STATE_ERROR:
                default:
                    break;
            }
        }
    }
}


/**
 * U3V Host device client handle to instance matching function.
 *
 * Local function used to match a client handle to a valid instance of 
 * gUSBHostU3VObj object. If UINT32_MAX is returned, handle is invalid.
 * @param deviceClientHandle 
 * @return uint32_t 
 */
static uint32_t U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle)
{
    uint32_t result = UINT32_MAX;

    for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceClientHandle == deviceClientHandle)
        {
            result = iterator;
            break;
        }
    }

    return result;
}


/**
 * U3V Host device object handle to instance matching function.
 *
 * Local function used to match an object handle to a valid instance of 
 * gUSBHostU3VObj object. If UINT32_MAX is returned, handle is invalid.
 * @param deviceObjHandle 
 * @return uint32_t 
 */
static uint32_t U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle)
{
    uint32_t result = UINT32_MAX;

    for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceObjHandle == deviceObjHandle)
        {
            result = iterator;
            break;
        }
    }

    return result;
}


/**
 * U3V Host interface handle to instance matching function.
 *
 * Local function used to match a valid interface handle to a valid instance of 
 * gUSBHostU3VObj object. If UINT32_MAX is returned, handle is invalid.
 * @param interfaceHandle 
 * @return uint32_t 
 */
static uint32_t U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    uint32_t result = UINT32_MAX;

    for (uint32_t iterator = UINT32_C(0); iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if ((gUSBHostU3VObj[iterator].controlIfHandle.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].eventIfHandle.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].streamIfHandle.ifHandle == interfaceHandle))
        {
            result = iterator;
            break;
        }
    }

    return result;
}


/**
 * USB Host layer to U3V Host result mapping.
 *
 * Local function used to map a USB Host layer result to a U3V Host result.
 * @param hostResult 
 * @return T_U3VHostResult 
 */
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


/**
 * U3V Control Interface - Read memory register function.
 *
 * This function can be used to read a register directly from a connected U3V 
 * device memory, using the dedicated Control Interface.
 * @param u3vCtrlIf
 * @param transferHandle
 * @param memAddress
 * @param transfSize
 * @param bytesRead
 * @param buffer   (unsigned integer datatype, size according to request)
 * @return T_U3VHostResult
 * @warning This function may be used only after the Control IF has been 
 * detected and assigned. This function makes use of 'vTaskDelay' of FreeRTOS.
 */
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
    uint32_t maxBytesPerRead;
    uint32_t totalBytesRead = UINT32_C(0);
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge ack = {0};
    const size_t cmdBufferSize = sizeof(T_U3VCtrlIfReadMemCommand);

    u3vResult = (u3vCtrlIf == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (bytesRead == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer == NULL)    ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize == 0)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = u3vCtrlIf;
    maxBytesPerRead = ctrlIfInst->maxAckTransfSize - (uint32_t)sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = ((uint32_t)transfSize > maxBytesPerRead)                          ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = ((uint32_t)cmdBufferSize > ctrlIfInst->maxCmdTransfSize)          ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead == UINT32_C(0))                                  ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesRead = UINT32_C(0);

    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }
    
    while (totalBytesRead < (uint32_t)transfSize)
    {
        const uint32_t bytesThisIteration = U3VDRV_MIN(((uint32_t)transfSize - totalBytesRead), maxBytesPerRead);
        uint32_t writeRetryCnt = UINT32_C(0);
        T_U3VCtrlIfReadMemCommand cmdStr = {0};

        ctrlIfInst->writeReqSts.length = 0;
        ctrlIfInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlIfInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        cmdStr.S.header.prefix    = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
        cmdStr.S.header.flags     = (uint16_t)(U3V_CTRL_REQ_ACK);
        cmdStr.S.header.cmd       = (uint16_t)(U3V_CTRL_READMEM_CMD);
        cmdStr.S.header.length    = (uint16_t)sizeof(T_U3VCtrlIfReadMemCmdPayload);
        cmdStr.S.header.requestId = (++(ctrlIfInst->requestId));
        cmdStr.S.payload.address  = memAddress + (uint64_t)totalBytesRead;
        cmdStr.S.payload.reserved = 0U;
        cmdStr.S.payload.byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             cmdStr.B,
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
            if ((writeRetryCnt * U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS) > (ctrlIfInst->u3vTimeout))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            vTaskDelay(pdMS_TO_TICKS(U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + U3VDRV_MAX((size_t)(bytesThisIteration), sizeof(T_U3VCtrlIfPendingAckPayload));
            T_U3VCtrlIfPendingAckPayload pendingAck = {0};
            uint32_t readRetryCnt = UINT32_C(0);

            ctrlIfInst->readReqSts.length = 0;
            ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlIfInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            memset(ack.B, 0, ackBufferSize);

            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ack.B,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack.S.header.prefix != (uint32_t)U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if ((readRetryCnt * U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS) > (ctrlIfInst->u3vTimeout))
                {
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                vTaskDelay(pdMS_TO_TICKS(U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS));
            }

            /* Inspect the acknowledge buffer */
            if (((ack.S.header.cmd != (uint16_t)U3V_CTRL_READMEM_ACK) && (ack.S.header.cmd != (uint16_t)U3V_CTRL_PENDING_ACK)) ||
                (ack.S.header.prefix != (uint32_t)U3V_CONTROL_MGK_PREFIX) ||
                (ack.S.header.status != (uint16_t)U3V_ERR_NO_ERROR) ||
                (ack.S.header.ackId != ctrlIfInst->requestId) ||
                ((ack.S.header.cmd == (uint16_t)U3V_CTRL_READMEM_ACK) && (ack.S.header.length != bytesThisIteration)) ||
                ((ack.S.header.cmd == (uint16_t)U3V_CTRL_PENDING_ACK) && (ack.S.header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
            if (ack.S.header.cmd == (uint16_t)U3V_CTRL_PENDING_ACK)
            {
                memcpy(pendingAck.B, ack.S.payload, sizeof(T_U3VCtrlIfPendingAckPayload));
                ctrlIfInst->u3vTimeout = U3VDRV_MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck.S.timeout));
                continue;
            }

            /* Acknowledge received successfully */
            reqAcknowledged = true;

        } /* loop until reqAcknowledged == true */
        totalBytesRead += bytesThisIteration;
    } /* loop until totalBytesRead == transfSize */

    if (totalBytesRead != (uint32_t)transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* Extract the data */
	memcpy(buffer, ack.S.payload, totalBytesRead);
    *bytesRead = totalBytesRead;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


/**
 * U3V Control Interface - Write memory register function.
 *
 * This function can be used to write a register directly to a connected U3V 
 * device memory, using the dedicated Control Interface.
 * @param u3vCtrlIf
 * @param transferHandle
 * @param memAddress
 * @param transfSize
 * @param bytesWritten
 * @param buffer  (unsigned integer datatype, size according to request)
 * @return T_U3VHostResult
 * @warning This function may be used only after the Control IF has been 
 * detected and assigned. This function makes use of 'vTaskDelay' of FreeRTOS.
 */
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
    uint32_t maxBytesPerWrite;
    uint32_t totalBytesWritten = UINT32_C(0);
    bool reqAcknowledged;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfWriteMemCommand);
    const size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + U3VDRV_MAX(sizeof(T_U3VCtrlIfWriteMemAckPayload), sizeof(T_U3VCtrlIfPendingAckPayload));

    u3vResult = (u3vCtrlIf == NULL)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;
    u3vResult = (bytesWritten == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer == NULL)       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize == 0)      ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    
    ctrlIfInst = u3vCtrlIf;
    maxBytesPerWrite = ctrlIfInst->maxAckTransfSize - (uint32_t)sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = ((uint32_t)transfSize > maxBytesPerWrite)                         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = ((uint32_t)cmdBufferSize > ctrlIfInst->maxCmdTransfSize)          ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite == UINT32_C(0))                                 ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesWritten = UINT32_C(0);
 
    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < (uint32_t)transfSize)
    {
        const uint32_t bytesThisIteration = U3VDRV_MIN(((uint32_t)transfSize - totalBytesWritten), maxBytesPerWrite);
        T_U3VCtrlIfWriteMemCommand cmnd = {0};
        T_U3VCtrlIfPendingAckPayload pendingAck = {0};
        uint32_t writeRetryCnt = UINT32_C(0);

        cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration;

        ctrlIfInst->writeReqSts.length = 0;
        ctrlIfInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlIfInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        cmnd.S.header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
        cmnd.S.header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        cmnd.S.header.cmd    = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        cmnd.S.header.length = (uint16_t)(sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration);
        cmnd.S.header.requestId = (++(ctrlIfInst->requestId));
        cmnd.S.payload.address = memAddress + (uint64_t)totalBytesWritten;

        memcpy(cmnd.S.payload.data, ((uint8_t *)buffer) + totalBytesWritten, bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             cmnd.B,
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
            if ((writeRetryCnt * U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS) > (ctrlIfInst->u3vTimeout))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            vTaskDelay(pdMS_TO_TICKS(U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            T_U3VCtrlIfAcknowledge ack = {0};
            T_U3VCtrlIfWriteMemAckPayload writeMemAck = {0};
            uint32_t readRetryCnt = UINT32_C(0);

            ctrlIfInst->readReqSts.length = 0;
            ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlIfInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ack.B,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = U3VHost_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack.S.header.prefix != (uint32_t)U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if ((readRetryCnt * U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS) > (ctrlIfInst->u3vTimeout))
                {
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                vTaskDelay(pdMS_TO_TICKS(U3V_HOST_CTRL_IF_WAIT_FOR_ACK_DELAY_MS));
            }
            
            memcpy(writeMemAck.B, ack.S.payload, sizeof(T_U3VCtrlIfWriteMemAckPayload));

            /* Inspect the acknowledge buffer */
            if (((ack.S.header.cmd != (uint16_t)U3V_CTRL_WRITEMEM_ACK) && (ack.S.header.cmd != (uint16_t)U3V_CTRL_PENDING_ACK)) ||
                (ack.S.header.prefix != (uint32_t)U3V_CONTROL_MGK_PREFIX) ||
                (ack.S.header.status != (uint16_t)U3V_ERR_NO_ERROR) ||
                (ack.S.header.ackId != ctrlIfInst->requestId) ||
                ((ack.S.header.cmd == (uint16_t)U3V_CTRL_WRITEMEM_ACK) && (ack.S.header.length != sizeof(T_U3VCtrlIfWriteMemAckPayload)) && (ack.S.header.length != 0U)) ||
                ((ack.S.header.cmd == (uint16_t)U3V_CTRL_WRITEMEM_ACK) && (ack.S.header.length == sizeof(T_U3VCtrlIfWriteMemAckPayload)) && (writeMemAck.S.bytesWritten != bytesThisIteration)) ||
                ((ack.S.header.cmd == (uint16_t)U3V_CTRL_PENDING_ACK) && (ack.S.header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
			if (ack.S.header.cmd == (uint16_t)U3V_CTRL_PENDING_ACK) 
            {
                memcpy(pendingAck.B, ack.S.payload, sizeof(T_U3VCtrlIfPendingAckPayload));
                ctrlIfInst->u3vTimeout = U3VDRV_MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck.S.timeout));
                continue;
            }
            /* Acknowledge received successfully */
			reqAcknowledged = true;
        } /* loop until reqAcknowledged == true */
        totalBytesWritten += bytesThisIteration;
    } /* loop until totalBytesWritten == transfSize */

    if (totalBytesWritten != (uint32_t)transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    *bytesWritten = totalBytesWritten;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


/**
 * U3V Control Interface - transfer request complete callback function.
 *
 * This callback is called when a transfer request over the Control Interface
 * has been completed.
 * @param u3vHostHandle
 * @param transfEvent
 * @param transfData
 * @warning Shall only be used for Control Interface transfers.
 * @note callback type is 'T_U3VHostTransfCompleteHandler'
 */
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


/**
 * U3V Control Interface - clear Control IF obect data.
 * 
 * @param pCtrlIfObj 
 */
static inline void U3VHost_CtrlIfClearObjData(T_U3VControlIfObj *pCtrlIfObj)
{
    if (pCtrlIfObj != NULL)
    {
        memset(pCtrlIfObj, 0, sizeof(T_U3VControlIfObj));
    }
}


/**
 * Greater Common Divisor calculation with 2 operands.
 * 
 * @param n1    (uint32_t)
 * @param n2    (uint32_t)
 * @return uint32_t 
 */
static inline uint32_t U3VHost_GCDu32(uint32_t n1, uint32_t n2)
{
    while (n2 > UINT32_C(0))
    {
        uint32_t temp = n2;
        n2 = n1 % n2;
        n1 = temp;
    }
    return n1;
}


/**
 * Least Common Multiplier calculation with 2 operands. 
 * 
 * @param n1    (uint32_t)
 * @param n2    (uint32_t)
 * @return uint32_t
 * @warning Depends to 'U3VHost_GCDu32' for calculation.
 */
static inline uint32_t U3VHost_LCMu32(uint32_t n1, uint32_t n2)
{
    uint32_t res = UINT32_C(0);

    if ((n1 > UINT32_C(0)) && (n2 > UINT32_C(0)))
    {
        if (n1 > n2)
        {
                res = ((n1 / U3VHost_GCDu32(n1, n2)) * n2);
        }
        else
        {
                res = ((n2 / U3VHost_GCDu32(n1, n2)) * n1);
        }
    }
    return res;
}

