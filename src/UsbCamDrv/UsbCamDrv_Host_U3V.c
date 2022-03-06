//
// Created by mojo on 04/02/22.
//

#include "UsbCamDrv_Host_U3V.h"
#include "UsbCamDrv_Host_U3V_Local.h"
#include "UsbCamDrv_Config.h"
#include "UsbCamDrv_DeviceClassSpec_U3V.h"


/********************************************************
* Local function declarations
*********************************************************/


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_UsbHostU3VInstanceObj gUSBHostU3VObj[USB_HOST_U3V_INSTANCES_NUMBER];

T_UsbHostU3VAttachListenerObj gUSBHostU3VAttachListener[USB_HOST_U3V_ATTACH_LISTENERS_NUMBER];

USB_HOST_CLIENT_DRIVER gUSBHostU3VClientDriver =
{
    .initialize             = _USB_HostU3V_Initialize,
    .deinitialize           = _USB_HostU3V_Deinitialize,
    .reinitialize           = _USB_HostU3V_Reinitialize,
    .interfaceAssign        = _USB_HostU3V_InterfaceAssign,
    .interfaceRelease       = _USB_HostU3V_InterfaceRelease,
    .interfaceEventHandler  = _USB_HostU3V_InterfaceEventHandler,
    .interfaceTasks         = _USB_HostU3V_InterfaceTasks,
    .deviceEventHandler     = _USB_HostU3V_DeviceEventHandler,
    .deviceAssign           = _USB_HostU3V_DeviceAssign,
    .deviceRelease          = _USB_HostU3V_DeviceRelease,
    .deviceTasks            = _USB_HostU3V_DeviceTasks
};



/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult USB_U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context)
{
    int32_t iterator;

    T_U3VHostResult result = U3V_HOST_RESULT_FAILURE;
    T_UsbHostU3VAttachListenerObj  *attachListener;

    if (eventHandler == NULL)
    {
        result = U3V_HOST_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Search for free listener object */
        for (iterator = 0; iterator < USB_HOST_U3V_ATTACH_LISTENERS_NUMBER; iterator++)
        {
            if (!gUSBHostU3VAttachListener[iterator].inUse)
            {
                /* Found a free object */
                attachListener = &gUSBHostU3VAttachListener[iterator];
                attachListener->inUse = true;
                attachListener->eventHandler = eventHandler;
                attachListener->context = context;
                result = U3V_HOST_RESULT_SUCCESS;
                break;
            }
        }
    }
    return result;
}


T_U3VHostDeviceObjHandle USB_U3VHost_DeviceObjectHandleGet(T_U3VHostObject u3vDeviceObj)
{
    //todo
}
                               

T_U3VHostHandle USB_U3VHost_Open(T_U3VHostObject u3vDeviceObj)
{
    T_U3VHostHandle result = U3V_HOST_HANDLE_INVALID;
    T_UsbHostU3VInstanceObj *u3vInstance;

    /* Single client only */
    if (u3vDeviceObj != 0)
    {
        u3vInstance = (T_UsbHostU3VInstanceObj *)u3vDeviceObj;
        if ((u3vInstance->inUse) && (u3vInstance->state == U3V_HOST_STATE_READY))
        {
            result = (T_U3VHostHandle)(u3vDeviceObj);
        }
    }
    return result;
}


void USB_U3VHost_Close(T_U3VHostHandle u3vDeviceHandle)
{
    //todo
}


T_U3VHostResult USB_U3VHost_EventHandlerSet(T_U3VHostHandle handle, T_U3VHostEventHandler eventHandler, uintptr_t context)
{
    T_U3VHostResult result = U3V_HOST_RESULT_HANDLE_INVALID;
    T_UsbHostU3VInstanceObj *u3vInstance = (T_UsbHostU3VInstanceObj *)(handle);

    if (u3vInstance != NULL)
    {
        u3vInstance->eventHandler = eventHandler;
        u3vInstance->context = context;
        result = U3V_HOST_RESULT_SUCCESS;
    }
    return result;
}


