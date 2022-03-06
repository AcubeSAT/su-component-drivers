//
// Created by mojo on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_IF_SHAREDTYPES_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_IF_SHAREDTYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "UsbCamDrv_DeviceClassSpec_U3V.h"
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
	OSAL_MUTEX_DECLARE(readWriteLock);
	uint8_t *ackBuffer;
	uint32_t maxAckTransfSize;
	uint8_t *cmdBuffer;
	uint32_t maxCmdTransfSize;
	uint16_t requestId;
	uint16_t maxRequestId;  			/* Maximum id value before loop back around */
	uint32_t u3vTimeout;    			/* Maximum device response time in ms */
} T_U3V_ControlIntfStr;

typedef struct
{
	uint32_t prefix;
	uint16_t status;
	uint16_t cmd;
	uint16_t length;
	uint16_t ackId;
} T_U3V_CtrlAckHeader;

typedef struct
{
	T_U3V_CtrlAckHeader header;
	uint8_t payload[0];
} T_U3V_CtrlAcknowledge;

typedef struct
{
	uint16_t reserved;
	uint16_t timeout;
} T_U3V_CtrlPendingAckPayload;

typedef struct
{
	uint32_t prefix;
	uint16_t flags;
	uint16_t cmd;
	uint16_t length;
	uint16_t requestId;
} T_U3V_CtrlCmdHeader;

typedef struct
{
	T_U3V_CtrlCmdHeader header;
	uint8_t payload[0];
} T_U3V_CtrlCommand;

typedef struct 
{
	uint64_t address;
	uint16_t reserved;
	uint16_t byteCount;
} T_U3V_CtrlReadMemCmdPayload;

typedef struct
{
	uint64_t address;
	uint8_t  data[0];
} T_U3V_CtrlWriteMemCmdPayload;

typedef struct
{
	uint16_t reserved;
	uint16_t bytesWritten;
} T_U3V_CtrlWriteMemAckPayload;




#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_IF_SHAREDTYPES_H
