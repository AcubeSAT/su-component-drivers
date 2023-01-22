#pragma once

#include <stdalign.h>
#include "U3VCam_Host.h"
#include "osal/osal.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Macro definitions
*******************************************************************************/

/**
 * U3V Host MAX number calculation for 2 operands.
 * 
 * @warning Operands must be of the same type.
 */
#define U3VDRV_MAX(a, b)         (((a) > (b)) ? (a) : (b))

/**
 * U3V Host MIN number calculation for 2 operands.
 * 
 * @warning Operands must be of the same type.
 */
#define U3VDRV_MIN(a, b)         (((a) < (b)) ? (a) : (b))


/*******************************************************************************
* Type definitions
*******************************************************************************/

/**
 * Area of struct typedefs with forced 1 byte packing.
 * 
 * For datatypes used in USB3 Vision protocol frames which require strict 1 byte 
 * packing when being sent over USB bus.
 */
#pragma pack(push, 1)

/**
 * U3V Control Interface ACK header.
 * 
 */
typedef struct
{
	uint32_t    prefix;
	uint16_t    status;
	uint16_t    cmd;
	uint16_t    length;
	uint16_t    ackId;
} T_U3VCtrlIfAckHeader;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfAckHeader) == 1), "Packing error for T_U3VCtrlIfAckHeader");

/**
 * U3V Control Interface ACK.
 * 
 */
typedef struct
{
	T_U3VCtrlIfAckHeader    header;
	uint8_t                 payload[];
} T_U3VCtrlIfAcknowledge;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfAcknowledge) == 1), "Packing error for T_U3VCtrlIfAcknowledge");

/**
 * U3V Control Interface pending ACK payload.
 * 
 */
typedef struct
{
	uint16_t    reserved;
	uint16_t    timeout;
} T_U3VCtrlIfPendingAckPayload;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfPendingAckPayload) == 1), "Packing error for T_U3VCtrlIfPendingAckPayload");

/**
 * U3V Control Interface CMD header.
 * 
 */
typedef struct
{
	uint32_t    prefix;
	uint16_t    flags;
	uint16_t    cmd;
	uint16_t    length;
	uint16_t    requestId;
} T_U3VCtrlIfCmdHeader;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfCmdHeader) == 1), "Packing error for T_U3VCtrlIfCmdHeader");

/**
 * U3V Control Interface CMD.
 * 
 */
typedef struct
{
	T_U3VCtrlIfCmdHeader    header;
	uint8_t                 payload[];
} T_U3VCtrlIfCommand;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfCommand) == 1), "Packing error for T_U3VCtrlIfCommand");

/**
 * U3V Control Interface read memory CMD payload.
 * 
 */
typedef struct 
{
	uint64_t    address;
	uint16_t    reserved;
	uint16_t    byteCount;
} T_U3VCtrlIfReadMemCmdPayload;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfReadMemCmdPayload) == 1), "Packing error for T_U3VCtrlIfReadMemCmdPayload");

/**
 * U3V Control Interface write memory CMD payload.
 * 
 */
typedef struct
{
	uint64_t    address;
	uint8_t     data[];
} T_U3VCtrlIfWriteMemCmdPayload;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfWriteMemCmdPayload) == 1), "Packing error for T_U3VCtrlIfWriteMemCmdPayload");

/**
 * U3V Control Interface write memory ACK payload.
 * 
 */
typedef struct
{
	uint16_t    reserved;
	uint16_t    bytesWritten;
} T_U3VCtrlIfWriteMemAckPayload;

U3V_STATIC_ASSERT((alignof(T_U3VCtrlIfWriteMemAckPayload) == 1), "Packing error for T_U3VCtrlIfWriteMemAckPayload");

/* end of forced 1 byte packing */
#pragma pack(pop)


/**
 * U3V String buffer type for text descriptors.
 * 
 * For handling uint8_t data or string related operations when accessing device
 * text descriptors. 
 */
