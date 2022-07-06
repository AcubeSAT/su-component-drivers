//
// Created by fomarko on 04/02/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H
#define COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "U3VCam_Host.h"
#include "U3VCam_Config.h"
#include "osal/osal.h"


/********************************************************
* Macro definitions
*********************************************************/

#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#define MIN(a, b)         (((a) < (b)) ? (a) : (b))


/********************************************************
* Type definitions
*********************************************************/

typedef void (*T_U3VCtrlIfTransfCompleteHandler)(T_U3VHostHandle u3vObj, T_U3VHostEvent transfEvent, void *transfData);

typedef enum
{
    U3V_HOST_STATE_ERROR                        = -1,       /* Error state */
    U3V_HOST_STATE_NOT_READY                    =  0,       /* The instance is not ready */
    U3V_HOST_STATE_SET_CONFIGURATION,                       /* The instance should set the configuration */
    U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET,              /* Wait for configuration set */
    U3V_HOST_STATE_WAIT_FOR_INTERFACES,                     /* Wait for interfaces to get ready */
    U3V_HOST_STATE_READY,                                   /* The instance is ready */
} T_U3VHostState;

typedef struct
{
    uint8_t                             idNum;              /* Number of interface */
    USB_HOST_DEVICE_INTERFACE_HANDLE    ifHandle;           /* Interface handle */
    USB_HOST_PIPE_HANDLE                bulkInPipeHandle;   /* Bulk in pipe handle */
    USB_HOST_PIPE_HANDLE                bulkOutPipeHandle;  /* Bulk out pipe handle */
} T_U3VHostChannelHandle;

typedef struct 
{
	T_U3VHostObject 					u3vInstObj;				/* uintptr_t to the device object where Control Channel belongs to */
	OSAL_MUTEX_DECLARE					(readWriteLock);  		/* Mutex lock/unlock */
	uint8_t 							*ackBuffer;				/* Acknowledge buffer pointer (alloc) */
	uint32_t							maxAckTransfSize;		/* Max acknowledge transfer size */
	uint8_t 							*cmdBuffer;				/* Command buffer pointer (alloc) */
	uint32_t 							maxCmdTransfSize;		/* Max command transfer size */
	uint16_t 							requestId;				/* Request ID */
	uint16_t 							maxRequestId;  			/* Maximum id value before loop back around */
	uint32_t 							u3vTimeout;    			/* Maximum device response time in ms */
	T_U3VCtrlIfTransfCompleteHandler	transReqCompleteCbk;	/* Transfer event complete callback */
	T_U3VHostEventReadCompleteData 		readReqSts;				/* Read request transfer status */
	T_U3VHostEventWriteCompleteData 	writeReqSts;			/* Write request transfer status */
} T_U3VControlIfObj;

typedef struct
{
    uint8_t                             idNum;              /* Number of interface */
    USB_HOST_DEVICE_INTERFACE_HANDLE    ifHandle;           /* Interface handle */
    USB_HOST_PIPE_HANDLE                bulkInPipeHandle;   /* Bulk in pipe handle */
    USB_HOST_PIPE_HANDLE                bulkOutPipeHandle;  /* Bulk out pipe handle */
    T_U3VControlIfObj                   *chanObj;           /* Control channel object data */
} T_U3VHostControlChannelHandle;

typedef struct
{
    bool                                inUse;              /* True if object is in use */
    USB_HOST_DEVICE_CLIENT_HANDLE       deviceClientHandle; /* Device client handle */
    USB_HOST_DEVICE_OBJ_HANDLE          deviceObjHandle;    /* Device object handle */
    USB_HOST_PIPE_HANDLE                controlPipeHandle;  /* Control Pipe Handle */
    T_U3VDeviceInfo                     u3vDevInfo;         /* U3V Device Information */
    char                                *u3vManifestData;   /* U3V Manifest */
    T_U3VHostControlChannelHandle       controlChHandle;    /* U3V Control Channel handle */
    T_U3VHostChannelHandle              eventChHandle;      /* U3V Event Channel handle */
    T_U3VHostChannelHandle              streamChHandle;     /* U3V Stream Channel handle */
    USB_SETUP_PACKET                    setupPacket;        /* Setup packet information */
    uintptr_t                           context;            /* Application defined context */
    T_U3VHostEventHandler               eventHandler;       /* Application callback */
    T_U3VHostDetachEventHandler         detachEventHandler; /* Detach event app callback */
    T_U3VHostState                      state;              /* U3V instance state */
    bool                                hostRequestDone;    /* True if an ongoing host request is done */
    USB_HOST_RESULT                     hostRequestResult;  /* Result of the host request */
} T_U3VHostInstanceObj;

typedef struct
{
    bool                                inUse;              /* This object is in use */
    T_U3VHostAttachEventHandler         eventHandler;       /* The attach event handler */
    uintptr_t                           context;            /* Client context */
} T_UsbHostU3VAttachListenerObj;

typedef struct 
{
	uint16_t    fileVersionSubminor;
	uint8_t     fileVersionMinor;
	uint8_t     fileVersionMajor;
	uint32_t    schema;
	uint64_t    address;
	uint64_t    size;
	uint64_t    unknown3;
	uint64_t    unknown4;
	uint64_t    unknown5;
	uint64_t    unknown6;
	uint64_t    unknown7;
} T_U3VManifestEntry;

typedef enum 
{
    U3V_MANIFEST_SCHEMA_RAW = 0x00,
    U3V_MANIFEST_SCHEMA_ZIP = 0x01
} T_U3VManifestSchema;


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

T_U3VHostResult _U3VHost_HostToU3VResultsMap(USB_HOST_RESULT hostResult);

T_U3VHostResult U3VHost_CtrlIf_ReadMemory(T_U3VControlIfHandle u3vCtrlIf,
										  T_U3VHostTransferHandle *transferHandle,
										  uint64_t memAddress,
										  size_t transfSize,
										  uint32_t *bytesRead,
										  void *buffer);

T_U3VHostResult U3VHost_CtrlIf_WriteMemory(T_U3VControlIfHandle u3vCtrlIf,
										   T_U3VHostTransferHandle *transferHandle,
										   uint64_t memAddress,
										   size_t transfSize,
										   uint32_t *bytesWritten,
										   const void *buffer);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_HOST_LOCAL_H
