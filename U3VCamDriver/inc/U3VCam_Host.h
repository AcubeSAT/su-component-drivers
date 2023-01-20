#pragma once

#include "U3VCam_Device_Class_Specs.h"
#include "U3VCam_Config.h"
#include "usb/usb_host_client_driver.h"

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
* Macro definitions
*******************************************************************************/

#define U3V_HOST_RESULT_MIN                     (USB_HOST_RESULT_MIN)
#define U3V_HOST_HANDLE_INVALID                 ((T_U3VHostHandle)UINTPTR_MAX)
#define U3V_HOST_TRANSFER_HANDLE_INVALID        ((T_U3VHostTransferHandle)UINTPTR_MAX)
#define U3V_INTERFACE                           (&gUSBHostU3VClientDriver)


/*******************************************************************************
* Type definitions
*******************************************************************************/

/**
 * U3V Host handle.
 * 
 */
typedef uintptr_t T_U3VHostHandle;

/**
 * U3V Host transfer handle.
 * 
 */
typedef uintptr_t T_U3VHostTransferHandle;

/**
 * Area of struct typedefs with forced 1 byte packing.
 * 
 * For datatypes used in USB3 Vision protocol frames which require strict 1 byte 
 * packing when being sent over USB bus.
 */
#pragma pack(push, 1)

/**
 * U3V Stream Interface generic packet.
 * 
 */
typedef struct 
{
    uint32_t        magicKey;           /* "U3VL" for Leader / "U3VT" for Trailer */
	uint16_t        reserved0;          /* Set 0 on Tx, ignore on Rx */
	uint16_t        size;
	uint64_t        blockID;
    void            *data;
} T_U3VSiGenericPacket;

U3V_STATIC_ASSERT(sizeof(T_U3VSiGenericPacket) == 20, "Packing error for T_U3VSiGenericPacket");

/* end of forced 1 byte packing */
#pragma pack(pop)

/**
 * U3V Host result.
 * 
 */
typedef enum
{
    U3V_HOST_RESULT_FAILURE             = U3V_HOST_RESULT_MIN,
    U3V_HOST_RESULT_BUSY,
    U3V_HOST_RESULT_REQUEST_STALLED,
    U3V_HOST_RESULT_INVALID_PARAMETER,
    U3V_HOST_RESULT_DEVICE_UNKNOWN,
    U3V_HOST_RESULT_ABORTED,
    U3V_HOST_RESULT_HANDLE_INVALID,
    U3V_HOST_RESULT_SUCCESS             = 1
} T_U3VHostResult;

/**
 * U3V Host memory register integer type.
 * 
 * List of supported camera memory registers that hold an integer value.
 */
typedef enum
{
    U3V_MEM_REG_INT_ACQ_MODE,
    U3V_MEM_REG_INT_ACQ_START,
    U3V_MEM_REG_INT_ACQ_STOP,
    U3V_MEM_REG_INT_DEVICE_RESET,
    U3V_MEM_REG_INT_PAYLOAD_SIZE,
    U3V_MEM_REG_INT_PIXEL_FORMAT,
} T_U3VMemRegInteger;

/**
 * U3V Host memory register float type.
 * 
 * List of supported camera memory registers that hold a float value.
 */
typedef enum
{
    U3V_MEM_REG_FLOAT_TEMPERATURE,
} T_U3VMemRegFloat;

/**
 * U3V Host memory register string type.
 * 
 * List of supported camera memory registers that hold a string (text) value.
 */
typedef enum
{
    U3V_MEM_REG_STRING_MANUFACTURER_NAME,
    U3V_MEM_REG_STRING_MODEL_NAME,
    U3V_MEM_REG_STRING_FAMILY_NAME,
    U3V_MEM_REG_STRING_DEVICE_VERSION,
    U3V_MEM_REG_STRING_MANUFACTURER_INFO,
    U3V_MEM_REG_STRING_SERIAL_NUMBER,
    U3V_MEM_REG_STRING_USER_DEFINED_NAME,
} T_U3VMemRegString;

