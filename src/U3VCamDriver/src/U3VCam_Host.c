//
// Created by fomarko on 04/02/22.
//

#include <string.h>
#include "U3VCam_Host.h"
#include "U3VCam_Host_Local.h"
#include "U3VCam_Device_Class_Specs.h"
#include "math.h"

#include "FreeRTOS.h"
#include "task.h"

/********************************************************
* Local function declarations
*********************************************************/

static void _U3VHost_Initialize(void *data);

static void _U3VHost_Deinitialize(void);

static void _U3VHost_Reinitialize(void *msdInitData);

static void _U3VHost_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                  USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                  USB_DEVICE_DESCRIPTOR *deviceDescriptor);

static void _U3VHost_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void _U3VHost_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void _U3VHost_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
                                     USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                     size_t nInterfaces,
                                     uint8_t *descriptor);

static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _U3VHost_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                               USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                               void *eventData,
                                                                               uintptr_t context);

static void _U3VHost_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static void _U3VHost_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static USB_HOST_DEVICE_EVENT_RESPONSE _U3VHost_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                  USB_HOST_DEVICE_EVENT event,
                                                                  void *eventData,
                                                                  uintptr_t context);

static int32_t _U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle);

static int32_t _U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle);

static int32_t _U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);


static inline uint32_t _LCMui32(uint32_t n1, uint32_t n2) /* least common multiplier calc restricted to 32bit size max */
{
    uint32_t lcm;

    lcm = (n1 > n2) ? n1 : n2;

    /* check if any of inputs is 0 */
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
};


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VHostInstanceObj gUSBHostU3VObj[U3V_HOST_INSTANCES_NUMBER];

T_UsbHostU3VAttachListenerObj gUSBHostU3VAttachListener[U3V_HOST_ATTACH_LISTENERS_NUMBER];

USB_HOST_CLIENT_DRIVER gUSBHostU3VClientDriver =
{
    .initialize             = _U3VHost_Initialize,
    .deinitialize           = _U3VHost_Deinitialize,
    .reinitialize           = _U3VHost_Reinitialize,
    .interfaceAssign        = _U3VHost_InterfaceAssign,
    .interfaceRelease       = _U3VHost_InterfaceRelease,
    .interfaceEventHandler  = _U3VHost_InterfaceEventHandler,
    .interfaceTasks         = _U3VHost_InterfaceTasks,
    .deviceEventHandler     = _U3VHost_DeviceEventHandler,
    .deviceAssign           = _U3VHost_DeviceAssign,
    .deviceRelease          = _U3VHost_DeviceRelease,
    .deviceTasks            = _U3VHost_DeviceTasks
};


