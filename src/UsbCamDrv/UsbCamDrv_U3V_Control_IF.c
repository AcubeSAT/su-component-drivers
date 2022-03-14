//
// Created by mojo on 17/02/22.
//

#include <string.h>
#include "UsbCamDrv_U3V_Control_IF.h"
#include "UsbCamDrv_U3V_Control_IF_local.h"
#include "UsbCamDrv_Host_U3V.h"
#include "UsbCamDrv_Host_U3V_Local.h"
#include "UsbCamDrv_Config.h"
#include "UsbCamDrv_DeviceClassSpec_U3V.h"


/********************************************************
* Local function declarations
*********************************************************/

static inline void _FreeCtrlIfObjAllocSpace(T_U3VControlInterfaceObj *pCtrlIfObj)
{
    OSAL_Free(pCtrlIfObj->ackBuffer);
    OSAL_Free(pCtrlIfObj->cmdBuffer);
    OSAL_Free(pCtrlIfObj);
};


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VControlInterfaceObj *controlIfObjData; // DEBUG - monitor data


/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VControlInterfHandle *pU3vCtrlIf, T_U3VHostObject u3vInstObj)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlInterfaceObj *ctrlIfobj = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    uint64_t sbrmAddress;
    uint32_t bytesRead;
    uint32_t maxResponse;
    uint32_t cmdBfrSize;
    uint32_t ackBfrSize;

    u3vResult = (pU3vCtrlIf == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
    u3vResult = (u3vInstObj == 0)    ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    /* retrieve U3V device instance and check if in ready state*/
    u3vInstance = (T_U3VHostInstanceObj *)u3vInstObj;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    /* allocate needed space for Control Interface data */
    ctrlIfobj = OSAL_Malloc(sizeof(T_U3VControlInterfaceObj));

    if(OSAL_MUTEX_Create(&(ctrlIfobj->readWriteLock)) != OSAL_RESULT_TRUE)
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* store permanently the location of U3V device instance 
     * (as uintptr_t) in Control IF data for future use */
    ctrlIfobj->u3vInstObj = (T_U3VHostObject)u3vInstance;

    ctrlIfobj->u3vTimeout = U3V_REQ_TIMEOUT;
    ctrlIfobj->maxAckTransfSize = MIN((1 << 16) + sizeof(T_U3VCtrlIfAckHeader), (size_t)(0xFFFFFFFF));
    ctrlIfobj->maxCmdTransfSize = MIN((1 << 16) + sizeof(T_U3VCtrlIfCmdHeader), (size_t)(0xFFFFFFFF));
    
    /* requestId, maxRequestId are preincremented, with overflow will start again from 0 */
    ctrlIfobj->requestId = -1; 
    ctrlIfobj->maxRequestId = -1;

    /* allocate buffer space */
    ctrlIfobj->ackBuffer = OSAL_Malloc(ctrlIfobj->maxAckTransfSize);
    ctrlIfobj->cmdBuffer = OSAL_Malloc(ctrlIfobj->maxCmdTransfSize);

    u3vResult = (ctrlIfobj->ackBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;
    u3vResult = (ctrlIfobj->cmdBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        ctrlIfobj = NULL;
        return u3vResult;
    }

    /* get from ABRM -> max device response time */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlInterfHandle)ctrlIfobj,
                                     NULL,
                                     U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                     U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                     &bytesRead,
                                     &maxResponse);

    u3vResult = (bytesRead != U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;
    
    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        ctrlIfobj = NULL;
        return u3vResult;
    }

    ctrlIfobj->u3vTimeout = MAX(U3V_REQ_TIMEOUT, maxResponse);

    /* get from ABRM -> SBRM address */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlInterfHandle)ctrlIfobj,
                                     NULL,
                                     U3V_ABRM_SBRM_ADDRESS_OFS,
                                     U3V_REG_SBRM_ADDRESS_SIZE,
                                     &bytesRead,
                                     &sbrmAddress);

    u3vResult = (bytesRead != U3V_REG_SBRM_ADDRESS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        ctrlIfobj = NULL;
        return u3vResult;
    }

    /* get from SBRM -> max command buffer size */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlInterfHandle)ctrlIfobj,
                                     NULL,
                                     sbrmAddress + U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                     sizeof(uint32_t),
                                     &bytesRead,
                                     &cmdBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        ctrlIfobj = NULL;
        return u3vResult;
    }

    ctrlIfobj->maxCmdTransfSize = MIN(ctrlIfobj->maxCmdTransfSize, cmdBfrSize);

    /* get from SBRM -> max acknowledge buffer size */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlInterfHandle)ctrlIfobj,
                                     NULL,
                                     sbrmAddress + U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                     sizeof(uint32_t),
                                     &bytesRead,
                                     &ackBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        ctrlIfobj = NULL;
        return u3vResult;
    }

    ctrlIfobj->maxAckTransfSize = MIN(ctrlIfobj->maxAckTransfSize, ackBfrSize);
    
    /* copy address of allocated Control IF to input pU3vCtrlIf handle */
    *pU3vCtrlIf = (T_U3VControlInterfHandle)ctrlIfobj;

    /* keep data visible for monitor debug */
    controlIfObjData = ctrlIfobj; 

    return u3vResult;
}