/**
 * U3V Host event read/write complete data.
 * 
 * Event context that will return upon completion of a read or write transfer 
 * request. 
 */
typedef struct
{   
    T_U3VHostTransferHandle  transferHandle;
    T_U3VHostResult          result;
    size_t                   length;
} T_U3VHostEventReadCompleteData, T_U3VHostEventWriteCompleteData;

/**
 * U3V Host event.
 * 
 */
typedef enum
{
    U3V_HOST_EVENT_READ_COMPLETE = 1,
    U3V_HOST_EVENT_WRITE_COMPLETE,
    U3V_HOST_EVENT_IMG_PLD_RECEIVED,
} T_U3VHostEvent;

/**
 * U3V Host event response.
 * 
 * @note Currently used as a placeholder.
 */
typedef enum
{
    U3V_HOST_EVENT_RESPONE_NONE
} T_U3VHostEventResponse;

/**
 * U3V device info.
 * 
 * Miscellaneous information for the connected U3V device.
 */
typedef struct 
{
	uint64_t    sirmAddr;
    uint32_t    hostByteAlignment;
    uint32_t    transferAlignment;
} T_U3VDeviceInfo;

/**
 * U3V Host attach event handler.
 * 
 * Callback function for the event on a new U3V device attachment (called by the
 * attach listener). 
 */