typedef union 
{
    uint8_t     asU8[U3V_MAX_DESCR_STR_LENGTH];
    char        asChar[U3V_MAX_DESCR_STR_LENGTH];
} T_U3VStringBuffer;

U3V_STATIC_ASSERT((sizeof(T_U3VStringBuffer) == sizeof(uint8_t[U3V_MAX_DESCR_STR_LENGTH])), "Alignment of uint8_t and char arrays not equal");


/**
 * U3V Host transfer complete callback handler.
 * 
 */
typedef void (*T_U3VHostTransfCompleteHandler)(T_U3VHostHandle ctrlIfObj, T_U3VHostEvent transfEvent, void *transfData);

/**
 * U3V Host state.
 * 
 * States for the tasks state machine of U3V Host.
 */
typedef enum
{
    U3V_HOST_STATE_ERROR                    = -1,
    U3V_HOST_STATE_NOT_READY                =  0,
    U3V_HOST_STATE_SET_CONFIGURATION,
    U3V_HOST_STATE_WAIT_FOR_CONFIG_SET,
    U3V_HOST_STATE_WAIT_FOR_INTERFACES,
    U3V_HOST_STATE_READY,
} T_U3VHostState;

/**
 * U3V Host interface handle.
 * 
 * Used to manipulate Control, Event and Streaming interface.
 */
typedef struct
{
    uint8_t                             idNum;
    USB_HOST_DEVICE_INTERFACE_HANDLE    ifHandle;
    USB_HOST_PIPE_HANDLE                bulkInPipeHandle;
    USB_HOST_PIPE_HANDLE                bulkOutPipeHandle;
} T_U3VHostInterfHandle;

/**
 * U3V Host Control Interface object.
 * 
 * Contains all essential data for the Control Interface local operations.
 */
typedef struct 
{
    T_U3VHostInterfHandle               *ctrlIntfHandle;
	OSAL_MUTEX_DECLARE                  (readWriteLock); //TODO: possibly use FreeRTOS mutex instead?
	uint8_t                             ackBuffer[128U + sizeof(T_U3VCtrlIfAckHeader)];
	uint32_t                            maxAckTransfSize;
	uint8_t                             cmdBuffer[128U + sizeof(T_U3VCtrlIfCmdHeader)];
	uint32_t                            maxCmdTransfSize;
	uint16_t                            requestId;
	uint16_t                            maxRequestId;
	uint32_t                            u3vTimeout;     /* ms */
    T_U3VHostTransfCompleteHandler      transfReqCompleteCbk;
	T_U3VHostEventReadCompleteData      readReqSts;
	T_U3VHostEventWriteCompleteData     writeReqSts;
} T_U3VControlIfObj;

/**
 * U3V Host instance object. 
 * 
 * Contains all essential data for the U3V Host local operations.
 */
typedef struct
{
    bool                                inUse;
    USB_HOST_DEVICE_CLIENT_HANDLE       deviceClientHandle;
    USB_HOST_DEVICE_OBJ_HANDLE          deviceObjHandle;
    USB_HOST_PIPE_HANDLE                controlPipeHandle;
    USB_SETUP_PACKET                    setupPacket;
    T_U3VHostState                      state;
    T_U3VDeviceInfo                     u3vDevInfo;
    T_U3VControlIfObj                   controlIfObj;
    T_U3VHostInterfHandle               controlIfHandle;
    T_U3VHostInterfHandle               eventIfHandle;
    T_U3VHostInterfHandle               streamIfHandle;
    uintptr_t                           context;
    T_U3VHostEventHandler               eventHandler;
    T_U3VHostDetachEventHandler         detachEventHandler;
    bool                                hostRequestDone;
    USB_HOST_RESULT                     hostRequestResult;
} T_U3VHostInstanceObj;

/**
 * U3V Host attach listener object.
 * 
 */
typedef struct
{
    bool                                inUse;
    T_U3VHostAttachEventHandler         eventHandler;
    uintptr_t                           context;
} T_U3VHostAttachListenerObj;


#ifdef __cplusplus
}
#endif //__cplusplus