T_U3VHostResult USB_U3VHost_Read(T_U3VHostHandle          handle,
                                    T_U3VHostTransferHandle *transferHandle,
                                    void                       *data,
                                    size_t                      size)
{
    T_UsbHostU3VInstanceObj *u3vInstance;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_FAILURE;
    USB_HOST_RESULT hostResult;

    u3vInstance = (T_UsbHostU3VInstanceObj *)handle;

    if (u3vInstance == NULL)
    {
        u3vResult = U3V_HOST_RESULT_HANDLE_INVALID;
    }
    else
    {
        /* Check if the specified transfer handle holder is NULL, if so use a local transfer handle holder */
        tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;

        if (!u3vInstance->inUse)
        {
            /* This object is not valid */
            u3vResult = U3V_HOST_RESULT_DEVICE_UNKNOWN;
        }
        else
        {
            if (u3vInstance->state != U3V_HOST_STATE_READY)
            {
                /* The instance is not ready for requests */
                u3vResult = U3V_HOST_RESULT_BUSY;
            }
            else
            {
                if((size != 0) && (data == NULL))
                {
                    /* Input parameters are not valid */
                    u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                }
                else
                {
                    /* The context for the transfer is the event that needs to
                     * be sent to the application. In this case the event to be
                     * sent to the application when the transfer completes is
                     * USB_HOST_U3V_EVENT_READ_COMPLETE */
                    hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkInPipeHandle,
                                                         tempTransferHandle,
                                                         data,
                                                         size,
                                                         (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));
                    u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);
                }
            }
        }
    }
    return u3vResult;
}

T_U3VHostResult USB_U3VHost_Write(T_U3VHostHandle          handle,
                                     T_U3VHostTransferHandle *transferHandle,
                                     void                       *data,
                                     size_t                      size)
{
    T_UsbHostU3VInstanceObj *u3vInstance;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_FAILURE;
    USB_HOST_RESULT hostResult;

    u3vInstance = (T_UsbHostU3VInstanceObj *)handle;

    if (u3vInstance == NULL)
    {
        u3vResult = U3V_HOST_RESULT_HANDLE_INVALID;
    }
    else
    {
        /* Check if the specified transfer handle holder is NULL, if so use a local transfer handle holder */
        tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;

        if (!u3vInstance->inUse)
        {
            /* This object is not valid */
            u3vResult = U3V_HOST_RESULT_DEVICE_UNKNOWN;
        }
        else
        {
            if (u3vInstance->state != U3V_HOST_STATE_READY)
            {
                /* The instance is not ready for requests */
                u3vResult = U3V_HOST_RESULT_BUSY;
            }
            else
            {
                if ((size != 0u) && (data == NULL))
                {
                    /* Input parameters are not valid */
                    u3vResult = U3V_HOST_RESULT_INVALID_PARAMETER;
                }
                else
                {
                    /* The context for the transfer is the event that needs to
                     * be sent to the application. In this case the event to be
                     * sent to the application when the transfer completes is
                     * U3V_HOST_EVENT_WRITE_COMPLETE */
                    hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkOutPipeHandle,
                                                         tempTransferHandle, data, size,
                                                         (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));
                    u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);
                }
            }
        }
    }
    return u3vResult;
}



/********************************************************
* Local function definitions
*********************************************************/

static void _USB_HostU3V_Initialize(void * data)
{
    uint32_t iterator;
    T_UsbHostU3VInstanceObj *u3vInstanceObj;

    for(iterator = 0u; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator ++)
    {
        /* Set the pipes handles to invalid */
        u3vInstanceObj = &gUSBHostU3VObj[iterator];

        u3vInstanceObj->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;


        u3vInstanceObj->controlIf.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->controlIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->controlIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;

        u3vInstanceObj->eventIf.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->eventIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->eventIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID; /* N/A */

        u3vInstanceObj->streamIf.ifHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstanceObj->streamIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        u3vInstanceObj->streamIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;   /* N/A */

        u3vInstanceObj->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
    }
}


