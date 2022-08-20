//
// Created by fomarko on 04/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_HOST_H
#define COMPONENT_DRIVERS_U3VCAM_HOST_H


#include "U3VCam_Device_Class_Specs.h"
#include "U3VCam_Config.h"
#include "usb/usb_host_client_driver.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

#define U3V_HOST_RESULT_MIN                         (USB_HOST_RESULT_MIN)
#define U3V_HOST_HANDLE_INVALID                     ((T_U3VHostHandle)(-1))
#define U3V_HOST_TRANSFER_HANDLE_INVALID            ((T_U3VHostTransferHandle)(-1))
#define U3V_HOST_REQUEST_HANDLE_INVALID             ((T_U3VHostRequestHandle)(-1))
#define U3V_INTERFACE                               (&gUSBHostU3VClientDriver)


/********************************************************
* Type definitions
*********************************************************/

typedef uintptr_t T_U3VHostHandle;
typedef uintptr_t T_U3VHostTransferHandle;
typedef uintptr_t T_U3VHostRequestHandle;
typedef USB_HOST_DEVICE_OBJ_HANDLE T_U3VHostDeviceObjHandle;


#pragma pack(push, 1)

typedef struct 
{
    uint32_t        magicKey;           /* "U3VL" for Leader / "U3VT" for Trailer */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        size;
	uint64_t        blockID;
    void            *data;
} T_U3VSiGenericPacket;

typedef struct
{
    uint32_t        magicKey;           /* "U3VL" for Leader */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        leaderSize;
	uint64_t        blockID;
	uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        payloadType;        /* 0x0001 for Image */
    uint64_t        timestamp;
    uint32_t        pixelFormat;
    uint32_t        sizeX;
    uint32_t        sizeY;
    uint32_t        offsetX;
    uint32_t        offsetY;
    uint16_t        paddingX;
    uint16_t        reserved2;          /* Set 0 on Tx, ignore on Rx */
} T_U3VStrmIfImageLeader;

typedef struct
{
    uint32_t        magicKey;           /* "U3VT" for Trailer */
    uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
    uint16_t        trailerSize;
    uint64_t        blockID;
    uint16_t        status;
    uint16_t        reserved1;          /* Set 0 on Tx, ignore on Rx */
    uint64_t        validPayloadSize;
    uint32_t        sizeY;
} T_U3VStrmIfImageTrailer;

#pragma pack(pop)


typedef enum
{
    U3V_HOST_RESULT_FAILURE             = U3V_HOST_RESULT_MIN,
    U3V_HOST_RESULT_BUSY,
    U3V_HOST_RESULT_REQUEST_STALLED,
    U3V_HOST_RESULT_INVALID_PARAMETER,
    U3V_HOST_RESULT_DEVICE_UNKNOWN,
    U3V_HOST_RESULT_ABORTED,
    U3V_HOST_RESULT_HANDLE_INVALID,
    U3V_HOST_RESULT_SUCCESS             = 1
} T_U3VHostResult;

typedef enum
{
    U3V_MEM_REG_INT_PIXELFORMAT         = 0x10,
    U3V_MEM_REG_INT_PAYLOAD_SIZE,
    U3V_MEM_REG_INT_ACQUISITION_MODE,
    U3V_MEM_REG_INT_DEVICE_RESET,
} T_U3VMemRegInteger;

typedef enum
{
    U3V_MEM_REG_FLOAT_TEMPERATURE       = 0x20,
} T_U3VMemRegFloat;

typedef enum
{
    U3V_MEM_REG_STRING_MANUFACTURER_NAME    = 0x40,
    U3V_MEM_REG_STRING_MODEL_NAME,
    U3V_MEM_REG_STRING_FAMILY_NAME,
    U3V_MEM_REG_STRING_DEVICE_VERSION,
    U3V_MEM_REG_STRING_MANUFACTURER_INFO,
    U3V_MEM_REG_STRING_SERIAL_NUMBER,
    U3V_MEM_REG_STRING_USER_DEFINED_NAME,
} T_U3VMemRegString;

typedef struct
{   
    T_U3VHostTransferHandle  transferHandle;
    T_U3VHostResult          result;
    size_t                   length;
} T_U3VHostEventReadCompleteData, T_U3VHostEventWriteCompleteData;

typedef enum
{
    U3V_HOST_EVENT_READ_COMPLETE = 0x10,
    U3V_HOST_EVENT_WRITE_COMPLETE,
    U3V_HOST_EVENT_IMG_PLD_RECEIVED,
} T_U3VHostEvent;

typedef enum
{
    U3V_HOST_EVENT_RESPONE_NONE  = 0
} T_U3VHostEventResponse;

typedef struct 
{
	uint32_t    hostByteAlignment;
	uint64_t    sirmAddr;
	uint32_t    transferAlignment;
} T_U3VDeviceInfo;

typedef struct 
{
	uint64_t imageSize;
	uint64_t chunkDataSize;
	uint32_t maxBlockSize;
	uint32_t blockPadding;
	uint64_t blockSize;
	uint32_t maxLeaderSize;
	uint32_t maxTrailerSize;
} T_U3VStreamIfConfig;

typedef void (*T_U3VHostAttachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

typedef void (*T_U3VHostDetachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

typedef T_U3VHostEventResponse (*T_U3VHostEventHandler)(T_U3VHostHandle u3vObjHandle, T_U3VHostEvent event, void *eventData, uintptr_t context);


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/

extern USB_HOST_CLIENT_DRIVER   gUSBHostU3VClientDriver;


/********************************************************
* Function declarations
*********************************************************/

T_U3VHostResult U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context);

T_U3VHostResult U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle, T_U3VHostDetachEventHandler detachEventHandler, uintptr_t context);

T_U3VHostHandle U3VHost_Open(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_EventHandlerSet(T_U3VHostHandle handle, T_U3VHostEventHandler eventHandler, uintptr_t context);

T_U3VHostResult U3VHost_GetStreamCapabilities(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_SetupStreamTransferParams(T_U3VHostHandle u3vObjHandle, T_U3VStreamIfConfig *streamConfig);

T_U3VHostResult U3VHost_StreamChControl(T_U3VHostHandle u3vObjHandle, bool enable);

T_U3VHostResult U3VHost_AcquisitionStart(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_AcquisitionStop(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_StartImgPayldTransfer(T_U3VHostHandle u3vObjHandle, void *imgBfr, size_t size);

T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VHostHandle u3vObjHandle);

void U3VHost_CtrlIf_InterfaceDestroy(T_U3VHostHandle u3vObjHandle);

T_U3VHostResult U3VHost_ReadMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t *const pReadValue);

T_U3VHostResult U3VHost_ReadMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat floatReg, float *const pReadValue);

T_U3VHostResult U3VHost_ReadMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString stringReg, char *const pReadBfr); /* string buffer size must be 64bytes long (at least) */

T_U3VHostResult U3VHost_WriteMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t writeValue);

T_U3VHostResult U3VHost_WriteMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat integerReg, float writeValue);

T_U3VHostResult U3VHost_WriteMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString integerReg, const char *pWriteBfr); /* string buffer size must be 64bytes long (at least) */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_HOST_H
