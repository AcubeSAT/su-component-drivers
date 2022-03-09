//
// Created by mojo on 17/02/22.
//

#include <string.h>
#include "UsbCamDrv_U3V_Control_IF.h"
#include "UsbCamDrv_Host_U3V_Local.h"



/********************************************************
* Local function declarations
*********************************************************/

static inline void _FreeCtrlIfObjAllocSpace(T_U3V_ControlIntfStr *pCtrlIfObj)
{
    OSAL_Free(pCtrlIfObj->ackBuffer);
    OSAL_Free(pCtrlIfObj->cmdBuffer);
    OSAL_Free(pCtrlIfObj);
};


/********************************************************
* Constant & Variable declarations
*********************************************************/



/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult U3VCtrlIf_IntfCreate(T_U3VHostObject u3vInstObj)
{
    T_UsbHostU3VInstanceObj *u3vInstance;

    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3V_ControlIntfStr *ctrlIfobj = NULL;
    uint64_t sbrmAddress;
    uint32_t bytesRead;
    uint32_t maxResponse;
    uint32_t cmdBfrSize;
    uint32_t ackBfrSize;

    if (u3vInstObj == 0)
    {
        u3vResult = U3V_HOST_RESULT_DEVICE_UNKNOWN;
        return u3vResult;
    }

    u3vInstance = (T_UsbHostU3VInstanceObj *)u3vInstObj;

    /* check for argument errors */
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance->controlIfData != NULL)         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    if(OSAL_MUTEX_Create(&(ctrlIfobj->readWriteLock)) != OSAL_RESULT_TRUE)
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    ctrlIfobj = OSAL_Malloc(sizeof(T_U3V_ControlIntfStr));
    
    ctrlIfobj->u3vTimeout = U3V_REQ_TIMEOUT;

    ctrlIfobj->maxAckTransfSize = MIN((1 << 16) + sizeof(T_U3V_CtrlAckHeader), (size_t)(0xFFFFFFFF));
    ctrlIfobj->maxCmdTransfSize = MIN((1 << 16) + sizeof(T_U3V_CtrlCmdHeader), (size_t)(0xFFFFFFFF));
    
    /* requestId, maxRequestId are preincremented, with underflow will start from 0 */
    ctrlIfobj->requestId = -1; 
    ctrlIfobj->maxRequestId = -1;

    ctrlIfobj->ackBuffer = OSAL_Malloc(ctrlIfobj->maxAckTransfSize);
    ctrlIfobj->cmdBuffer = OSAL_Malloc(ctrlIfobj->maxCmdTransfSize);

    u3vResult = (ctrlIfobj->ackBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;
    u3vResult = (ctrlIfobj->cmdBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        return u3vResult;
    }

    u3vInstance->controlIfData = ctrlIfobj;

    /* get ABRM max device response time */
    u3vResult = U3VCtrlIf_ReadMemory(u3vInstance,
                                     NULL,
                                     U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                     U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                     &bytesRead,
                                     &maxResponse);

    u3vResult = (bytesRead != U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? USB_HOST_RESULT_FAILURE : u3vResult;
    
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        u3vInstance->controlIfData = NULL;
        return u3vResult;
    }

    ctrlIfobj->u3vTimeout = MAX(U3V_REQ_TIMEOUT, maxResponse);

    /* get ABRM -> SBRM address */
    u3vResult = U3VCtrlIf_ReadMemory(u3vInstance,
                                     NULL,
                                     U3V_ABRM_SBRM_ADDRESS_OFS,
                                     U3V_REG_SBRM_ADDRESS_SIZE,
                                     &bytesRead,
                                     &sbrmAddress);

    u3vResult = (bytesRead != U3V_REG_SBRM_ADDRESS_SIZE) ? USB_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        u3vInstance->controlIfData = NULL;
        return u3vResult;
    }

    /* get SBRM max command buffer size */
    u3vResult = U3VCtrlIf_ReadMemory(u3vInstance,
                                     NULL,
                                     sbrmAddress + U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                     sizeof(uint32_t),
                                     &bytesRead,
                                     &cmdBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? USB_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        u3vInstance->controlIfData = NULL;
        return u3vResult;
    }

    ctrlIfobj->maxCmdTransfSize = MIN(ctrlIfobj->maxCmdTransfSize, cmdBfrSize);

    /* get SBRM max acknowledge buffer size */
    u3vResult = U3VCtrlIf_ReadMemory(u3vInstance,
                                     NULL,
                                     sbrmAddress + U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                     sizeof(uint32_t),
                                     &bytesRead,
                                     &ackBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? -1 : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        u3vInstance->controlIfData = NULL;
        return u3vResult;
    }

    ctrlIfobj->maxAckTransfSize = MIN(ctrlIfobj->maxAckTransfSize, ackBfrSize);
    
    // u3vInstance->controlIfData = ctrlIfobj;

    return u3vResult;
}



