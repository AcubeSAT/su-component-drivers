//
// Created by fomarko on 04/02/22.
//

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

#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#define MIN(a, b)         (((a) < (b)) ? (a) : (b))


/********************************************************
* Type definitions
*********************************************************/

#pragma pack(push, 1)

typedef struct
{
	uint32_t 	prefix;
	uint16_t 	status;
	uint16_t 	cmd;
	uint16_t 	length;
	uint16_t 	ackId;
} T_U3VCtrlIfAckHeader;

typedef struct
{
	T_U3VCtrlIfAckHeader 	header;
	uint8_t 				payload[0];
} T_U3VCtrlIfAcknowledge;

typedef struct
{
	uint16_t 	reserved;
	uint16_t 	timeout;
} T_U3VCtrlIfPendingAckPayload;

typedef struct
{
	uint32_t 	prefix;
	uint16_t 	flags;
	uint16_t 	cmd;
	uint16_t 	length;
	uint16_t 	requestId;
} T_U3VCtrlIfCmdHeader;

typedef struct
{
	T_U3VCtrlIfCmdHeader 	header;
	uint8_t 				payload[0];
} T_U3VCtrlIfCommand;

typedef struct 
{
	uint64_t 	address;
	uint16_t 	reserved;
	uint16_t 	byteCount;
} T_U3VCtrlIfReadMemCmdPayload;

typedef struct
{
	uint64_t 	address;
	uint8_t  	data[0];
} T_U3VCtrlIfWriteMemCmdPayload;

typedef struct
{
	uint16_t 	reserved;
	uint16_t 	bytesWritten;
} T_U3V_CtrlIfWriteMemAckPayload;

#pragma pack(pop)

typedef void (*T_U3VHostTransfCompleteHandler)(T_U3VHostHandle ctrlIfObj, T_U3VHostEvent transfEvent, void *transfData);

typedef enum
{
    U3V_HOST_STATE_ERROR                        = -1,
    U3V_HOST_STATE_NOT_READY                    =  0,
    U3V_HOST_STATE_SET_CONFIGURATION,
    U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET,
    U3V_HOST_STATE_WAIT_FOR_INTERFACES,
    U3V_HOST_STATE_READY,
} T_U3VHostState;

typedef struct
{
    uint8_t                             idNum;
    USB_HOST_DEVICE_INTERFACE_HANDLE    ifHandle;
    USB_HOST_PIPE_HANDLE                bulkInPipeHandle;
    USB_HOST_PIPE_HANDLE                bulkOutPipeHandle;
} T_U3VHostInterfHandle;

typedef struct 
{
    T_U3VHostInterfHandle               *ctrlIntfHandle;
	OSAL_MUTEX_DECLARE					(readWriteLock);
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

typedef struct
{
    bool                                inUse;
    T_U3VHostAttachEventHandler         eventHandler;
    uintptr_t                           context;
} T_UsbHostU3VAttachListenerObj;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/

extern T_U3VHostInstanceObj gUSBHostU3VObj[U3V_HOST_INSTANCES_NUMBER];


/********************************************************
* Function declarations
*********************************************************/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H
