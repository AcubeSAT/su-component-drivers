//
// Created by fomarko on 17/02/22.
//


#include "U3VCam_Host.h"
#include "U3VCam_Host_Local.h"



/********************************************************
* Local function declarations
*********************************************************/
static void _U3VHost_CtrlIf_TransferReqCompleteCbk(T_U3VHostHandle u3vHostHandle, T_U3VHostEvent transfEvent, void *transfData);
static void _CtrlIntfClearObjData(T_U3VControlIfObj *pCtrlIfObj);

/********************************************************
* Constant & Variable declarations
*********************************************************/


/********************************************************
* Function definitions
*********************************************************/
T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VHostHandle u3vObjHandle)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    uint64_t sbrmAddress;
    uint32_t bytesRead;
    uint32_t maxResponse;
    uint32_t cmdBfrSize;
    uint32_t ackBfrSize;

    u3vResult = (u3vObjHandle == 0UL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    u3vInstance = (T_U3VHostInstanceObj *)u3vObjHandle;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = &u3vInstance->controlIfObj;

    if(OSAL_MUTEX_Create(&(ctrlIfInst->readWriteLock)) != OSAL_RESULT_TRUE)
    {
        _CtrlIntfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->transfReqCompleteCbk = _U3VHost_CtrlIf_TransferReqCompleteCbk;

    ctrlIfInst->u3vTimeout = U3V_REQ_TIMEOUT;
    ctrlIfInst->maxAckTransfSize = (256UL + sizeof(T_U3VCtrlIfAckHeader));
    ctrlIfInst->maxCmdTransfSize = (256UL + sizeof(T_U3VCtrlIfCmdHeader));

    /* requestId, maxRequestId are preincremented, with overflow will start again from 0 */
    ctrlIfInst->requestId = -1;
    ctrlIfInst->maxRequestId = -1;
    ctrlIfInst->ctrlIntfHandle = &u3vInstance->controlIfHandle;

    /* get from ABRM -> max device response time */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInst,
                                          NULL,
                                          U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                          U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                          &bytesRead,
                                          &maxResponse);

    u3vResult = (bytesRead != U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _CtrlIntfClearObjData(ctrlIfInst);
        return u3vResult;
    }
    
    ctrlIfInst->u3vTimeout = MAX(U3V_REQ_TIMEOUT, maxResponse);

    /* get from ABRM -> SBRM address */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInst,
                                          NULL,
                                          U3V_ABRM_SBRM_ADDRESS_OFS,
                                          U3V_REG_SBRM_ADDRESS_SIZE,
                                          &bytesRead,
                                          &sbrmAddress);

    u3vResult = (bytesRead != U3V_REG_SBRM_ADDRESS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _CtrlIntfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    /* get from SBRM -> max command buffer size */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInst,
                                          NULL,
                                          sbrmAddress + U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                          sizeof(uint32_t),
                                          &bytesRead,
                                          &cmdBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _CtrlIntfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->maxCmdTransfSize = MIN(ctrlIfInst->maxCmdTransfSize, cmdBfrSize);

    /* get from SBRM -> max acknowledge buffer size */
    u3vResult = U3VHost_CtrlIf_ReadMemory((T_U3VControlIfObjHandle)ctrlIfInst,
                                          NULL,
                                          sbrmAddress + U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                          sizeof(uint32_t),
                                          &bytesRead,
                                          &ackBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _CtrlIntfClearObjData(ctrlIfInst);
        return u3vResult;
    }

    ctrlIfInst->maxAckTransfSize = MIN(ctrlIfInst->maxAckTransfSize, ackBfrSize);
    
    return u3vResult;
}


void U3VHost_CtrlIf_InterfaceDestroy(T_U3VHostHandle u3vObjHandle)
{
    T_U3VControlIfObj *u3vCtrlIntfObj = NULL;
    T_U3VHostInstanceObj *u3vDev = NULL;

    if (u3vObjHandle != 0UL)
    {
        u3vDev = (T_U3VHostInstanceObj *)u3vObjHandle;
        u3vCtrlIntfObj = (T_U3VControlIfObj *)(&u3vDev->controlIfObj);
        _CtrlIntfClearObjData(u3vCtrlIntfObj);
    }
    else
    {
        /* invalid handle */
    }
}