static void _USB_HostU3V_Deinitialize(void)
{
    /* N/A */
}


static void _USB_HostU3V_Reinitialize(void * msdInitData)
{
    /* N/A */
}


static void _USB_HostU3V_InterfaceAssign(USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,         //todo rework
                                         USB_HOST_DEVICE_OBJ_HANDLE        deviceObjHandle,
                                         size_t                            nInterfaces,
                                         uint8_t                          *descriptor)
{
    int32_t                             u3vInstanceIndex;
    uint32_t                            iterator;
    T_UsbHostU3VInstanceObj            *u3vInstance = NULL;
    USB_INTERFACE_DESCRIPTOR           *interfaceDescriptor;
    USB_ENDPOINT_DESCRIPTOR            *endpointDescriptor;
    USB_HOST_ENDPOINT_DESCRIPTOR_QUERY  endpointDescriptorQuery;
    USB_HOST_INTERFACE_DESCRIPTOR_QUERY interfaceDescriptorQuery;

    /* Expected number of interfaces = 3 for U3V device (Control/Event/Streaming) */
    if (nInterfaces > 1u) 
    {
        /* Then this means that this is an IAD. We first assign a U3V instance to this device */

        for (iterator = 0u; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator++)
        {
            if (!gUSBHostU3VObj[iterator].inUse)
            {
                u3vInstance = &gUSBHostU3VObj[iterator];
                u3vInstance->inUse = true;
                break;
            }
        }

        if (u3vInstance == NULL)
        {
            /* Instance not found. Release all interfaces */
            for (iterator = 0u; iterator < nInterfaces; iterator++)
            {
                USB_HOST_DeviceInterfaceRelease(interfaces[iterator]);
            }
        }
        else
        {
            /* Save the device object handle and open the control pipe */
            u3vInstance->deviceObjHandle = deviceObjHandle;
            u3vInstance->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);

            /* An instance is assigned. The descriptor will be a pointer to the IAD.
             * Lets get the first interface descriptor in the IAD group and see which interface this is. */
            USB_HOST_DeviceInterfaceQueryContextClear(&interfaceDescriptorQuery);
            interfaceDescriptorQuery.flags = USB_HOST_INTERFACE_QUERY_ANY;

            /* We know that we need two interfaces */
            for (iterator = 0u; iterator < nInterfaces; iterator++)
            {
                /* We need to search for two interface descriptors */
                interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)(descriptor),
                                                                                     &interfaceDescriptorQuery);

                /* If we have a valid interface descriptor find out its type */
                if (interfaceDescriptor != NULL)
                {
                    /* Control Interface */
                    if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                        (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                        (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_CONTROL))
                    {
                        /* We found the communication class */
                        u3vInstance->controlIf.idNum = interfaceDescriptor->bInterfaceNumber;
                        u3vInstance->controlIf.ifHandle = interfaces[iterator];

                        /* Create the endpoint query */
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            /* Open the pipe */
                            u3vInstance->controlIf.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->controlIf.ifHandle,
                                                                                              endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make sure that the pipe handle stays invalid if we could not open the pipe */
                            u3vInstance->controlIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* Get the bulk out endpoint */
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_HOST_TO_DEVICE;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->controlIf.bulkOutPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->controlIf.ifHandle,
                                                                                               endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make the pipe handle invalid */
                            u3vInstance->controlIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                    }
                    /* Event Interface*/
                    else if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                             (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                             (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_EVENT))
                    {
                        /* We found the data class */

                        u3vInstance->eventIf.idNum = interfaceDescriptor->bInterfaceNumber;
                        u3vInstance->eventIf.ifHandle = interfaces[iterator];

                        /* Get the bulk in endpoint */ 
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->eventIf.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->eventIf.ifHandle,
                                                                                    endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make the pipe handle invalid */
                            u3vInstance->eventIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* no EP OUT in Event interface*/
                    }
                    /* Data Streaming Interface */
                    else if ((interfaceDescriptor->bInterfaceClass == U3V_DEVICE_CLASS_MISC) &&
                             (interfaceDescriptor->bInterfaceSubClass == U3V_INTERFACE_U3V_SUBLCASS) &&
                             (interfaceDescriptor->bInterfaceProtocol == U3V_INTERFACE_DATASTREAM))
                    {
                        /* We found the data class */

                        u3vInstance->streamIf.ifHandle = interfaces[iterator];
                        u3vInstance->streamIf.idNum = interfaceDescriptor->bInterfaceNumber;

                        /* Get the bulk in endpoint */ 
                        USB_HOST_DeviceEndpointQueryContextClear(&endpointDescriptorQuery);
                        endpointDescriptorQuery.transferType = USB_TRANSFER_TYPE_BULK;
                        endpointDescriptorQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST;
                        endpointDescriptorQuery.flags = (USB_HOST_ENDPOINT_QUERY_FLAG)(USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE|USB_HOST_ENDPOINT_QUERY_BY_DIRECTION);
                        endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointDescriptorQuery);

                        if (endpointDescriptor != NULL)
                        {
                            u3vInstance->streamIf.bulkInPipeHandle = USB_HOST_DevicePipeOpen(u3vInstance->streamIf.ifHandle,
                                                                                    endpointDescriptor->bEndpointAddress);
                        }
                        else
                        {
                            /* Make the pipe handle invalid */
                            u3vInstance->streamIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
                        }
                        /* no EP OUT in Streaming interface*/
                    }
                    
                }
                else
                {
                    /* There have to be at least two interface descriptors */
                }
            }

            /* Now check if we can move the host client driver instance to a ready state */

            if ((u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->eventIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                (u3vInstance->streamIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
            {
                /* All the pipes are opened. The client driver is ready */
                u3vInstance->state = U3V_HOST_STATE_READY;

                /* We know that the client driver is now ready. We can
                 * let all the listeners know that the device has been
                 * attached.  */

                for (iterator = 0u; iterator < USB_HOST_U3V_ATTACH_LISTENERS_NUMBER; iterator++)
                {
                    if (gUSBHostU3VAttachListener[iterator].inUse)
                    {
                        /* Call the attach listener event handler 
                         * function. */
                        gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostObject)(u3vInstance),
                                                                         gUSBHostU3VAttachListener[iterator].context);
                    }
                }
            }
            else
            {
                /* Something went wrong. */
                u3vInstance->state = U3V_HOST_STATE_ERROR;
            }
        }
    }
    else
    {
        /* nInterfaces do not match to a U3V device */
    }
}