/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context)
{
    T_U3VHostResult result = U3V_HOST_RESULT_FAILURE;
    T_UsbHostU3VAttachListenerObj *attachListener;
    int32_t iterator;

    if (eventHandler == NULL)
    {
        result = U3V_HOST_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Search for free listener object */
        for (iterator = 0; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
        {
            if (!gUSBHostU3VAttachListener[iterator].inUse)
            {
                /* Found a free object */
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


T_U3VHostDeviceObjHandle U3VHost_DeviceObjectHandleGet(T_U3VHostHandle u3vObjHandle)
{
    USB_HOST_DEVICE_OBJ_HANDLE result = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
    T_U3VHostInstanceObj *u3vInstance;

    if(u3vObjHandle != 0UL)
    {
        u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
        result = u3vInstance->deviceObjHandle;
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


void U3VHost_Close(T_U3VHostHandle u3vDeviceHandle)
{
    //TODO
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


T_U3VHostResult U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle,
                                              T_U3VHostDetachEventHandler detachEventHandler,
                                              uintptr_t context)
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


T_U3VHostResult _U3VHost_HostToU3VResultsMap(USB_HOST_RESULT hostResult)
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

    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                          NULL,
                                          U3V_ABRM_SBRM_ADDRESS_OFS,
                                          sizeof(sbrmAddress),
                                          &bytesRead,
                                          (void *)&sbrmAddress);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
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
        u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
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

        /* the SI Info req may take up to 620us to complete */
        u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
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
            deviceByteAlignment = 1 << ((siInfo & U3V_SIRM_INFO_ALIGNMENT_MASK) >> U3V_SIRM_INFO_ALIGNMENT_SHIFT);
            u3vInstance->u3vDevInfo.transferAlignment = _LCMui32(deviceByteAlignment, u3vInstance->u3vDevInfo.hostByteAlignment);
        }
        
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_ReadMemRegIntegerValue(T_U3VHostHandle u3vObjHandle,
                                               T_U3VMemRegInteger integerReg,
                                               uint32_t *const pReadValue)
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
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].colorCodingID_Reg;
                u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                      NULL,
                                                      regAddr,
                                                      sizeof(regValue),
                                                      &bytesRead,
                                                      (void *)&regValue);
                /* value is stored on high byte (bits 24 to 31) */
                regValue = (regValue >> 24U) & 0xFFUL;
                break;

            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].payloadSizeVal_Reg;
                u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                      NULL,
                                                      regAddr,
                                                      sizeof(regValue),
                                                      &bytesRead,
                                                      (void *)&regValue);
                /* redundant assignment */
                regValue = regValue;
                break;

            case U3V_MEM_REG_INT_ACQUISITION_MODE:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].acquisitionMode_Reg;
                u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                      NULL,
                                                      regAddr,
                                                      sizeof(regValue),
                                                      &bytesRead,
                                                      (void *)&regValue);
                /* redundant assignment */
                regValue = regValue;
                break;

            case U3V_MEM_REG_INT_DEVICE_RESET:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].deviceReset_Reg;
                u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                      NULL,
                                                      regAddr,
                                                      sizeof(regValue),
                                                      &bytesRead,
                                                      (void *)&regValue);
                /* value is stored in MSB (bit 31) */
                regValue = (regValue >> 31U) & 0xFFUL;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            if (bytesRead = sizeof(regValue))
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


T_U3VHostResult U3VHost_WriteMemRegIntegerValue(T_U3VHostHandle u3vObjHandle,
                                                T_U3VMemRegInteger integerReg,
                                                uint32_t regVal)
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
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].colorCodingID_Reg;
                /* value is stored on high byte (bits 24 to 31) */
                regValue = (regVal << 24U) & 0xFF000000UL;
                break;

            case U3V_MEM_REG_INT_PAYLOAD_SIZE:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].payloadSizeVal_Reg;
                regValue = regVal;
                break;

            case U3V_MEM_REG_INT_ACQUISITION_MODE:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].acquisitionMode_Reg;
                regValue = regVal;
                break;

            case U3V_MEM_REG_INT_DEVICE_RESET:
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].deviceReset_Reg;
                regValue = regVal;
                break;

            default:
                u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                break;
        }

        if (u3vResult == U3V_HOST_RESULT_SUCCESS)
        {
            u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                   NULL,
                                                   regAddr,
                                                   sizeof(regValue),
                                                   &bytesWritten,
                                                   (void *)&regValue);
            if (bytesWritten != sizeof(regValue))
            {
                u3vResult = U3V_HOST_RESULT_ABORTED;
            }
        }
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_ReadMemRegFloatValue(T_U3VHostHandle u3vObjHandle,
                                             T_U3VMemRegFloat floatReg,
                                             float *const pReadValue)
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
                regAddr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                          U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].temperature_Reg;
                u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                      NULL,
                                                      regAddr,
                                                      sizeof(regValue),
                                                      &bytesRead,
                                                      (void *)&regValue);
                                                      
                /* Calculate temperature in Celcius rounded to 2 decimals */
                floatRetVal = roundf(((float)((regValue & 0xFFFUL) / 10UL) - 273.15F) * 100) / 100;
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