T_U3VHostResult U3VHost_CtrlIf_ReadMemory(T_U3VControlInterfHandle ctrlIfObj,
                                     T_U3VHostTransferHandle *transferHandle,
                                     uint32_t memAddress,
                                     size_t transfSize,
                                     uint32_t *bytesRead,
                                     void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlInterfaceObj *ctrlIfInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerRead = 0u;
    uint32_t totalBytesRead = 0u;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + transfSize;

    /* check input argument errors */
    u3vResult = (bytesRead   == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer      == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize  == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfObj   == 0)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = (T_U3VControlInterfaceObj *)ctrlIfObj;
    u3vInstance = (T_U3VHostInstanceObj *)ctrlIfInst->u3vInstObj;
    maxBytesPerRead = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize)  ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead == 0u)                         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (!u3vInstance->inUse)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY)    ? U3V_HOST_RESULT_BUSY : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;
    *bytesRead = 0u;

    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }
    
    while (totalBytesRead < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesRead), maxBytesPerRead);
        T_U3VCtrlIfCommand *command = (T_U3VCtrlIfCommand *)(ctrlIfInst->cmdBuffer);
        T_U3VCtrlIfReadMemCmdPayload *payload = (T_U3VCtrlIfReadMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0u;

        // ctrlIfInst->readReqSts.length = 0u;
        // ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
        // ctrlIfInst->readReqSts.transferHandle = 0u;

        command->header.prefix = (uint32_t)(U3V_CONTROL_PREFIX);
        command->header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        command->header.cmd    = (uint16_t)(U3V_CTRL_READMEM_CMD);
        command->header.length = (uint16_t)sizeof(T_U3VCtrlIfReadMemCmdPayload);

        if ((ctrlIfInst->requestId + 1u) == ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
        payload->address = memAddress + totalBytesRead;
        payload->reserved = 0u;
        payload->byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIfHandle.bulkOutPipeHandle,
                                             tempTransferHandle,
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

        // while ((ctrlIfInst->writeReqSts.length != cmdBufferSize) ||
        //        (ctrlIfInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
        //        (ctrlIfInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        // {
        //     /* Wait for read request to complete with retry limit */
        //     writeRetryCnt++;
        //     if (writeRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
        //     {
        //         /* write failed */
        //         OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        //         u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
        //         return u3vResult;
        //     }
        // }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0u;

            // ctrlIfInst->readReqSts.length = 0u;
            // ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            // ctrlIfInst->readReqSts.transferHandle = 0u;

            /* Reset buffer. Ensure that we have enough room for a pending_ack_payload
             * (e.g. if we try to read only two bytes then we would not be able to 
             * receive a pending_ack_payload, which has a transfSize greater than 2 bytes). */
            ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + MAX((size_t)(bytesThisIteration), sizeof(T_U3VCtrlIfPendingAckPayload));
            memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

            /* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIfHandle.bulkInPipeHandle,
                                                 tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            // if ((actual < sizeof(T_U3VCtrlIfAckHeader)) ||
            //     (ack->header.prefix != U3V_CONTROL_PREFIX) ||
            //     (actual != (ack->header.length + sizeof(T_U3VCtrlIfAckHeader))))

            // while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
            //        (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
            //        (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
            while (*ctrlIfInst->ackBuffer == 0)
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if (readRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
                {
                    /* write failed */
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
            }

            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
            if (ack->header.cmd == U3V_CTRL_PENDING_ACK)
            {
                pendingAck = (T_U3VCtrlIfPendingAckPayload *)(ack->payload);
                ctrlIfInst->u3vTimeout = MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }

            /* Acknowledge received successfully */
            reqAcknowledged = true;

        } /* loop until reqAcknowledged == true */
        totalBytesRead += bytesThisIteration;
    } /* loop until totalBytesRead == transfSize */

    if (totalBytesRead != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* Extract the data */
	memcpy(buffer, ack->payload, totalBytesRead);
    *bytesRead = totalBytesRead;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


T_U3VHostResult U3VHost_CtrlIf_WriteMemory(T_U3VControlInterfHandle ctrlIfObj,
                                      T_U3VHostTransferHandle *transferHandle,
                                      uint32_t memAddress,
                                      size_t transfSize,
                                      uint32_t *bytesWritten,
                                      const void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlInterfaceObj *ctrlIfInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VHostTransferHandle *tempTransferHandle, localTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerWrite = 0u;
    uint32_t totalBytesWritten = 0u;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    T_U3V_CtrlWriteMemAckPayload *writeMemAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + MAX(sizeof(T_U3V_CtrlWriteMemAckPayload),
                                                             sizeof(T_U3VCtrlIfPendingAckPayload));

    /* check input argument errors */
    u3vResult = (bytesWritten == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer       == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize   == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfObj    == 0)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    
    ctrlIfInst = (T_U3VControlInterfaceObj *)ctrlIfObj;
    u3vInstance = (T_U3VHostInstanceObj *)ctrlIfInst->u3vInstObj;
    maxBytesPerWrite = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize)  ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite = 0u)                         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (!u3vInstance->inUse)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY)    ? U3V_HOST_RESULT_BUSY : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle == NULL) ? &localTransferHandle : transferHandle;
    *bytesWritten = 0u;

    /* The context for the transfer is the event that needs to be sent to the application. 
     * In this case the event to be sent to the application when the transfer completes is
     * U3V_HOST_EVENT_WRITE_COMPLETE */
 
    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesWritten), maxBytesPerWrite);

        T_U3VCtrlIfCommand*command = (T_U3VCtrlIfCommand *)(ctrlIfInst->cmdBuffer);
        T_U3VCtrlIfWriteMemCmdPayload *payload = (T_U3VCtrlIfWriteMemCmdPayload *)(command->payload);

        command->header.prefix = (uint32_t)(U3V_CONTROL_PREFIX);
		command->header.flags = (uint16_t)(U3V_CTRL_REQ_ACK);
		command->header.cmd = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        command->header.length = (uint16_t)(sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration);

        if (ctrlIfInst->requestId + 1u == ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
		payload->address = (uint64_t)(memAddress + totalBytesWritten);
        cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration;

        memcpy(payload->data, (uint8_t *)(buffer + totalBytesWritten), bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIfHandle.bulkOutPipeHandle,
                                             tempTransferHandle, 
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

		reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            /* reset buffer */
			memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

			/* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlIfHandle.bulkInPipeHandle,
                                                 tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            while (*ctrlIfInst->ackBuffer == 0);
            {
                // Wait for data to arrive in buffer
            } 
            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);

			/* Validate that we read enough bytes to process the header and that this seems like a valid GenCP response */

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlIfPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* Fix for broken Basler cameras where they can get in a state where there is an extra bogus response on the
			 * pipe that needs to be thrown away. We just submit another read in that case. */
            if (ack->header.ackId == (ctrlIfInst->requestId - 1u))
            {
                continue;
            }

            writeMemAck = (T_U3V_CtrlWriteMemAckPayload *)(ack->payload);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_WRITEMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) &&
                 (ack->header.length != sizeof(T_U3V_CtrlWriteMemAckPayload)) && (ack->header.length != 0)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlIfPendingAckPayload))) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length == sizeof(T_U3V_CtrlWriteMemAckPayload)) &&
                 (writeMemAck->bytesWritten != bytesThisIteration)))
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
			if (ack->header.cmd == U3V_CTRL_PENDING_ACK) 
            {
                pendingAck = (T_U3VCtrlIfPendingAckPayload *)(ack->payload);
                ctrlIfInst->u3vTimeout = MAX(ctrlIfInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }
            /* Acknowledge received successfully */
			reqAcknowledged = true;
        } /* loop until reqAcknowledged == true */
        totalBytesWritten += bytesThisIteration;
    } /* loop until totalBytesWritten == transfSize */

    if (totalBytesWritten != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    *bytesWritten = totalBytesWritten;

    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));

    return u3vResult;
}


void U3VHost_CtrlIf_InterfaceDestroy(T_U3VControlInterfHandle *pU3vCtrlIf)
{
    T_U3VControlInterfaceObj *ctrlIfobj;

    if ((pU3vCtrlIf != NULL) && (*pU3vCtrlIf != 0))
    {
        ctrlIfobj = (T_U3VControlInterfaceObj *)(*pU3vCtrlIf);

        // wait_for_completion(&u3vInst->controlIfHandle.ioctl_complete);
        // if (!u3vInst->stalling_disabled &&
        //     u3vInst->u3v_info->legacy_ctrl_ep_stall_enabled)
        // 	reset_pipe(u3vInst, &u3vInst->control_info);

        _FreeCtrlIfObjAllocSpace(ctrlIfobj);
        *pU3vCtrlIf = 0;
    }
    else
    {
        /* invalid handle */
    }
}
