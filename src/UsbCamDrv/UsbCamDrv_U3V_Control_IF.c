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
// #include "FreeRTOS.h"
// #include "task.h"


/********************************************************
* Local function declarations
*********************************************************/

static inline void _FreeCtrlChObjAllocSpace(T_U3VControlChannelObj *pCtrlChObj)
{
    OSAL_Free(pCtrlChObj->ackBuffer);
    OSAL_Free(pCtrlChObj->cmdBuffer);
    OSAL_Free(pCtrlChObj);
};


/********************************************************
* Constant & Variable declarations
*********************************************************/

T_U3VControlChannelObj *controlIfObjData; // DEBUG - monitor data


/********************************************************
* Function definitions
*********************************************************/

T_U3VHostResult U3VHost_CtrlCh_InterfaceCreate(T_U3VControlChannelHandle *pU3vCtrlCh, T_U3VHostObject u3vInstObj)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlChannelObj *ctrlChobj = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    uint64_t sbrmAddress;
    uint32_t bytesRead;
    uint32_t maxResponse;
    uint32_t cmdBfrSize;
    uint32_t ackBfrSize;

    u3vResult = (pU3vCtrlCh == NULL) ? U3V_HOST_RESULT_HANDLE_INVALID : u3vResult;
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
    ctrlChobj = OSAL_Malloc(sizeof(T_U3VControlChannelObj));

    if(OSAL_MUTEX_Create(&(ctrlChobj->readWriteLock)) != OSAL_RESULT_TRUE)
    {
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    ctrlChobj->u3vTimeout = U3V_REQ_TIMEOUT;
    ctrlChobj->maxAckTransfSize = MIN((1 << 16) + sizeof(T_U3VCtrlChAckHeader), (size_t)(0xFFFFFFFF));
    ctrlChobj->maxCmdTransfSize = MIN((1 << 16) + sizeof(T_U3VCtrlChCmdHeader), (size_t)(0xFFFFFFFF));
    
    /* requestId, maxRequestId are preincremented, with overflow will start again from 0 */
    ctrlChobj->requestId = -1; 
    ctrlChobj->maxRequestId = -1;
    ctrlChobj->transReqCompleteCbk = _U3VHost_CtrlCh_TransferReqCompleteCbk;

    /* allocate buffer space */
    ctrlChobj->ackBuffer = OSAL_Malloc(ctrlChobj->maxAckTransfSize);
    ctrlChobj->cmdBuffer = OSAL_Malloc(ctrlChobj->maxCmdTransfSize);

    u3vResult = (ctrlChobj->ackBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;
    u3vResult = (ctrlChobj->cmdBuffer == NULL) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlChObjAllocSpace(ctrlChobj);
        ctrlChobj = NULL;
        return u3vResult;
    }

    /* copy Ctrl If pointer to U3V instance */
    u3vInstance->controlChHandle.chanObj = ctrlChobj;
    /* copy U3V obj handle of U3V inst to Ctrl IF */
    ctrlChobj->u3vInstObj = u3vInstObj;

    /* get from ABRM -> max device response time */
    u3vResult = U3VHost_CtrlCh_ReadMemory((T_U3VControlChannelHandle)ctrlChobj,
                                          NULL,
                                          U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS,
                                          U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE,
                                          &bytesRead,
                                          &maxResponse);

    u3vResult = (bytesRead != U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlChObjAllocSpace(ctrlChobj);
        ctrlChobj = NULL;
        return u3vResult;
    }

    ctrlChobj->u3vTimeout = MAX(U3V_REQ_TIMEOUT, maxResponse);

    /* get from ABRM -> SBRM address */
    u3vResult = U3VHost_CtrlCh_ReadMemory((T_U3VControlChannelHandle)ctrlChobj,
                                          NULL,
                                          U3V_ABRM_SBRM_ADDRESS_OFS,
                                          U3V_REG_SBRM_ADDRESS_SIZE,
                                          &bytesRead,
                                          &sbrmAddress);

    u3vResult = (bytesRead != U3V_REG_SBRM_ADDRESS_SIZE) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlChObjAllocSpace(ctrlChobj);
        ctrlChobj = NULL;
        return u3vResult;
    }

    /* get from SBRM -> max command buffer size */
    u3vResult = U3VHost_CtrlCh_ReadMemory((T_U3VControlChannelHandle)ctrlChobj,
                                          NULL,
                                          sbrmAddress + U3V_SBRM_MAX_CMD_TRANSFER_OFS,
                                          sizeof(uint32_t),
                                          &bytesRead,
                                          &cmdBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlChObjAllocSpace(ctrlChobj);
        ctrlChobj = NULL;
        return u3vResult;
    }

    ctrlChobj->maxCmdTransfSize = MIN(ctrlChobj->maxCmdTransfSize, cmdBfrSize);

    /* get from SBRM -> max acknowledge buffer size */
    u3vResult = U3VHost_CtrlCh_ReadMemory((T_U3VControlChannelHandle)ctrlChobj,
                                          NULL,
                                          sbrmAddress + U3V_SBRM_MAX_ACK_TRANSFER_OFS,
                                          sizeof(uint32_t),
                                          &bytesRead,
                                          &ackBfrSize);

    u3vResult = (bytesRead != sizeof(uint32_t)) ? U3V_HOST_RESULT_FAILURE : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        _FreeCtrlChObjAllocSpace(ctrlChobj);
        ctrlChobj = NULL;
        return u3vResult;
    }

    ctrlChobj->maxAckTransfSize = MIN(ctrlChobj->maxAckTransfSize, ackBfrSize);
    
    /* copy address of allocated Control IF to input pU3vCtrlCh handle */
    *pU3vCtrlCh = (T_U3VControlChannelHandle)ctrlChobj;

    /* keep data visible for monitor debug */
    controlIfObjData = ctrlChobj; // DEBUG
    return u3vResult;
}