T_U3VHostResult U3VHost_CtrlIf_ReadMemory(T_U3VControlIfObjHandle ctrlIfObj,
                                          T_U3VHostTransferHandle *transferHandle,
                                          uint64_t memAddress,
                                          size_t transfSize,
                                          uint32_t *bytesRead,
                                          void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInterfHandle *ctrlIfHandle = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerRead = 0U;
    uint32_t totalBytesRead = 0U;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + transfSize;

    /* check input argument errors */
    u3vResult = (bytesRead   == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer      == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize  == 0U)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfObj   == 0U)   ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlIfInst = (T_U3VControlIfObj *)ctrlIfObj;
    maxBytesPerRead = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead == 0U)                        ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesRead = 0U;

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
        uint32_t writeRetryCnt = 0U;

        ctrlIfInst->writeReqSts.length = 0U;
        ctrlIfInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlIfInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
        command->header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        command->header.cmd    = (uint16_t)(U3V_CTRL_READMEM_CMD);
        command->header.length = (uint16_t)sizeof(T_U3VCtrlIfReadMemCmdPayload);

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
        payload->address = memAddress + (uint64_t)totalBytesRead;
        payload->reserved = 0U;
        payload->byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlIfInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlIfInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlIfInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if (writeRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
            {
                /* write failed */
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0U;

            ctrlIfInst->readReqSts.length = 0U;
            ctrlIfInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlIfInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + MAX((size_t)(bytesThisIteration), sizeof(T_U3VCtrlIfPendingAckPayload));
            memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

            /* read the ack */
            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if (readRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
                {
                    /* read failed */
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
            }

            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
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


T_U3VHostResult U3VHost_CtrlIf_WriteMemory(T_U3VControlIfObjHandle ctrlIfObj,
                                           T_U3VHostTransferHandle *transferHandle,
                                           uint64_t memAddress,
                                           size_t transfSize,
                                           uint32_t *bytesWritten,
                                           const void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlIfObj *ctrlIfInst = NULL;
    T_U3VHostInterfHandle *ctrlIfHandle = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerWrite = 0U;
    uint32_t totalBytesWritten = 0U;
    bool reqAcknowledged;
    T_U3VCtrlIfAcknowledge *ack = NULL;
    T_U3VCtrlIfPendingAckPayload *pendingAck = NULL;
    T_U3V_CtrlIfWriteMemAckPayload *writeMemAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlIfAckHeader) + MAX(sizeof(T_U3V_CtrlIfWriteMemAckPayload),
                                                              sizeof(T_U3VCtrlIfPendingAckPayload));

    /* check input argument errors */
    u3vResult = (bytesWritten == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer       == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize   == 0U)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfObj    == 0U)   ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    
    ctrlIfInst = (T_U3VControlIfObj *)ctrlIfObj;
    maxBytesPerWrite = ctrlIfInst->maxAckTransfSize - sizeof(T_U3VCtrlIfAckHeader);
    ctrlIfHandle = ctrlIfInst->ctrlIntfHandle;

    u3vResult = (cmdBufferSize > ctrlIfInst->maxCmdTransfSize) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite == 0U)                       ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlIfHandle->bulkInPipeHandle == USB_HOST_PIPE_HANDLE_INVALID)  ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;
    u3vResult = (ctrlIfHandle->bulkOutPipeHandle == USB_HOST_PIPE_HANDLE_INVALID) ? U3V_HOST_RESULT_DEVICE_UNKNOWN : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesWritten = 0U;
 
    if(OSAL_MUTEX_Lock(&(ctrlIfInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesWritten), maxBytesPerWrite);
        T_U3VCtrlIfCommand *command = (T_U3VCtrlIfCommand *)(ctrlIfInst->cmdBuffer);
        T_U3VCtrlIfWriteMemCmdPayload *payload = (T_U3VCtrlIfWriteMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0U;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
		command->header.flags = (uint16_t)(U3V_CTRL_REQ_ACK);
		command->header.cmd = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        command->header.length = (uint16_t)(sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration);

        if ((ctrlIfInst->requestId + 1U) >= ctrlIfInst->maxRequestId)
        {
            ctrlIfInst->requestId = 0U;
        }

        command->header.requestId = (uint16_t)(++(ctrlIfInst->requestId));
		payload->address = memAddress + (uint64_t)totalBytesWritten;
        cmdBufferSize = sizeof(T_U3VCtrlIfCmdHeader) + sizeof(T_U3VCtrlIfWriteMemCmdPayload) + bytesThisIteration;

        memcpy(payload->data, (uint8_t *)(buffer + totalBytesWritten), bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlIfInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlIfInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlIfInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlIfInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if (writeRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
            {
                /* write failed */
                OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0U;

            /* reset buffer */
			memset(ctrlIfInst->ackBuffer, 0, ackBufferSize);

			/* read the ack */
            hostResult = USB_HOST_DeviceTransfer(ctrlIfHandle->bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlIfInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _U3VHost_HostToU3VResultsMap(hostResult);

            ack = (T_U3VCtrlIfAcknowledge *)(ctrlIfInst->ackBuffer);
            writeMemAck = (T_U3V_CtrlIfWriteMemAckPayload *)(ack->payload);

            while ((ctrlIfInst->readReqSts.length != ackBufferSize) ||
                   (ctrlIfInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlIfInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack->header.prefix != U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if (readRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
                {
                    /* read failed */
                    OSAL_MUTEX_Unlock(&(ctrlIfInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
            }
            
            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_WRITEMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlIfInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length != sizeof(T_U3V_CtrlIfWriteMemAckPayload)) && (ack->header.length != 0U)) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length == sizeof(T_U3V_CtrlIfWriteMemAckPayload)) && (writeMemAck->bytesWritten != bytesThisIteration)) ||
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


static void _U3VHost_CtrlIf_TransferReqCompleteCbk(T_U3VHostHandle u3vHostHandle, T_U3VHostEvent transfEvent, void *transfData)
{
    T_U3VHostEventWriteCompleteData *writeCompleteEventData;
    T_U3VHostEventReadCompleteData  *readCompleteEventData;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VControlIfObj *ctrlIfInstance = NULL;
    u3vInstance = (T_U3VHostInstanceObj *)u3vHostHandle;

    if (u3vInstance != NULL)
    {
        ctrlIfInstance = &u3vInstance->controlIfObj;
        switch (transfEvent)
        {
            case U3V_HOST_EVENT_READ_COMPLETE:
                readCompleteEventData = (T_U3VHostEventReadCompleteData *)transfData;
                ctrlIfInstance->readReqSts = *readCompleteEventData;
                break;

            case U3V_HOST_EVENT_WRITE_COMPLETE:
                writeCompleteEventData = (T_U3VHostEventWriteCompleteData *)transfData;
                ctrlIfInstance->writeReqSts = *writeCompleteEventData;
                break;

            default:
                break;
        }
    }
}


static void _CtrlIntfClearObjData(T_U3VControlIfObj *pCtrlIfObj)
{
    memset(pCtrlIfObj, 0x0U, sizeof(T_U3VControlIfObj));
}