typedef void (*T_U3VHostAttachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

/**
 * U3V Host detach event handler.
 * 
 * Callback function for the event of the detachment of a connected U3V device.
 */
typedef void (*T_U3VHostDetachEventHandler)(T_U3VHostHandle u3vObjHandle, uintptr_t context);

/**
 * U3V Host event response handler.
 * 
 * Callback function for handling U3V Host events, mainly for incoming image 
 * payload data.
 */
typedef T_U3VHostEventResponse (*T_U3VHostEventHandler)(T_U3VHostHandle u3vObjHandle, T_U3VHostEvent event, void *eventData, uintptr_t context);


/*******************************************************************************
* Global data
*******************************************************************************/

extern USB_HOST_CLIENT_DRIVER   gUSBHostU3VClientDriver;


/*******************************************************************************
* Function declarations
*******************************************************************************/

/**
 * U3V Host device attach event handler set.
 * 
 * This function shall be used by the U3V application to assign a callback 
 * function that will be called when a new U3V device is attached.
 * @param eventHandler 
 * @param context 
 * @return T_U3VHostResult 
 * @note callback type to set 'T_U3VHostAttachEventHandler'
 */
T_U3VHostResult U3VHost_AttachEventHandlerSet(T_U3VHostAttachEventHandler eventHandler, uintptr_t context);

/**
 * U3V Host device detach event handler set.
 * 
 * This function shall be used by the U3V application to assign a callback 
 * function that will be called when a connected U3V device is detached.
 * @param handle 
 * @param detachEventHandler 
 * @param context 
 * @return T_U3VHostResult
 * @note callback type to set 'T_U3VHostDetachEventHandler'
 */
T_U3VHostResult U3VHost_DetachEventHandlerSet(T_U3VHostHandle handle, T_U3VHostDetachEventHandler detachEventHandler, uintptr_t context);

/**
 * U3V Host open obj handle.
 * 
 * This function returns a U3V host handle to the application that can be used 
 * as a reference to a specific instance.
 * @param u3vObjHandle 
 * @return T_U3VHostHandle 
 */
T_U3VHostHandle U3VHost_Open(T_U3VHostHandle u3vObjHandle);

/**
 * U3V Host event handler set.
 * 
 * This function shall be used by the U3V application to assign a callback 
 * function that will be called when a host (transfer) event occurs. A typical 
 * code example can be seen below.
 * @param handle 
 * @param eventHandler 
 * @param context 
 * @return T_U3VHostResult 
 * @code
 * static T_U3VHostEventResponse U3VApp_HostEventHandlerCbk(T_U3VHostHandle u3vHandle, T_U3VHostEvent event, void *pEventData, uintptr_t context)
 * {
 *     T_U3VHostEventReadCompleteData  *readCompleteEventData;
 *     T_U3VAppData                    *pUsbU3VAppData;
 *     T_U3VSiGenericPacket            *pckLeaderOrTrailer;
 * 
 *     pUsbU3VAppData = (T_U3VAppData*)context;
 *     readCompleteEventData = (T_U3VHostEventReadCompleteData *)(pEventData);
 *     T_U3VCamDriverImageAcqPayloadEvent appPldTransfEvent;
 * 
 *     switch (event)
 *     {
 *         case U3V_HOST_EVENT_IMG_PLD_RECEIVED:
 *             pckLeaderOrTrailer = (T_U3VSiGenericPacket*)pUsbU3VAppData->appImgDataBfr;
 *             pUsbU3VAppData->appImgBlockCounter++;
 *             if (pckLeaderOrTrailer->magicKey == (uint32_t)U3V_LEADER_MGK_PREFIX)
 *             {
 *                 pUsbU3VAppData->appImgTransfState = U3V_SI_IMG_TRANSF_STATE_LEADER_COMPLETE;
 *                 appPldTransfEvent = U3V_CAM_DRV_IMG_LEADER_DATA;
 *                 pUsbU3VAppData->appImgBlockCounter = UINT32_C(0);
 *             }
 *             else if (pckLeaderOrTrailer->magicKey == (uint32_t)U3V_TRAILER_MGK_PREFIX)
 *             {
 *                 pUsbU3VAppData->appImgTransfState = U3V_SI_IMG_TRANSF_STATE_TRAILER_COMPLETE;
 *                 appPldTransfEvent = U3V_CAM_DRV_IMG_TRAILER_DATA;
 *             }
 *             else
 *             {
 *                 appPldTransfEvent = U3V_CAM_DRV_IMG_PAYLOAD_DATA;
 *             }
 *             if (u3vAppData.appImgEvtCbk != NULL)
 *             {
 *                 u3vAppData.appImgEvtCbk(appPldTransfEvent,
 *                                         u3vAppData.appImgDataBfr,
 *                                         readCompleteEventData->length,
 *                                         pUsbU3VAppData->appImgBlockCounter);
 *             }
 *             break;
 * 
 *         case U3V_HOST_EVENT_WRITE_COMPLETE:
 *         case U3V_HOST_EVENT_READ_COMPLETE:
 *         default:
 *             break;
 *     }
 *     return U3V_HOST_EVENT_RESPONE_NONE;
 * }
 * @endcode
 */
T_U3VHostResult U3VHost_EventHandlerSet(T_U3VHostHandle handle, T_U3VHostEventHandler eventHandler, uintptr_t context);

/**
 * U3V Host get Stream Interface capabilities.
 * 
 * This function shall be used by the application to fetch Stream interface 
 * related data from the connected U3V device.
 * @param u3vObjHandle 
 * @return T_U3VHostResult 
 * @warning must be called after the initialization of Control IF
 */
T_U3VHostResult U3VHost_GetStreamCapabilities(T_U3VHostHandle u3vObjHandle);

/**
 * U3V Host setup Stream capabilities.
 * 
 * This function shall be used by the application to setup Stream interface 
 * related data from the conected U3V device.
 * @param u3vObjHandle 
 * @param imgPayloadSize 
 * @return T_U3VHostResult 
 */
T_U3VHostResult U3VHost_SetupStreamIfTransfer(T_U3VHostHandle u3vObjHandle, uint32_t imgPayloadSize);

/**
 * U3V Host control Stream interface activity.
 * 
 * @param u3vObjHandle 
 * @param enable 
 * @return T_U3VHostResult 
 */
T_U3VHostResult U3VHost_StreamIfControl(T_U3VHostHandle u3vObjHandle, bool enable);

/**
 * U3V Host start image payload transfer function.
 * 
 * This function shall be called by the application to initiate the receive 
 * procedure of an image payload block, with the specified size. To avoid data 
 * loss, make sure that the requested size is a multiple of the byte packing 
 * size.
 * @param u3vObjHandle 
 * @param imgBfr 
 * @param size 
 * @return T_U3VHostResult 
 * @note For optimized results, prefer using 1024 or 512 byte size (same as 
 * U3V_PAYLD_BLOCK_MAX_SIZE), but make sure that the USB Host Layer below is 
 * USB2.0-HS.
 */
T_U3VHostResult U3VHost_StartImgPayldTransfer(T_U3VHostHandle u3vObjHandle, void *imgBfr, size_t size);

/**
 * U3V Host Control Interface create function.
 * 
 * This function shall be called by the application after the enumaration of the
 * U3V device (handle opening) in order to establish the basic register memory 
 * R/W interfaces to get access to all camera functionalities.
 * @param u3vObjHandle 
 * @return T_U3VHostResult
 * @warning This function must be called after the U3V Host handle ID has been 
 * acquired by the application.
 */
T_U3VHostResult U3VHost_CtrlIf_InterfaceCreate(T_U3VHostHandle u3vObjHandle);

/**
 * U3V Host Control Interface destrio function.
 * 
 * This function shall be called by the application after a connected U3V device
 * has been detached, in order to clear and reset all data previously 
 * established on the Control Interface.
 * @param u3vObjHandle 
 */
void U3VHost_CtrlIf_InterfaceDestroy(T_U3VHostHandle u3vObjHandle);

/**
 * U3V Host Read memory register integer value.
 * 
 * This function shall be called by the application in order to read a memory 
 * register of the connected U3V camera, which holds an integer value.
 * @param u3vObjHandle 
 * @param integerReg 
 * @param pReadValue 
 * @return T_U3VHostResult
 * @warning This function shall only be called after the Control Interface has 
 * been established.
 * @note Available integer registers can be seen in enum T_U3VMemRegInteger.
 */
T_U3VHostResult U3VHost_ReadMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t *pReadValue);