static void _USB_HostU3V_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    int32_t u3vIndex;
    T_UsbHostU3VInstanceObj * u3vInstance;
    
    /* Get the instance associated with this interface */
    u3vIndex = _USB_HostU3V_InterfaceHandleToInstance(interfaceHandle);

    if (u3vIndex >= 0u)
    {
        /* Get the pointer to the instance object */
        u3vInstance = &gUSBHostU3VObj[u3vIndex];
        
        if(u3vInstance->inUse)
        {
            u3vInstance->inUse = false;

            if(u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the controlIf.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->controlIf.bulkInPipeHandle);
                u3vInstance->controlIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->controlIf.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the controlIf.bulkOutPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->controlIf.bulkOutPipeHandle);
                u3vInstance->controlIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->eventIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the eventIf.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->eventIf.bulkInPipeHandle);
                u3vInstance->eventIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }
            
            if(u3vInstance->streamIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
            {
                /* Close the streamIf.bulkInPipeHandle and invalidate the pipe handle */
                USB_HOST_DevicePipeClose(u3vInstance->streamIf.bulkInPipeHandle);
                u3vInstance->streamIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
            }

            if(u3vInstance->eventHandler != NULL)
            {
                /* Let the client know that the device is detached */
                u3vInstance->eventHandler((T_U3VHostHandle)(u3vInstance),
                                          U3V_HOST_EVENT_DEVICE_DETACHED,
                                          NULL,
                                          u3vInstance->context);
            }

            /* Release the object */
            u3vInstance->eventHandler = NULL;
            u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
            u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
        }
    }
}


static USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _USB_HostU3V_InterfaceEventHandler(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
                                                                                   USB_HOST_DEVICE_INTERFACE_EVENT event,
                                                                                   void *eventData,
                                                                                   uintptr_t context)
{
    int32_t u3vIndex;
    T_UsbHostU3VInstanceObj *u3vInstance;
    USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA * dataTransferEvent;
    T_U3VHostEvent u3vEvent;
    T_U3VHostEventWriteCompleteData u3vTransferCompleteData;

    /* Find out to which U3V Instance this interface belongs */
    u3vIndex = _USB_HostU3V_InterfaceHandleToInstance(interfaceHandle);

    u3vInstance = &gUSBHostU3VObj[u3vIndex];
    u3vEvent = (T_U3VHostEvent)(context);

    switch (event)
    {
        case USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE:
            /* This means a data transfer has completed */
            dataTransferEvent = (USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *)(eventData);
            u3vTransferCompleteData.transferHandle = dataTransferEvent->transferHandle;
            u3vTransferCompleteData.result = _USB_HostU3V_HostToU3VResultsMap(dataTransferEvent->result);
            u3vTransferCompleteData.length = dataTransferEvent->length;

            if (u3vInstance->eventHandler != NULL)
            {
                u3vInstance->eventHandler((T_U3VHostHandle)(u3vInstance),
                                          u3vEvent,
                                          &u3vTransferCompleteData,
                                          u3vInstance->context);
            }
            break;

        default:
            break;
    }

    return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE;
}


static void _USB_HostU3V_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    /* N/A */
}


static USB_HOST_DEVICE_EVENT_RESPONSE _USB_HostU3V_DeviceEventHandler(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                                                      USB_HOST_DEVICE_EVENT event,
                                                                      void *eventData,
                                                                      uintptr_t context)
{
   /* The event context is the pointer to the U3V Instance Object */
   T_UsbHostU3VInstanceObj *u3vInstance = (T_UsbHostU3VInstanceObj *)(context);
   USB_HOST_DEVICE_EVENT_CONFIGURATION_SET_DATA *configSetEventData;

   switch (event)
   {
        case USB_HOST_DEVICE_EVENT_CONFIGURATION_SET:
            /* This means the configuration was set. Update the instance variables to let the main state machine know. */
            configSetEventData = (USB_HOST_DEVICE_EVENT_CONFIGURATION_SET_DATA *)(eventData);
            u3vInstance->hostRequestResult = configSetEventData->result;
            u3vInstance->hostRequestDone = true;
            break;
        
        case USB_HOST_DEVICE_EVENT_CONFIGURATION_DESCRIPTOR_GET_COMPLETE:
            break;
        
        default:
            break;
    }
    return USB_HOST_DEVICE_EVENT_RESPONSE_NONE;
}


static void _USB_HostU3V_DeviceAssign(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle,
                                      USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
                                      USB_DEVICE_DESCRIPTOR *deviceDescriptor)
{
    uint32_t iterator;
    T_UsbHostU3VInstanceObj *u3vInstanceObj = NULL;
    /* If this function is being called, this means that a device class subclass
     * protocol match was obtained.  Look for a free instance */
    for (iterator = 0u; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator++)
    {
        /* Search for an available U3V instance object */
        if (!gUSBHostU3VObj[iterator].inUse)
        {
            /* Allocate the object */
            u3vInstanceObj = &gUSBHostU3VObj[iterator];
            u3vInstanceObj->inUse = true;
            u3vInstanceObj->deviceObjHandle = deviceObjHandle;
            u3vInstanceObj->deviceClientHandle = deviceHandle;
            break;
        }
    }

    if(u3vInstanceObj == NULL)
    {
        /* This means an instance could not be allocated. Return the device back to the host */
        USB_HOST_DeviceRelease(deviceHandle);
    }
    else
    {
        /* An instance object was allocated. Check if the device has any configurations */
        if(deviceDescriptor->bNumConfigurations > 0)
        {
            /* This means we have configurations. We can try setting the first 
             * configuration. Also open the control pipe to the device. */
            u3vInstanceObj->state = U3V_HOST_STATE_SET_CONFIGURATION;
            u3vInstanceObj->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);
        }
        else
        {
            /* There are no configurations! Move to error state */
            u3vInstanceObj->state = U3V_HOST_STATE_ERROR;
        }
    }
}