T_U3VHostResult U3VHost_CtrlCh_ReadMemory(T_U3VControlChannelHandle ctrlChObj,
                                          T_U3VHostTransferHandle *transferHandle,
                                          uint64_t memAddress,
                                          size_t transfSize,
                                          uint32_t *bytesRead,
                                          void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlChannelObj *ctrlChInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerRead = 0u;
    uint32_t totalBytesRead = 0u;
    bool reqAcknowledged;
    T_U3VCtrlChAcknowledge *ack = NULL;
    T_U3VCtrlChPendingAckPayload *pendingAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlChCmdHeader) + sizeof(T_U3VCtrlChReadMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlChAckHeader) + transfSize;

    /* check input argument errors */
    u3vResult = (bytesRead   == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer      == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize  == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlChObj   == 0)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    ctrlChInst = (T_U3VControlChannelObj *)ctrlChObj;
    u3vInstance = (T_U3VHostInstanceObj *)ctrlChInst->u3vInstObj;
    maxBytesPerRead = ctrlChInst->maxAckTransfSize - sizeof(T_U3VCtrlChAckHeader);

    u3vResult = (cmdBufferSize > ctrlChInst->maxCmdTransfSize)  ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerRead == 0u)                         ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (!u3vInstance->inUse)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY)    ? U3V_HOST_RESULT_BUSY              : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesRead = 0u;

    if(OSAL_MUTEX_Lock(&(ctrlChInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }
    
    while (totalBytesRead < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesRead), maxBytesPerRead);
        T_U3VCtrlChCommand *command = (T_U3VCtrlChCommand *)(ctrlChInst->cmdBuffer);
        T_U3VCtrlChReadMemCmdPayload *payload = (T_U3VCtrlChReadMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0u;

        ctrlChInst->writeReqSts.length = 0u;
        ctrlChInst->writeReqSts.result = U3V_HOST_RESULT_FAILURE;
        ctrlChInst->writeReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
        command->header.flags  = (uint16_t)(U3V_CTRL_REQ_ACK);
        command->header.cmd    = (uint16_t)(U3V_CTRL_READMEM_CMD);
        command->header.length = (uint16_t)sizeof(T_U3VCtrlChReadMemCmdPayload);

        if ((ctrlChInst->requestId + 1u) >= ctrlChInst->maxRequestId)
        {
            ctrlChInst->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlChInst->requestId));
        payload->address = memAddress + (uint64_t)totalBytesRead;
        payload->reserved = 0u;
        payload->byteCount = (uint16_t)(bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlChHandle.bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlChInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != U3V_HOST_RESULT_SUCCESS)
        {
            OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlChInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlChInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlChInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if (writeRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
            {
                /* write failed */
                OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            // vTaskDelay(pdMS_TO_TICKS(1));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0u;

            ctrlChInst->readReqSts.length = 0u;
            ctrlChInst->readReqSts.result = U3V_HOST_RESULT_FAILURE;
            ctrlChInst->readReqSts.transferHandle = U3V_HOST_TRANSFER_HANDLE_INVALID;

            ackBufferSize = sizeof(T_U3VCtrlChAckHeader) + MAX((size_t)(bytesThisIteration), sizeof(T_U3VCtrlChPendingAckPayload));
            memset(ctrlChInst->ackBuffer, 0, ackBufferSize);

            /* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlChHandle.bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlChInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            if (u3vResult != U3V_HOST_RESULT_SUCCESS)
            {
                OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            while ((ctrlChInst->readReqSts.length != ackBufferSize) ||
                   (ctrlChInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlChInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if (readRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
                {
                    /* read failed */
                    OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                // vTaskDelay(pdMS_TO_TICKS(1));
            }

            ack = (T_U3VCtrlChAcknowledge *)(ctrlChInst->ackBuffer);

            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_READMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlChInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_READMEM_ACK) && (ack->header.length != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlChPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
            if (ack->header.cmd == U3V_CTRL_PENDING_ACK)
            {
                pendingAck = (T_U3VCtrlChPendingAckPayload *)(ack->payload);
                ctrlChInst->u3vTimeout = MAX(ctrlChInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }

            /* Acknowledge received successfully */
            reqAcknowledged = true;

        } /* loop until reqAcknowledged == true */
        totalBytesRead += bytesThisIteration;
    } /* loop until totalBytesRead == transfSize */

    if (totalBytesRead != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    /* Extract the data */
	memcpy(buffer, ack->payload, totalBytesRead);
    *bytesRead = totalBytesRead;

    OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));

    return u3vResult;
}


