//
// Created by mojo on 04/02/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_LOCAL_H
#define COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_LOCAL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "UsbCamDrv_Host_U3V.h"
#include "UsbCamDrv_U3V_Control_IF_local.h"
#include "UsbCamDrv_Config.h"


/********************************************************
* Macro definitions
*********************************************************/

#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#define MIN(a, b)         (((a) < (b)) ? (a) : (b))


/********************************************************
* Type definitions
*********************************************************/

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
    uint8_t                             idNum;              /* Number of interface */
    USB_HOST_DEVICE_INTERFACE_HANDLE    ifHandle;           /* Interface handle */
    USB_HOST_PIPE_HANDLE                bulkInPipeHandle;   /* Bulk in pipe handle */
    USB_HOST_PIPE_HANDLE                bulkOutPipeHandle;  /* Bulk out pipe handle */
    T_U3VControlChannelObj              *chanObj;           /* Control channel object data */
} T_U3VHostControlChannelHandle;

typedef struct
{
    bool                                inUse;              /* True if object is in use */
    USB_HOST_DEVICE_CLIENT_HANDLE       deviceClientHandle; /* Device client handle */
    USB_HOST_DEVICE_OBJ_HANDLE          deviceObjHandle;    /* Device object handle */
    USB_HOST_PIPE_HANDLE                controlPipeHandle;  /* Control Pipe Handle */
    T_U3VDeviceInfo                     u3vDevInfo;         /* U3V Device Information */
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

static void _USB_HostU3V_Initialize(void * data);
 
static void _USB_HostU3V_Deinitialize(void);
 
static void _USB_HostU3V_Reinitialize(void * msdInitData);

static void _USB_HostU3V_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                      USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                      USB_DEVICE_DESCRIPTOR *deviceDescriptor);

static void _USB_HostU3V_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void _USB_HostU3V_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle);

static void _USB_HostU3V_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
                                         USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                         size_t nInterfaces,
                                         uint8_t *descriptor);

static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _USB_HostU3V_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                                   USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                                   void *eventData,
                                                                                   uintptr_t context);

static void _USB_HostU3V_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static void _USB_HostU3V_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

static USB_HOST_DEVICE_EVENT_RESPONSE _USB_HostU3V_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                      USB_HOST_DEVICE_EVENT event,
                                                                      void *eventData,
                                                                      uintptr_t context);

static int32_t _USB_HostU3V_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle);

static int32_t _USB_HostU3V_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle);

static int32_t _USB_HostU3V_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);

T_U3VHostResult _USB_HostU3V_HostToU3VResultsMap(USB_HOST_RESULT hostResult);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_HOST_U3V_LOCAL_H