static void _USB_HostU3V_DeviceRelease(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    int32_t index;
    T_UsbHostU3VInstanceObj *u3vInstance;

    /* Find the U3V instance object that owns this device */
    index = _USB_HostU3V_DeviceHandleToInstance(deviceHandle);

    if (index >= 0u)
    {
        u3vInstance = &gUSBHostU3VObj[index];
        u3vInstance->inUse = false;

        if (u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the controlIf.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->controlIf.bulkInPipeHandle);
            u3vInstance->controlIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->controlIf.bulkOutPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the controlIf.bulkOutPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->controlIf.bulkOutPipeHandle);
            u3vInstance->controlIf.bulkOutPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->eventIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the eventIf.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->eventIf.bulkInPipeHandle);
            u3vInstance->eventIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->streamIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
        {
            /* Close the streamIf.bulkInPipeHandle and invalidate the pipe handle */
            USB_HOST_DevicePipeClose(u3vInstance->streamIf.bulkInPipeHandle);
            u3vInstance->streamIf.bulkInPipeHandle = USB_HOST_PIPE_HANDLE_INVALID;
        }

        if (u3vInstance->eventHandler != NULL)
        {
            /* Let the client know that the device is detached */
            u3vInstance->eventHandler((T_U3VHostHandle)(u3vInstance),
                                      U3V_HOST_EVENT_DEVICE_DETACHED,
                                      NULL, 
                                      u3vInstance->context);
        }

        /* Release the object */
        u3vInstance->eventHandler = NULL;
        u3vInstance->deviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
        u3vInstance->deviceClientHandle = USB_HOST_DEVICE_CLIENT_HANDLE_INVALID;
    }
}


