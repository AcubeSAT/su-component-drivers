//
// Created by fomarko on 17/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_CONTROL_IF_TYPES_H
#define COMPONENT_DRIVERS_U3VCAM_CONTROL_IF_TYPES_H


#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/


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



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_CONTROL_IF_TYPES_H
