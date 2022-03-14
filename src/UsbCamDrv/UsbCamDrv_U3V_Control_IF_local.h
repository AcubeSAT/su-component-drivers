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

typedef struct 
{
	T_U3VHostObject 				u3vInstObj;				/* uintptr_t to the device object where Control IF belongs to */
	OSAL_MUTEX_DECLARE				(readWriteLock);  		/* Mutex lock/unlock */
	uint8_t 						*ackBuffer;
	uint32_t						maxAckTransfSize;
	uint8_t 						*cmdBuffer;
	uint32_t 						maxCmdTransfSize;
	uint16_t 						requestId;
	uint16_t 						maxRequestId;  			/* Maximum id value before loop back around */
	uint32_t 						u3vTimeout;    			/* Maximum device response time in ms */
	// T_U3VHostEventReadCompleteData 	readReqSts;
	// T_U3VHostEventWriteCompleteData writeReqSts;
} T_U3VControlInterfaceObj;

typedef struct
{
	uint32_t prefix;
	uint16_t status;
	uint16_t cmd;
	uint16_t length;
	uint16_t ackId;
} T_U3VCtrlIfAckHeader;

typedef struct
{
	T_U3VCtrlIfAckHeader header;
	uint8_t payload[0];
} T_U3VCtrlIfAcknowledge;

typedef struct
{
	uint16_t reserved;
	uint16_t timeout;
} T_U3VCtrlIfPendingAckPayload;

typedef struct
{
	uint32_t prefix;
	uint16_t flags;
	uint16_t cmd;
	uint16_t length;
	uint16_t requestId;
} T_U3VCtrlIfCmdHeader;

typedef struct
{
	T_U3VCtrlIfCmdHeader header;
	uint8_t payload[0];
} T_U3VCtrlIfCommand;

typedef struct 
{
	uint64_t address;
	uint16_t reserved;
	uint16_t byteCount;
} T_U3VCtrlIfReadMemCmdPayload;

typedef struct
{
	uint64_t address;
	uint8_t  data[0];
} T_U3VCtrlIfWriteMemCmdPayload;

typedef struct
{
	uint16_t reserved;
	uint16_t bytesWritten;
} T_U3V_CtrlWriteMemAckPayload;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/


/********************************************************
* Function declarations
*********************************************************/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CONTROL_IF_LOCAL_H