static void _USB_HostU3V_DeviceTasks(USB_HOST_DEVICE_CLIENT_HANDLE deviceHandle)
{
    int32_t index;
    int32_t iterator;
    T_UsbHostU3VInstanceObj *u3vInstance;
    USB_HOST_RESULT result;
    USB_HOST_REQUEST_HANDLE requestHandle;

    index = _USB_HostU3V_DeviceHandleToInstance(deviceHandle);

   if (index >= 0)
    {
        u3vInstance = &gUSBHostU3VObj[index];

        if (u3vInstance->inUse)
        {
            /* Instance is valid */
            switch (u3vInstance->state)
            {
                case U3V_HOST_STATE_NOT_READY:
                    /* The instance is not ready. We dont do anything yet */
                    break;

                case U3V_HOST_STATE_SET_CONFIGURATION:
                    /* The instance should set the configuration. We clear the
                     * controlTransferDone flag. This will be set in the device
                     * event handler when the configuration set event is received. */
                    u3vInstance->hostRequestDone = false;
                    result = USB_HOST_DeviceConfigurationSet(u3vInstance->deviceClientHandle, &requestHandle,
                                                             0, (uintptr_t)(u3vInstance));

                    if (result == USB_HOST_RESULT_SUCCESS)
                    {
                        /* The result was successful. Change state to wating for configuration. */
                        u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET;
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_CONFIGURATION_SET:
                    /* Here we are waiting for the configuration to be set */
                    if (u3vInstance->hostRequestDone == true)
                    {
                        if (u3vInstance->hostRequestResult == USB_HOST_RESULT_SUCCESS)
                        {
                            /* The configuration has been set. Now wait for the host
                             * layer to send the communication and the data
                             * interface to the client driver */
                            u3vInstance->state = U3V_HOST_STATE_WAIT_FOR_INTERFACES;
                        }
                        else
                        {
                            /* If we could not set the configuration, then state
                             * instance state to error */
                            u3vInstance->state = U3V_HOST_STATE_ERROR;
                        }
                    }
                    break;

                case U3V_HOST_STATE_WAIT_FOR_INTERFACES:
                    /* Here we wait for both the interfaces to get ready */
                    if ((u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->controlIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->eventIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID) &&
                        (u3vInstance->streamIf.bulkInPipeHandle != USB_HOST_PIPE_HANDLE_INVALID))
                    {
                        /* Set the state to ready */
                        u3vInstance->state = U3V_HOST_STATE_READY;
                        /* We know that the client driver is now ready. We can
                         * let all the listeners know that the device has been
                         * attached */
                        for (iterator = 0; iterator < USB_HOST_U3V_ATTACH_LISTENERS_NUMBER; iterator++)
                        {
                            if (gUSBHostU3VAttachListener[iterator].inUse)
                            {
                                /* Call the attach listener event handler function. */
                                gUSBHostU3VAttachListener[iterator].eventHandler((T_U3VHostObject)(u3vInstance),
                                                                                 gUSBHostU3VAttachListener[iterator].context);
                            }
                        }
                    }
                    break;

                case U3V_HOST_STATE_READY:
                    /* The U3V client driver is ready to be opened */
                    break;

                case U3V_HOST_STATE_ERROR:
                    /* The instance is an error state. Do nothing */
                    break;
            }
        }
    }
}


static int32_t _USB_HostU3V_DeviceHandleToInstance(USB_HOST_DEVICE_CLIENT_HANDLE deviceClientHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceClientHandle == deviceClientHandle)
        {
            result = iterator;
            break;
        }
    }
    return result;
}


static int32_t _USB_HostU3V_DeviceObjHandleToInstance(USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator++)
    {
        if (gUSBHostU3VObj[iterator].deviceObjHandle == deviceObjHandle)
        {
            result = iterator;
            break;
        }
    }
    return result;
}


static int32_t _USB_HostU3V_InterfaceHandleToInstance(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    int32_t result = -1;
    int32_t iterator;

    for (iterator = 0; iterator < USB_HOST_U3V_INSTANCES_NUMBER; iterator++)
    {
        if ((gUSBHostU3VObj[iterator].controlIf.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].eventIf.ifHandle == interfaceHandle) ||
            (gUSBHostU3VObj[iterator].streamIf.ifHandle == interfaceHandle))
        {
            result = iterator;
            break;
        }
    }
    return result;
}


T_U3VHostResult _USB_HostU3V_HostToU3VResultsMap(USB_HOST_RESULT hostResult)
{
    T_U3VHostResult u3vResult;

    switch (hostResult)
    {
        case USB_HOST_RESULT_SUCCESS:
            u3vResult = U3V_HOST_RESULT_SUCCESS;
            break;

        case USB_HOST_RESULT_FAILURE:
        case USB_HOST_RESULT_PARAMETER_INVALID:
        case USB_HOST_RESULT_PIPE_HANDLE_INVALID:
            u3vResult = U3V_HOST_RESULT_FAILURE;
            break;

        case USB_HOST_RESULT_REQUEST_BUSY:
            u3vResult = U3V_HOST_RESULT_BUSY;
            break;

        case USB_HOST_RESULT_REQUEST_STALLED:
            u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
            break;

        case USB_HOST_RESULT_TRANSFER_ABORTED:
            u3vResult = U3V_HOST_RESULT_ABORTED;
            break;

        default:
            u3vResult = U3V_HOST_RESULT_FAILURE;
            break;
    }
    return u3vResult;
}