T_U3VHostResult U3VCtrlIf_ReadMemory(T_UsbHostU3VInstanceObj *u3vInstance,
                                     T_U3VHostTransferHandle *transferHandle,
                                     uint32_t memAddress,
                                     size_t transfSize,
                                     uint32_t *bytesRead,
                                     void *buffer)
{
    T_U3V_ControlIntfStr *ctrlIfobj = NULL;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerRead = 0u;
    uint32_t totalBytesRead = 0u;
    int32_t actual = 0;
    bool reqAcknowledged;
    T_U3V_CtrlAcknowledge *ack = NULL;
    T_U3V_CtrlPendingAckPayload *pendingAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3V_CtrlCmdHeader) + sizeof(T_U3V_CtrlReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3V_CtrlAckHeader) + transfSize;

    /* check input argument errors */
    u3vResult = (u3vInstance == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (bytesRead   == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer      == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize  == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfobj = u3vInstance->controlIfData;
    maxBytesPerRead = ctrlIfobj->maxAckTransfSize - sizeof(T_U3V_CtrlAckHeader);

    u3vResult = (cmdBufferSize > ctrlIfobj->maxCmdTransfSize) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead = 0u) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (!u3vInstance->inUse) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_BUSY : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;
    *bytesRead = 0u;

    if(OSAL_MUTEX_Lock(&(ctrlIfobj->readWriteLock), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }
    
    while (totalBytesRead < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesRead), maxBytesPerRead);
        T_U3V_CtrlCommand *command = (T_U3V_CtrlCommand *)(ctrlIfobj->cmdBuffer);
        T_U3V_CtrlReadMemCmdPayload *payload = (T_U3V_CtrlReadMemCmdPayload *)(command->payload);

        command->header.prefix = (uint32_t)(U3V_CONTROL_PREFIX);
        command->header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        command->header.cmd    = (uint16_t)(U3V_CTRL_READMEM_CMD);
        command->header.length = (uint16_t)sizeof(T_U3V_CtrlReadMemCmdPayload);

        if ((ctrlIfobj->requestId + 1u) == ctrlIfobj->maxRequestId)
        {
            ctrlIfobj->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfobj->requestId));
        payload->address = memAddress + totalBytesRead;
        payload->reserved = 0u;
        payload->byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkOutPipeHandle,
                                             tempTransferHandle,
                                             ctrlIfobj->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            return u3vResult;
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            /* Reset. Ensure that we have enough room for a pending_ack_payload
             * (e.g. if we try to read only two bytes then we would not be able to 
             * receive a pending_ack_payload, which has a transfSize greater than 2 bytes). */
            actual = 0;
            ackBufferSize = sizeof(T_U3V_CtrlAckHeader) + MAX((size_t)(bytesThisIteration), sizeof(T_U3V_CtrlPendingAckPayload));
            memset(ctrlIfobj->ackBuffer, 0, ackBufferSize);

            /* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkInPipeHandle,
                                                 tempTransferHandle,
                                                 ctrlIfobj->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));
            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* Validate that we read enough bytes to process the header and that this seems like a valid GenCP response */

            // if ((actual < sizeof(T_U3V_CtrlAckHeader)) ||
            //     (ack->header.prefix != U3V_CONTROL_PREFIX) ||
            //     (actual != (ack->header.length + sizeof(T_U3V_CtrlAckHeader))))

            ack = (T_U3V_CtrlAcknowledge *)(ctrlIfobj->ackBuffer);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfobj->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3V_CtrlPendingAckPayload))))
            {
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
            if (ack->header.cmd == U3V_CTRL_PENDING_ACK)
            {
                pendingAck = (T_U3V_CtrlPendingAckPayload *)(ack->payload);
                ctrlIfobj->u3vTimeout = MAX(ctrlIfobj->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }

            /* Acknowledge received successfully */
            reqAcknowledged = true;

        } /* loop until reqAcknowledged == true */
        totalBytesRead += bytesThisIteration;
    } /* loop until totalBytesRead == transfSize */

    if (totalBytesRead != transfSize)
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* Extract the data */
	memcpy(buffer, ack->payload, totalBytesRead);
    *bytesRead = totalBytesRead;

    OSAL_MUTEX_Unlock(&(ctrlIfobj->readWriteLock));

    return u3vResult;
}