T_U3VHostResult U3VHost_CtrlCh_WriteMemory(T_U3VControlChannelHandle ctrlChObj,
                                           T_U3VHostTransferHandle *transferHandle,
                                           uint64_t memAddress,
                                           size_t transfSize,
                                           uint32_t *bytesWritten,
                                           const void *buffer)
{
    T_U3VHostResult u3vResult = U3V_HOST_RESULT_SUCCESS;
    T_U3VControlChannelObj *ctrlChInst = NULL;
    T_U3VHostInstanceObj *u3vInstance = NULL;
    T_U3VHostTransferHandle tempTransferHandle;
    USB_HOST_RESULT hostResult;
    uint32_t maxBytesPerWrite = 0u;
    uint32_t totalBytesWritten = 0u;
    bool reqAcknowledged;
    T_U3VCtrlChAcknowledge *ack = NULL;
    T_U3VCtrlChPendingAckPayload *pendingAck = NULL;
    T_U3V_CtrlChWriteMemAckPayload *writeMemAck = NULL;
    size_t cmdBufferSize = sizeof(T_U3VCtrlChCmdHeader) + sizeof(T_U3VCtrlChWriteMemCmdPayload);
    size_t ackBufferSize = sizeof(T_U3VCtrlChAckHeader) + MAX(sizeof(T_U3V_CtrlChWriteMemAckPayload),
                                                              sizeof(T_U3VCtrlChPendingAckPayload));

    /* check input argument errors */
    u3vResult = (bytesWritten == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (buffer       == NULL) ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (transfSize   == 0u)   ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (ctrlChObj    == 0)    ? U3V_HOST_RESULT_HANDLE_INVALID    : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }
    
    ctrlChInst = (T_U3VControlChannelObj *)ctrlChObj;
    u3vInstance = (T_U3VHostInstanceObj *)ctrlChInst->u3vInstObj;
    maxBytesPerWrite = ctrlChInst->maxAckTransfSize - sizeof(T_U3VCtrlChAckHeader);

    u3vResult = (cmdBufferSize > ctrlChInst->maxCmdTransfSize)  ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (maxBytesPerWrite == 0u)                        ? U3V_HOST_RESULT_INVALID_PARAMETER : u3vResult;
    u3vResult = (u3vInstance == NULL)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (!u3vInstance->inUse)                           ? U3V_HOST_RESULT_DEVICE_UNKNOWN    : u3vResult;
    u3vResult = (u3vInstance->state != U3V_HOST_STATE_READY)    ? U3V_HOST_RESULT_BUSY              : u3vResult;

    if (u3vResult != U3V_HOST_RESULT_SUCCESS)
    {
        return u3vResult;
    }

    tempTransferHandle = (transferHandle != NULL) ? *transferHandle : tempTransferHandle;
    *bytesWritten = 0u;
 
    if(OSAL_MUTEX_Lock(&(ctrlChInst->readWriteLock), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
    {   
        u3vResult = U3V_HOST_RESULT_BUSY;
        return u3vResult;
    }

    while (totalBytesWritten < transfSize)
    {
        uint32_t bytesThisIteration = MIN((uint32_t)(transfSize - totalBytesWritten), maxBytesPerWrite);
        T_U3VCtrlChCommand *command = (T_U3VCtrlChCommand *)(ctrlChInst->cmdBuffer);
        T_U3VCtrlChWriteMemCmdPayload *payload = (T_U3VCtrlChWriteMemCmdPayload *)(command->payload);
        uint32_t writeRetryCnt = 0u;

        command->header.prefix = (uint32_t)(U3V_CONTROL_MGK_PREFIX);
		command->header.flags = (uint16_t)(U3V_CTRL_REQ_ACK);
		command->header.cmd = (uint16_t)(U3V_CTRL_WRITEMEM_CMD);
        command->header.length = (uint16_t)(sizeof(T_U3VCtrlChWriteMemCmdPayload) + bytesThisIteration);

        if ((ctrlChInst->requestId + 1u) >= ctrlChInst->maxRequestId)
        {
            ctrlChInst->requestId = 0u;
        }

        command->header.requestId = (uint16_t)(++(ctrlChInst->requestId));
		payload->address = memAddress + (uint64_t)totalBytesWritten;
        cmdBufferSize = sizeof(T_U3VCtrlChCmdHeader) + sizeof(T_U3VCtrlChWriteMemCmdPayload) + bytesThisIteration;

        memcpy(payload->data, (uint8_t *)(buffer + totalBytesWritten), bytesThisIteration);

        hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlChHandle.bulkOutPipeHandle,
                                             &tempTransferHandle,
                                             ctrlChInst->cmdBuffer,
                                             cmdBufferSize,
                                             (uintptr_t)(U3V_HOST_EVENT_WRITE_COMPLETE));

        u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

        if (u3vResult != USB_HOST_RESULT_TRUE)
        {
            OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
            return u3vResult;
        }

        while ((ctrlChInst->writeReqSts.length != cmdBufferSize) ||
               (ctrlChInst->writeReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
               (ctrlChInst->writeReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID))
        {
            /* Wait for write request to complete with retry limit */
            writeRetryCnt++;
            if (writeRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
            {
                /* write failed */
                OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                return u3vResult;
            }
            // vTaskDelay(pdMS_TO_TICKS(1));
        }

        reqAcknowledged = false;
        while (!reqAcknowledged)
        {
            uint32_t readRetryCnt = 0u;

            /* reset buffer */
			memset(ctrlChInst->ackBuffer, 0, ackBufferSize);

			/* read the ack */
            hostResult = USB_HOST_DeviceTransfer(u3vInstance->controlChHandle.bulkInPipeHandle,
                                                 &tempTransferHandle,
                                                 ctrlChInst->ackBuffer,
                                                 ackBufferSize,
                                                 (uintptr_t)(U3V_HOST_EVENT_READ_COMPLETE));

            u3vResult = _USB_HostU3V_HostToU3VResultsMap(hostResult);

            ack = (T_U3VCtrlChAcknowledge *)(ctrlChInst->ackBuffer);
            writeMemAck = (T_U3V_CtrlChWriteMemAckPayload *)(ack->payload);

            while ((ctrlChInst->readReqSts.length != ackBufferSize) ||
                   (ctrlChInst->readReqSts.result != U3V_HOST_RESULT_SUCCESS) ||
                   (ctrlChInst->readReqSts.transferHandle == U3V_HOST_TRANSFER_HANDLE_INVALID) ||
                   (ack->header.prefix != U3V_CONTROL_MGK_PREFIX))
            {
                /* Wait for read request to complete with retry limit */
                readRetryCnt++;
                if (readRetryCnt > U3V_REQ_READWRITE_RETRY_TIMES_LMT)
                {
                    /* read failed */
                    OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                    u3vResult = U3V_HOST_RESULT_REQUEST_STALLED;
                    return u3vResult;
                }
                // vTaskDelay(pdMS_TO_TICKS(1));
            }
            
            /* Inspect the acknowledge buffer */
            if (((ack->header.cmd != U3V_CTRL_WRITEMEM_ACK) && (ack->header.cmd != U3V_CTRL_PENDING_ACK)) ||
                (ack->header.prefix != U3V_CONTROL_MGK_PREFIX) ||
                (ack->header.status != U3V_ERR_NO_ERROR) ||
                (ack->header.ackId != ctrlChInst->requestId) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length != sizeof(T_U3V_CtrlChWriteMemAckPayload)) && (ack->header.length != 0u)) ||
                ((ack->header.cmd == U3V_CTRL_WRITEMEM_ACK) && (ack->header.length == sizeof(T_U3V_CtrlChWriteMemAckPayload)) && (writeMemAck->bytesWritten != bytesThisIteration)) ||
                ((ack->header.cmd == U3V_CTRL_PENDING_ACK) && (ack->header.length != sizeof(T_U3VCtrlChPendingAckPayload))))
            {
                OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
                u3vResult = U3V_HOST_RESULT_FAILURE;
                return u3vResult;
            }

            /* For a pending ack, update the timeout and resubmit the read request */
			if (ack->header.cmd == U3V_CTRL_PENDING_ACK) 
            {
                pendingAck = (T_U3VCtrlChPendingAckPayload *)(ack->payload);
                ctrlChInst->u3vTimeout = MAX(ctrlChInst->u3vTimeout, (uint32_t)(pendingAck->timeout));
                continue;
            }
            /* Acknowledge received successfully */
			reqAcknowledged = true;
        } /* loop until reqAcknowledged == true */
        totalBytesWritten += bytesThisIteration;
    } /* loop until totalBytesWritten == transfSize */

    if (totalBytesWritten != transfSize)
    {
        OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));
        u3vResult = U3V_HOST_RESULT_FAILURE;
        return u3vResult;
    }

    *bytesWritten = totalBytesWritten;

    OSAL_MUTEX_Unlock(&(ctrlChInst->readWriteLock));

    return u3vResult;
}