T_U3VHostResult U3VHost_ReadMemRegStringValue(T_U3VHostHandle u3vObjHandle,
                                              T_U3VMemRegString stringReg,
                                              char *const pReadBfr)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint32_t stringSize;
    char stringBfr[64u];
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
            u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                                  NULL,
                                                  regAddr,
                                                  stringSize,
                                                  &bytesRead,
                                                  (void *)stringBfr);

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
    uint32_t acquisnStartCmdVal;
    uint64_t acquisStartRegAdr;

    u3vResult = (u3vObjHandle == 0U)        ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

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

    acquisStartRegAdr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                        U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].acquisitionStart_Reg;

    acquisnStartCmdVal = (0x80U << 24);

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                           NULL,
                                           acquisStartRegAdr,
                                           sizeof(acquisnStartCmdVal),
                                           &bytesWritten,
                                           (void *)&acquisnStartCmdVal);

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
    uint32_t acquisnStopCmdVal;
    uint64_t acquisStopRegAdr;

    u3vResult = (u3vObjHandle == 0U)        ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

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

    acquisStopRegAdr = U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].camRegBaseAddress +
                       U3VCamRegisterLUT[U3V_CAM_MODEL_SEL].acquisitionStop_Reg;

    acquisnStopCmdVal = 0x0000000UL;

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                           NULL,
                                           acquisStopRegAdr,
                                           sizeof(acquisnStopCmdVal),
                                           &bytesWritten,
                                           (void *)&acquisnStopCmdVal);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    return u3vResult;
}


T_U3VHostResult U3VHost_SetupStreamTransferParams(T_U3VHostHandle u3vObjHandle, T_U3VStreamIfConfig *streamConfig)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint64_t sirmAddress;
    uint32_t u32ImageSize; /* image payload size does not overflow 32bit, this can hold max size */
    uint32_t siRequiredLeaderSize;
    uint64_t siRequiredPayloadSize;
    uint32_t siRequiredTrailerSize;
    uint32_t siMaxLeaderSize;
    uint32_t siPayloadTransfSize;
    uint32_t siPayloadTransfCount;
    uint32_t siPayloadFinalTransf1Size;
    uint32_t siPayloadFinalTransf2Size;
    uint32_t siMaxTrailerSize;

    u3vResult = (u3vObjHandle == 0U)   ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (streamConfig == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

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

    sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    u32ImageSize = (uint32_t)(streamConfig->imageSize & 0xFFFFFFFFUL) ;
    siMaxLeaderSize = streamConfig->maxLeaderSize;
    siMaxTrailerSize = streamConfig->maxTrailerSize;
    siPayloadTransfSize = 512U;
    siPayloadTransfCount = u32ImageSize / siPayloadTransfSize;
    /* transfer1 size is the remainder of the total payload with padding of U3V_TARGET_ARCH_BYTE_ALIGNMENT */
    siPayloadFinalTransf1Size = ((u32ImageSize % siPayloadTransfSize) / U3V_TARGET_ARCH_BYTE_ALIGNMENT) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    /* transfer2 size is the remainder of the transfer1 block payload, padded to U3V_TARGET_ARCH_BYTE_ALIGNMENT and rounded up */
    siPayloadFinalTransf2Size = u32ImageSize - siPayloadTransfCount * siPayloadTransfSize - siPayloadFinalTransf1Size;
    /* if transfer2 padding is not an integer, round up to padding */
    if ((siPayloadFinalTransf2Size % U3V_TARGET_ARCH_BYTE_ALIGNMENT) > 0UL)
    {
        siPayloadFinalTransf2Size = ((siPayloadFinalTransf2Size / U3V_TARGET_ARCH_BYTE_ALIGNMENT) + 1UL) * U3V_TARGET_ARCH_BYTE_ALIGNMENT;
    }

    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                          NULL,
                                          sirmAddress + U3V_SIRM_REQ_LEADER_SIZE_OFS,
                                          sizeof(siRequiredLeaderSize),
                                          &bytesRead,
                                          (void *)&siRequiredLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                           NULL,
                                           sirmAddress + U3V_SIRM_REQ_PAYLOAD_SIZE_OFS,
                                           sizeof(siRequiredPayloadSize),
                                           &bytesRead,
                                           (void *)&siRequiredPayloadSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                           NULL,
                                           sirmAddress + U3V_SIRM_REQ_TRAILER_SIZE_OFS,
                                           sizeof(siRequiredTrailerSize),
                                           &bytesRead,
                                           (void *)&siRequiredTrailerSize);
    
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    if (((uint32_t)siRequiredPayloadSize != u32ImageSize) ||
        (siRequiredLeaderSize > siMaxLeaderSize) ||
        (siRequiredTrailerSize > siMaxTrailerSize))
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                           NULL,
                                           sirmAddress + U3V_SIRM_MAX_LEADER_SIZE_OFS,
                                           sizeof(siMaxLeaderSize),
                                           &bytesRead,
                                           (void *)&siMaxLeaderSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                            NULL,
                                            sirmAddress + U3V_SIRM_MAX_TRAILER_SIZE_OFS,
                                            sizeof(siMaxTrailerSize),
                                            &bytesRead,
                                            (void *)&siMaxTrailerSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                            NULL,
                                            sirmAddress + U3V_SIRM_PAYLOAD_SIZE_OFS,
                                            sizeof(siPayloadTransfSize),
                                            &bytesRead,
                                            (void *)&siPayloadTransfSize);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                            NULL,
                                            sirmAddress + U3V_SIRM_PAYLOAD_COUNT_OFS,
                                            sizeof(siPayloadTransfCount),
                                            &bytesRead,
                                            (void *)&siPayloadTransfCount);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                            NULL,
                                            sirmAddress + U3V_SIRM_TRANSFER1_SIZE_OFS,
                                            sizeof(siPayloadFinalTransf1Size),
                                            &bytesRead,
                                            (void *)&siPayloadFinalTransf1Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
                                            NULL,
                                            sirmAddress + U3V_SIRM_TRANSFER2_SIZE_OFS,
                                            sizeof(siPayloadFinalTransf2Size),
                                            &bytesRead,
                                            (void *)&siPayloadFinalTransf2Size);

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    vTaskDelay(pdMS_TO_TICKS(10));


    return u3vResult;
}