/**
 * U3V Host Write memory register integer value.
 * 
 * This function shall be called by the application in order to write a memory
 * register of the connected U3V camera, which holds an integer value, with a 
 * new integer value.
 * @param u3vObjHandle 
 * @param integerReg 
 * @param writeValue 
 * @return T_U3VHostResult
 * @warning This function shall only be called after the Control Interface has 
 * been established.
 * @note Available integer registers can be seen in enum T_U3VMemRegInteger.
 */
T_U3VHostResult U3VHost_WriteMemRegIntegerValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegInteger integerReg, uint32_t writeValue);

/**
 * U3V Host Read memory register float value.
 * 
 * This function shall be called by the application in order to read a memory 
 * register of the connected U3V camera, which holds a float value.
 * @param u3vObjHandle 
 * @param floatReg 
 * @param pReadValue 
 * @return T_U3VHostResult
 * @warning This function shall only be called after the Control Interface has 
 * been established.
 * @note Available float registers can be seen in enum T_U3VMemRegFloat.
 */
T_U3VHostResult U3VHost_ReadMemRegFloatValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegFloat floatReg, float *pReadValue);

/**
 * U3V Host Read memory register string value.
 * 
 * This function shall be called by the application in order to read a memory 
 * register of the connected U3V camera, which holds a strint (text) value.
 * @param u3vObjHandle 
 * @param stringReg 
 * @param pReadBfr 
 * @return T_U3VHostResult
 * @warning This function shall only be called after the Control Interface has 
 * been established. The size of the string value can reach up to 64 bytes, 
 * therefore use a buffer which can hold at least 64 bytes. The buffer's  base 
 * type shall be either uint8_t or char.
 * @note Available string registers can be seen in enum T_U3VMemRegString.
 */
T_U3VHostResult U3VHost_ReadMemRegStringValue(T_U3VHostHandle u3vObjHandle, T_U3VMemRegString stringReg, void *pReadBfr);


#ifdef __cplusplus
}
#endif //__cplusplus