void U3VHost_CtrlCh_InterfaceDestroy(T_U3VControlChannelHandle *pU3vCtrlCh)
{
    T_U3VControlChannelObj *ctrlChobj = NULL;
    T_U3VHostInstanceObj   *u3vDev = NULL;

    if ((pU3vCtrlCh != NULL) && (*pU3vCtrlCh != 0))
    {
        ctrlChobj = (T_U3VControlChannelObj *)(*pU3vCtrlCh);
        u3vDev = (T_U3VHostInstanceObj *)(ctrlChobj->u3vInstObj);
        // wait_for_completion(&u3vInst->controlChHandle.ioctl_complete);
        // if (!u3vInst->stalling_disabled &&
        //     u3vInst->u3v_info->legacy_ctrl_ep_stall_enabled)
        // 	reset_pipe(u3vInst, &u3vInst->control_info);

        _FreeCtrlChObjAllocSpace(ctrlChobj);

        if (u3vDev != NULL)
        {
            u3vDev->controlChHandle.chanObj = NULL;
        }

        *pU3vCtrlCh = 0;

        controlIfObjData = NULL; // DEBUG
    }
    else
    {
        /* invalid handle */
    }
}


void _U3VHost_CtrlCh_TransferReqCompleteCbk(T_U3VHostHandle u3vObj, T_U3VHostEvent transfEvent, void *transfData)
{
    T_U3VHostInstanceObj *u3vInst;
    T_U3VHostEventWriteCompleteData *writeCompleteEventData;
    T_U3VHostEventReadCompleteData *readCompleteEventData;
    u3vInst = (T_U3VHostInstanceObj*) u3vObj;

    switch (transfEvent)
    {
        case U3V_HOST_EVENT_READ_COMPLETE:
            readCompleteEventData = (T_U3VHostEventReadCompleteData *)transfData;
            u3vInst->controlChHandle.chanObj->readReqSts = *readCompleteEventData;
            break;

        case U3V_HOST_EVENT_WRITE_COMPLETE:
            writeCompleteEventData = (T_U3VHostEventWriteCompleteData *)transfData;
            u3vInst->controlChHandle.chanObj->writeReqSts = *writeCompleteEventData;
            break;

        default:
            break;
    }
}