T_U3VHostResult U3VHost_StreamChControl(T_U3VHostHandle u3vObjHandle, bool enable)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    uint32_t bytesRead;
    uint64_t sirmAddress;
    uint32_t siControlCmd;

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

    sirmAddress = u3vInstance->u3vDevInfo.sirmAddr;
    siControlCmd = (enable) ? 0x00000001UL : 0x00000000UL;

    u3vResult = U3VHost_CtrlIf_WriteMemory((T_U3VControlIfObjHandle)ctrlIfInstance,
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


T_U3VHostResult U3VHost_ResetStreamCh(T_U3VHostHandle u3vObjHandle)
{
    USB_HOST_RESULT hostResult;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VHostInstanceObj *u3vInstance;
    T_U3VControlIfObj *ctrlIfInstance;
    USB_HOST_REQUEST_HANDLE tempReqHandle;

    u3vResult = (u3vObjHandle == 0U)    ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    hostResult = USB_HOST_DevicePipeHaltClear(u3vInstance->streamIfHandle.bulkInPipeHandle, &tempReqHandle, 0U);
    u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

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
    // strmChInstance = u3vInstance->streamIfHandle.;

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
    u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

    return u3vResult;
}


/********************************************************
* Local function definitions
*********************************************************/

static void _U3VHost_Initialize(void * data)
{
    T_U3VHostInstanceObj *u3vInstanceObj = NULL;
    uint32_t iterator;

    for(iterator = 0U; iterator < U3V_HOST_INSTANCES_NUMBER; iterator ++)
    {
        /* Set the pipes handles to invalid */
        u3vInstanceObj = &gUSBHostU3VObj[iterator];

        u3vInstanceObj->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
        u3vInstanceObj->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->controlPipeHandle = USB_HOST_CONTROL_PIPE_HANDLE_INVALID;

        u3vInstanceObj->controlIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;

        u3vInstanceObj->eventIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->eventIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;   /* N/A */

        u3vInstanceObj->streamIfHandle.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->streamIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;  /* N/A */

    }
}

static void _U3VHost_Deinitialize(void)
{
    /* N/A */
}


static void _U3VHost_Reinitialize(void * msdInitData)
{
    /* N/A */
}


static void _U3VHost_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
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

    /* Expected number of interfaces = 3 for U3V device (Control/Event/Streaming) */
    if (nInterfaces > 1U) 
    {
        /* Then this means that this is an IAD. We first assign a U3V instance to this device */

        for (iterator = 0U; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
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
            /* Instance not found. Release all interfaces */
            for (iterator = 0U; iterator < nInterfaces; iterator++)
            {
                USB_HOST_DeviceInterfaceRelease(interfaces[iterator]);
            }
        }
        else
        {
            /* Save the device object handle and open the control pipe */
            u3vInstance->deviceObjHandle = deviceObjHandle;
            u3vInstance->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);

            /* An instance is assigned. The descriptor will be a pointer to the IAD.
             * Lets get the first interface descriptor in the IAD group and see which interface this is. */
            USB_HOST_DeviceInterfaceQueryContextClear(&interfaceDescriptorQuery);
            interfaceDescriptorQuery.flags = USB_HOST_INTERFACE_QUERY_ANY;

            /* We know that we need two interfaces */
            for (iterator = 0U; iterator < nInterfaces; iterator++)
            {
                /* We need to search for two interface descriptors */
                interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)(descriptor),
                                                                                     &interfaceDescriptorQuery);

                /* If we have a valid interface descriptor find out its type */
                if (interfaceDescriptor != NULL)
                {
                    /* Control Interface */
                    if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                        (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                        (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_CONTROL) &&
                        (interfaceDescriptor->bDescriptorType == U3V_DESCRIPTOR_TYPE_INTERFACE))
                    {
                        /* We found the communication class */
                        u3vInstance->controlIfHandle.idNum = interfaceDescriptor->bInterfaceNumber;
                        u3vInstance->controlIfHandle.ifHandle = interfaces[iterator];

                        /* Create the endpoint query */
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            /* Open the pipe */
                            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->controlIfHandle.ifHandle,
                                                                                                    endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make sure that the pipe handle stays invalid if we could not open the pipe */
                            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* Get the bulk out endpoint */
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_HOST_TO_DEVICE;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
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
                        /* We found the data class */

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
                            /* Make the pipe handle invalid */
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
                        /* We found the data class */

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
                            /* Make the pipe handle invalid */
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

            /* Now check if we can move the host client driver instance to a ready state */

            if ((u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
            {
                /* All the pipes are opened. The client driver is ready */
                u3vInstance->state = U3V_HOST_STATE_READY;

                /* We know that the client driver is now ready. We can let 
                 * all the listeners know that the device has be attached. */
                for (iterator = 0U; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
                {
                    if (gUSBHostU3VAttachListener[iterator].inUse)
                    {
                        /* Call the attach listener event handler 
                         * function. */
                        gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostHandle)u3vInstance,
                                                                         gUSBHostU3VAttachListener[iterator].context);
                    }
                }
            }
            else
            {
                /* Something went wrong. */
                u3vInstance->state = U3V_HOST_STATE_ERROR;
            }
        }
    }
    else
    {
        /* nInterfaces do not match to a U3V device */
    }
}


static void _U3VHost_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    int32_t u3vIndex;
    T_U3VHostInstanceObj *u3vInstance;

    /* Get the instance associated with this interface */
    u3vIndex = _U3VHost_InterfaceHandleToInstance(interfaceHandle);

    if (u3vIndex >= 0U)
    {
        /* Get the pointer to the instance object */
        u3vInstance = &gUSBHostU3VObj[u3vIndex];
        
        if(u3vInstance->inUse)
        {
            u3vInstance->inUse = false;

            if(u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the controlIfHandle.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkInPipeHandle);
                u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->controlIfHandle.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the controlIfHandle.bulkOutPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkOutPipeHandle);
                u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the eventIfHandle.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->eventIfHandle.bulkInPipeHandle);
                u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }
            
            if(u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the streamIfHandle.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->streamIfHandle.bulkInPipeHandle);
                u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->detachEventHandler != NULL)
            {
                /* Let the client know that the device is detached */
                u3vInstance->detachEventHandler((T_U3VHostHandle)(u3vInstance), u3vInstance->context);
            }

            /* Release the object */
            u3vInstance->detachEventHandler = NULL;
            u3vInstance->eventHandler = NULL;
            u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
            u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
        }
    }
}


