//
// Created by mojo on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_LOCAL_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_LOCAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "UsbCamDrv_Host_U3V.h"
#include "osal/osal.h"

#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/


/********************************************************
* Type definitions
*********************************************************/

typedef void (*T_U3VCtrlChTransfCompleteHandler)(T_U3VHostHandle u3vObj, T_U3VHostEvent transfEvent, void *transfData);

#pragma pack(push, 1)

typedef struct
{
	uint32_t prefix;
	uint16_t status;
	uint16_t cmd;
	uint16_t length;
	uint16_t ackId;
} T_U3VCtrlChAckHeader;

typedef struct
{
	T_U3VCtrlChAckHeader header;
	uint8_t payload[0];
} T_U3VCtrlChAcknowledge;

typedef struct
{
	uint16_t reserved;
	uint16_t timeout;
} T_U3VCtrlChPendingAckPayload;

typedef struct
{
	uint32_t prefix;
	uint16_t flags;
	uint16_t cmd;
	uint16_t length;
	uint16_t requestId;
} T_U3VCtrlChCmdHeader;

typedef struct
{
	T_U3VCtrlChCmdHeader header;
	uint8_t payload[0];
} T_U3VCtrlChCommand;

typedef struct 
{
	uint64_t address;
	uint16_t reserved;
	uint16_t byteCount;
} T_U3VCtrlChReadMemCmdPayload;

typedef struct
{
	uint64_t address;
	uint8_t  data[0];
} T_U3VCtrlChWriteMemCmdPayload;

typedef struct
{
	uint16_t reserved;
	uint16_t bytesWritten;
} T_U3V_CtrlChWriteMemAckPayload;

#pragma pack(pop)

typedef struct 
{
	T_U3VHostObject 				u3vInstObj;				/* uintptr_t to the device object where Control Channel belongs to */
	OSAL_MUTEX_DECLARE				(readWriteLock);  		/* Mutex lock/unlock */
	uint8_t 						*ackBuffer;				/* Acknowledge buffer pointer (alloc) */
	uint32_t						maxAckTransfSize;		/* Max acknowledge transfer size */
	uint8_t 						*cmdBuffer;				/* Command buffer pointer (alloc) */
	uint32_t 						maxCmdTransfSize;		/* Max command transfer size */
	uint16_t 						requestId;				/* Request ID */
	uint16_t 						maxRequestId;  			/* Maximum id value before loop back around */
	uint32_t 						u3vTimeout;    			/* Maximum device response time in ms */
	T_U3VCtrlChTransfCompleteHandler transReqCompleteCbk;	/* Transfer event complete callback */
	T_U3VHostEventReadCompleteData 	readReqSts;				/* Read request transfer status */
	T_U3VHostEventWriteCompleteData writeReqSts;			/* Write request transfer status */
} T_U3VControlChannelObj;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/

void _U3VHost_CtrlCh_TransferReqCompleteCbk(T_U3VHostHandle u3vObj, T_U3VHostEvent transfEvent, void *transfData);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_LOCAL_H
