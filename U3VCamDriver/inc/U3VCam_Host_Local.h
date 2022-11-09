
#ifndef COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H
#define COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H


#include "U3VCam_Host.h"
#include "osal/osal.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

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


/********************************************************
* Type definitions
*********************************************************/

#pragma pack(push, 1)

/**
 * U3V Control Interface ACK header.
 * 
 */
typedef struct
{
	uint32_t 	prefix;
	uint16_t 	status;
	uint16_t 	cmd;
	uint16_t 	length;
	uint16_t 	ackId;
} T_U3VCtrlIfAckHeader;

/**
 * U3V Control Interface ACK.
 * 
 */
typedef struct
{
	T_U3VCtrlIfAckHeader    header;
	uint8_t                 payload[];
} T_U3VCtrlIfAcknowledge;

/**
 * U3V Control Interface pending ACK payload.
 * 
 */
typedef struct
{
	uint16_t 	reserved;
	uint16_t 	timeout;
} T_U3VCtrlIfPendingAckPayload;

/**
 * U3V Control Interface CMD header.
 * 
 */
typedef struct
{
	uint32_t 	prefix;
	uint16_t 	flags;
	uint16_t 	cmd;
	uint16_t 	length;
	uint16_t 	requestId;
} T_U3VCtrlIfCmdHeader;

/**
 * U3V Control Interface CMD.
 * 
 */
typedef struct
{
	T_U3VCtrlIfCmdHeader    header;
	uint8_t                 payload[];
} T_U3VCtrlIfCommand;

/**
 * U3V Control Interface read memory CMD payload.
 * 
 */
typedef struct 
{
	uint64_t 	address;
	uint16_t 	reserved;
	uint16_t 	byteCount;
} T_U3VCtrlIfReadMemCmdPayload;

/**
 * U3V Control Interface write memory CMD payload.
 * 
 */
typedef struct
{
	uint64_t    address;
	uint8_t     data[];
} T_U3VCtrlIfWriteMemCmdPayload;

/**
 * U3V Control Interface write memory ACK payload.
 * 
 */
typedef struct
{
	uint16_t 	reserved;
	uint16_t 	bytesWritten;
} T_U3V_CtrlIfWriteMemAckPayload;

#pragma pack(pop)

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
    U3V_HOST_STATE_ERROR                        = -1,
    U3V_HOST_STATE_NOT_READY                    =  0,
    U3V_HOST_STATE_SET_CONFIGURATION,
    U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET,
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
	OSAL_MUTEX_DECLARE					(readWriteLock); //TODO: possibly use FreeRTOS mutex instead?
	uint8_t 							ackBuffer[128U + sizeof(T_U3VCtrlIfAckHeader)];
	uint32_t							maxAckTransfSize;
	uint8_t 							cmdBuffer[128U + sizeof(T_U3VCtrlIfCmdHeader)];
	uint32_t 							maxCmdTransfSize;
	uint16_t 							requestId;
	uint16_t 							maxRequestId;
	uint32_t 							u3vTimeout;     /* ms */
    T_U3VHostTransfCompleteHandler      transfReqCompleteCbk;
	T_U3VHostEventReadCompleteData 		readReqSts;
	T_U3VHostEventWriteCompleteData 	writeReqSts;
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


/********************************************************
* Local data
*********************************************************/

static T_U3VCamRegisterCfg u3vCamRegisterCfg =
{
	.camRegBaseAddress				= (uint64_t)U3V_CFG_MDL_CAM_REG_BASE_ADDRESS,
    .SBRMOffset                     = (uint64_t)U3V_CFG_MDL_SBRM_OFFSET,
    .temperature_Reg                = (uint64_t)U3V_CFG_MDL_TEMPERATURE_REG,
    .deviceReset_Reg                = (uint64_t)U3V_CFG_MDL_DEVICE_RESET_REG,
    .singleFrameAcquisitionMode_Reg = (uint64_t)U3V_CFG_MDL_SINGLE_FRAME_ACQ_MODE_REG,
    .acquisitionMode_Reg            = (uint64_t)U3V_CFG_MDL_ACQ_MODE_REG,
    .acquisitionStart_Reg           = (uint64_t)U3V_CFG_MDL_ACQ_START_REG,
    .acquisitionStop_Reg            = (uint64_t)U3V_CFG_MDL_ACQ_STOP_REG,
    .colorCodingID_Reg              = (uint64_t)U3V_CFG_MDL_COLOR_CODING_ID_REG,
    .payloadSizeVal_Reg             = (uint64_t)U3V_CFG_MDL_PAYLOAD_SIZE_VAL_REG,
    .pixelFormatCtrlVal_Int_Sel     = (uint32_t)U3V_CFG_MDL_PIXEL_FORMAT_CTRL_INT_SEL 
};


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H