static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _U3VHost_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                               USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                               void *eventData,
                                                                               uintptr_t context)
{
    T_U3VHostInstanceObj *u3vInstance;
    int32_t u3vIndex;
    USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *dataTransferEvent;
    T_U3VHostEvent u3vEvent;
    T_U3VHostEventWriteCompleteData u3vTransferCompleteData;

    /* Find out to which U3V Instance this interface belongs */
    u3vIndex = _U3VHost_InterfaceHandleToInstance(interfaceHandle);
    u3vInstance = &gUSBHostU3VObj[u3vIndex];
    u3vEvent = (T_U3VHostEvent)(context);

    switch (event)
    {
        case USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE:
            /* This means a data transfer has completed */
            dataTransferEvent = (USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *)(eventData);
            u3vTransferCompleteData.transferHandle = dataTransferEvent->transferHandle;
            u3vTransferCompleteData.result = _U3VHost_HostToU3VResultsMap(dataTransferEvent->result);
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


static void _U3VHost_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    /* N/A */
}


static USB_HOST_DEVICE_EVENT_RESPONSE _U3VHost_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                  USB_HOST_DEVICE_EVENT event,
                                                                  void *eventData,
                                                                  uintptr_t context)
{
   /* The event context is the pointer to the U3V Instance Object */
   T_U3VHostInstanceObj *u3vInstance = (T_U3VHostInstanceObj *)(context);
   USB_HOST_DEVICE_EVENT_CONFIGURATION_SET_DATA *configSetEventData;

   switch (event)
   {
        case USB_HOST_DEVICE_EVENT_CONFIGURATION_SET:
            /* This means the configuration was set. Update the instance
             * variables to let the main state machine know. */
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


static void _U3VHost_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                  USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                  USB_DEVICE_DESCRIPTOR *deviceDescriptor)
{
    uint32_t iterator;
    T_U3VHostInstanceObj *u3vInstanceObj = NULL;
    /* If this function is being called, this means that a device class subclass
     * protocol match was obtained.  Look for a free instance */
    for (iterator = 0U; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        /* Search for an available U3V instance object */
        if (!gUSBHostU3VObj[iterator].inUse)
        {
            /* Allocate the object */
            u3vInstanceObj = &gUSBHostU3VObj[iterator];
            u3vInstanceObj->inUse = true;
            u3vInstanceObj->deviceObjHandle = deviceObjHandle;
            u3vInstanceObj->deviceClientHandle = deviceHandle;
            break;
        }
    }

    if(u3vInstanceObj == NULL)
    {
        /* This means an instance could not be allocated. Return the device back to the host */
        USB_HOST_DeviceRelease(deviceHandle);
    }
    else
    {
        /* An instance object was allocated. Check if the device has any configurations */
        if(deviceDescriptor->bNumConfigurations > 0U)
        {
            /* This means we have configurations. We can try setting the first 
             * configuration. Also open the control pipe to the device. */
            u3vInstanceObj->state = U3V_HOST_STATE_SET_CONFIGURATION;
            u3vInstanceObj->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);
        }
        else
        {
            /* There are no configurations! Move to error state */
            u3vInstanceObj->state = U3V_HOST_STATE_ERROR;
        }
    }
}