T_U3VHostResult U3VCtrlIf_WriteMemory(T_UsbHostU3VInstanceObj *u3vInstance,
                                      T_U3VHostTransferHandle *transferHandle,
                                      uint32_t memAddress,
                                      size_t transfSize,
                                      uint32_t *bytesWritten,
                                      const void *buffer)
{
    T_U3V_ControlIntfStr *ctrlIfobj = NULL;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerWrite = 0u;
    uint32_t totalBytesWritten = 0u;
    int32_t actual = 0;
    bool reqAcknowledged;
    T_U3V_CtrlAcknowledge *ack = NULL;
    T_U3V_CtrlPendingAckPayload *pendingAck = NULL;
    T_U3V_CtrlWriteMemAckPayload *writeMemAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3V_CtrlCmdHeader) + sizeof(T_U3V_CtrlReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3V_CtrlAckHeader) + MAX(sizeof(T_U3V_CtrlWriteMemAckPayload), sizeof(T_U3V_CtrlPendingAckPayload));

    /* check input argument errors */
    u3vResult = (u3vInstance   == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (bytesWritten  == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer        == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize    == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfobj = u3vInstance->controlIfData;
    maxBytesPerWrite = ctrlIfobj->maxAckTransfSize - sizeof(T_U3V_CtrlAckHeader);

    u3vResult = (cmdBufferSize > ctrlIfobj->maxCmdTransfSize) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite = 0u) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (!u3vInstance->inUse) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_BUSY : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;
    *bytesWritten = 0u;

    /* The context for the transfer is the event that needs to be sent to the application. 
     * In this case the event to be sent to the application when the transfer completes is
     * U3V_HOST_EVENT_WRITE_COMPLETE */
 
    if(OSAL_MUTEX_Lock(&(ctrlIfobj->readWriteLock), OSAL_WAIT_FOREVER) == OSAL_RESULT_FALSE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesWritten), maxBytesPerWrite);

        T_U3V_CtrlCommand*command = (T_U3V_CtrlCommand *)(ctrlIfobj->cmdBuffer);
        T_U3V_CtrlWriteMemCmdPayload *payload = (T_U3V_CtrlWriteMemCmdPayload *)(command->payload);

        command->header.prefix = (uint32_t)(U3V_CONTROL_PREFIX);
		command->header.flags = (uint16_t)(U3V_CTRL_REQ_ACK);
		command->header.cmd = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        command->header.length = (uint16_t)(sizeof(T_U3V_CtrlWriteMemCmdPayload) + bytesThisIteration);

        if (ctrlIfobj->requestId + 1u == ctrlIfobj->maxRequestId)
        {
            ctrlIfobj->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfobj->requestId));
		payload->address = (uint64_t)(memAddress + totalBytesWritten);
        cmdBufferSize = sizeof(T_U3V_CtrlCmdHeader) + sizeof(T_U3V_CtrlWriteMemCmdPayload) + bytesThisIteration;

        memcpy(payload->data, (uint8_t *)(buffer + totalBytesWritten), bytesThisIteration);

        // ret = usb_bulk_msg(u3v->udev,
        //                    usb_sndbulkpipe(u3v->udev,
        //                                    usb_endpoint_num(u3v->control_info.bulk_out)),
        //                    ctrl->cmd_buffer, cmd_buffer_size,
        //                    &actual, ctrl->u3v_timeout);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkOutPipeHandle,
                                             tempTransferHandle, 
                                             ctrlIfobj->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            return u3vResult;
        }

		reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            /* reset */
			actual = 0;
			memset(ctrlIfobj->ackBuffer, 0, ackBufferSize);

			/* read the ack */
            // ret = usb_bulk_msg(u3v->udev,
            //                    usb_rcvbulkpipe(u3v->udev,
            //                                    usb_endpoint_num(u3v->control_info.bulk_in)),
            //                    ctrl->ack_buffer, ack_buffer_size, &actual,
            //                    ctrl->u3v_timeout);

			/* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIf.bulkInPipeHandle,
                                                 tempTransferHandle,
                                                 ctrlIfobj->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            ack = (T_U3V_CtrlAcknowledge *)(ctrlIfobj->ackBuffer);

			/* Validate that we read enough bytes to process the header and that this seems like a valid GenCP response */

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfobj->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3V_CtrlPendingAckPayload))))
            {
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* Fix for broken Basler cameras where they can get in a state where there is an extra bogus response on the
			 * pipe that needs to be thrown away. We just submit another read in that case. */
            if (ack->header.ackId == (ctrlIfobj->requestId - 1u))
            {
                continue;
            }

            writeMemAck = (T_U3V_CtrlWriteMemAckPayload *)(ack->payload);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_WRITEMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfobj->requestId) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length != sizeof(T_U3V_CtrlWriteMemAckPayload)) && (ack->header.length != 0)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3V_CtrlPendingAckPayload))) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length == sizeof(T_U3V_CtrlWriteMemAckPayload)) && (writeMemAck->bytesWritten != bytesThisIteration)))
            {
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
			if (ack->header.cmd == U3V_CTRL_PENDING_ACK) 
            {
                pendingAck = (T_U3V_CtrlPendingAckPayload *)(ack->payload);
                ctrlIfobj->u3vTimeout = MAX(ctrlIfobj->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }
            /* Acknowledge received successfully */
			reqAcknowledged = true;
        } /* loop until reqAcknowledged == true */
        totalBytesWritten += bytesThisIteration;
    } /* loop until totalBytesWritten == transfSize */

    if (totalBytesWritten != transfSize)
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    *bytesWritten = totalBytesWritten;

    OSAL_MUTEX_Unlock(&(ctrlIfobj->readWriteLock));

    return u3vResult;
}


void U3VCtrlIf_IntfDestroy(T_U3VHostObject u3vInstObj)
{
    T_UsbHostU3VInstanceObj *u3vInstance = NULL;
    T_U3V_ControlIntfStr *ctrlIfobj = NULL;

    if(u3vInstObj == 0)
    {
        return;
    }

    u3vInstance = (T_UsbHostU3VInstanceObj *)u3vInstObj;
    ctrlIfobj = u3vInstance->controlIfData;

    if (ctrlIfobj == NULL)
    {
        return;
    }

	// wait_for_completion(&u3vInst->controlIf.ioctl_complete);
	// if (!u3vInst->stalling_disabled &&
	//     u3vInst->u3v_info->legacy_ctrl_ep_stall_enabled)
	// 	reset_pipe(u3vInst, &u3vInst->control_info);

    _FreeCtrlIfObjAllocSpace(ctrlIfobj);
	u3vInstance->controlIfData = NULL;
}