static void _U3VHost_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    int32_t index;
    T_U3VHostInstanceObj *u3vInstance;

    /* Find the U3V instance object that owns this device */
    index = _U3VHost_DeviceHandleToInstance(deviceHandle);

    if (index >= 0U)
    {
        u3vInstance = &gUSBHostU3VObj[index];
        u3vInstance->inUse = false;

        if (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the controlIfHandle.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkInPipeHandle);
            u3vInstance->controlIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->controlIfHandle.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the controlIfHandle.bulkOutPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->controlIfHandle.bulkOutPipeHandle);
            u3vInstance->controlIfHandle.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the eventIfHandle.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->eventIfHandle.bulkInPipeHandle);
            u3vInstance->eventIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the streamIfHandle.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->streamIfHandle.bulkInPipeHandle);
            u3vInstance->streamIfHandle.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->detachEventHandler != NULL)
        {
            /* Let the client know that the device is detached */
            u3vInstance->detachEventHandler((T_U3VHostHandle)(u3vInstance),
                                            u3vInstance->context);
        }

        /* Release the object */
        u3vInstance->detachEventHandler = NULL;
        u3vInstance->eventHandler = NULL;
        u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
    }
}


static void _U3VHost_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    int32_t index;
    int32_t iterator;
    T_U3VHostInstanceObj *u3vInstance;
    USB_HOST_RESULT result;
    USB_HOST_REQUEST_HANDLE requestHandle;

    index = _U3VHost_DeviceHandleToInstance(deviceHandle);

   if (index >= 0)
    {
        u3vInstance = &gUSBHostU3VObj[index];

        if (u3vInstance->inUse)
        {
            /* Instance is valid */
            switch (u3vInstance->state)
            {
                case U3V_HOST_STATE_NOT_READY:
                    /* The instance is not ready. We dont do anything yet */
                    break;

                case U3V_HOST_STATE_SET_CONFIGURATION:
                    /* The instance should set the configuration. We clear the
                     * controlTransferDone flag. This will be set in the device
                     * event handler when the configuration set event is received. */
                    u3vInstance->hostRequestDone = false;
                    result = USB_HOST_DeviceConfigurationSet(u3vInstance->deviceClientHandle,
                                                             &requestHandle,
                                                             0U,
                                                             (uintptr_t)(u3vInstance));

                    if (result == USB_HOST_RESULT_SUCCESS)
                    {
                        /* The result was successful. Change state to wating for configuration. */
                        u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET;
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET:
                    /* Here we are waiting for the configuration to be set */
                    if (u3vInstance->hostRequestDone == true)
                    {
                        if (u3vInstance->hostRequestResult == USB_HOST_RESULT_SUCCESS)
                        {
                            /* The configuration has been set. Now wait for the host
                             * layer to send the communication and the data
                             * interface to the client driver */
                            u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_INTERFACES;
                        }
                        else
                        {
                            /* If we could not set the configuration, then state
                             * instance state to error */
                            u3vInstance->state = U3V_HOST_STATE_ERROR;
                        }
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_INTERFACES:
                    /* Here we wait for both the interfaces to get ready */
                    if ((u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->controlIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->eventIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->streamIfHandle.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
                    {
                        /* Set the state to ready */
                        u3vInstance->state = U3V_HOST_STATE_READY;
                        /* We know that the client driver is now ready. We can
                         * let all the listeners know that the device has been
                         * attached */
                        for (iterator = 0; iterator < U3V_HOST_ATTACH_LISTENERS_NUMBER; iterator++)
                        {
                            if (gUSBHostU3VAttachListener[iterator].inUse)
                            {
                                /* Call the attach listener event handler function. */
                                gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostHandle)u3vInstance,
                                                                                 gUSBHostU3VAttachListener[iterator].context);
                            }
                        }
                    }
                    break;

                case U3V_HOST_STATE_READY:
                    /* The U3V client driver is ready to be opened */
                    break;

                case U3V_HOST_STATE_ERROR:
                    /* The instance is an error state. Do nothing */
                    break;
            }
        }
    }
}


static int32_t _U3VHost_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceClientHandle == deviceClientHandle)
        {
            result = iterator;
            break;
        }
    }
    return result;
}


static int32_t _U3VHost_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceObjHandle == deviceObjHandle)
        {
            result = iterator;
            break;
        }
    }
    return result;
}


static int32_t _U3VHost_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < U3V_HOST_INSTANCES_NUMBER; iterator++)
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

