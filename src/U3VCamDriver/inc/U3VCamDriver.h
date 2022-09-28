
#ifndef COMPONENT_DRIVERS_U3VCAMDRIVER_H
#define COMPONENT_DRIVERS_U3VCAMDRIVER_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Type definitions
*********************************************************/

/**
 * U3VCamDriver driver status datatype.
 * 
 * Enum which describes the operability status of the 
 * U3VCamDriver.
 * @note On normal operation after initialization, the 
 * expected result shall always be 'U3V_CAM_DRV_OK'.
 */
typedef enum
{
    U3V_CAM_DRV_NOT_INITD   = -2,
    U3V_CAM_DRV_ERROR       = -1,
    U3V_CAM_DRV_OK          =  0
} T_U3VCamDriverStatus;

/**
 * U3V Camera text descriptor text datatype.
 * 
 * Enum to specify the text descriptor to get.
 * @note All text descriptors are considered 64 byte long
 * by convention, even when their content is shorter.
 */
typedef enum
{
    U3V_CAM_DRV_GET_TEXT_SERIAL_NUMBER,
    U3V_CAM_DRV_GET_TEXT_MANUFACTURER_NAME,
    U3V_CAM_DRV_GET_TEXT_MODEL_NAME,
    U3V_CAM_DRV_GET_TEXT_DEVICE_VERSION,
} T_U3VCamDriverDeviceDescriptorTextType;

/**
 * Image payload block event datatype.
 * 
 * Received image payload block event notifies the higher
 * level app about the type of the received payload block
 * type (leader, payload data or trailer). While the 
 * 'leader' and 'trailer' packets do not contain image 
 * pixel data, can sometimes be used to determine image
 * related information, such as size, offset, etc. More 
 * information regarding 'leader' and 'trailer' packets
 * usage can be found in official 'GenICam' documentation.
 * @note The 'leader' packet always arrives first in order.
 * @note The 'trailer' packet always arrives in the end,
 * signaling end of image acquisition.
 * @note The 'payload data' packets arrive between the 
 * 'leader' and 'trailer' packets, carrying actual image
 * pixel data.
 */
typedef enum
{
    U3V_CAM_DRV_IMG_LEADER_DATA = 1,
    U3V_CAM_DRV_IMG_PAYLOAD_DATA,
    U3V_CAM_DRV_IMG_TRAILER_DATA,
} T_U3VCamDriverImageAcqPayloadEvent;

/**
 * Image payload block received event callback datatype.
 *
 * This datatype defines the callback function type to be
 * used by the higher level application, which will notify
 * the app that an image payload block has been received.
 * The type of the received payload block is defined by
 * the 'event' (see T_U3VCamDriverImageAcqPayloadEvent).
 * The 'leader' packet arrives first in order.
 * The 'payload data' packets then arrive in series (approx
 * 3,7k packets when buffer size 1024 bytes, for RGB8)
 * The 'trailer' packet then arrives at the end of the
 * transfer, singaling the end of the image acquisition.
 * A basic example of this callback is seen below:
 * 
 * void APP_U3vImgPldBlkRcvCbk(T_U3VCamDriverImageAcqPayloadEvent event, void *imageData, size_t blockSize, uint32_t blockCount)
 * {
 *      switch (event)
 * 	    {
 * 		    case U3V_CAM_DRV_IMG_LEADER_DATA:
 *              // leader packet received, request new block for image payload data
 * 		    	break;
 *  
 * 		    case U3V_CAM_DRV_IMG_TRAILER_DATA:
 *              // trailer packet received, image payload transfer complete
 * 		    	imageRequested = false;
 * 		    	break;
 *  
 * 		    case U3V_CAM_DRV_IMG_PAYLOAD_DATA:
 * 		    	if (true == DRV_USART_WriteBuffer(usrtDrv, imageData, blockSize))
 * 		    	{
 *                  // transfer ok, can request new block
 * 		    	}
 *              else
 *              {
 *                  // transfer not ok, do something
 *              }
 * 		    	break;
 *  
 * 		    default:
 * 		    	break;
 * 	    }
 * 	    imagePayloadBlockPending = false;
 * }
 * 
 * @note When a new image block is requested, the app shall
 * wait until this callback is triggered. Multiple calls of
 * the U3VCamDriver_RequestNewImagePayloadBlock in the time
 * between, may not result in the equal amount of packets
 * returned, as the requests are handled asynchronously.
 */
typedef void (*T_U3VCamDriverPayloadEventCallback) (T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, size_t blockSize, uint32_t blockCnt);



/********************************************************
* Function declarations
*********************************************************/

/**
 * U3VCamDriver Initializer function.
 * 
 * This function has to be run once at startup to initialize
 * U3VCamDriver static values.
 */
void U3VCamDriver_Initialize(void);

/**
 * U3VCamDriver cyclic routine.
 * 
 * This function is the main driver's routine that has to be
 * mapped to an OS cyclic task. Has to run along with the
 * USB Host and USB-HS driver tasks, on equal timing.
 * 
 * @note Use 10ms cyclic task time, for optimized results.
 */
void U3VCamDriver_Tasks(void);

/**
 * Set image payload transfer parameters for U3VCamDriver.
 * 
 * This function provides the configuration interface to the
 * higher level application, where the app callback and the
 * payload transfer buffer address needs to be specified. 
 * This is assumed to be called once after initialization
 * has finished, but can also be called during runtime,
 * as long as the driver is in idle state, not transfering
 * data. The size of the data buffer must be at least the 
 * size of the image payload block size, which is defined 
 * with the U3V_IN_BUFFER_MAX_SIZE macro (is local). If the
 * buffer size is allocated in runtime, the function
 * U3VCamDriver_GetImagePayldMaxBlockSize may be used.
 * 
 * @param callback Callback to the app software to notify
 * the app that an image payload block has been received.
 * @param imgDataBfr Buffer address where image payload 
 * block will be copied into.
 * @return T_U3VCamDriverStatus Status of the driver that
 * indicates the operability of the driver.
 */
T_U3VCamDriverStatus U3VCamDriver_SetImagePayldTransfParams(T_U3VCamDriverPayloadEventCallback callback, void *imgDataBfr);

/**
 * Request a new image payload block from U3VCamDriver.
 * 
 * This function is called by the higher level application
 * to submit a request for a new image payload block. In
 * order for the request to be processed properly, the app
 * has first to configure the app callback and the image
 * data buffer via the U3VCamDriver_SetImagePayldTransfParams.
 * When the image payload block has been received and 
 * transfered into the data buffer, the app callback will be
 * called to notify about the received block with an event
 * (T_U3VCamDriverImageAcqPayloadEvent) which indicates the 
 * type of the received image payload block. The 'leader' is
 * the first block to be received, then a large series of
 * 'payload data' containing the actual image data follow,
 * until the 'leader' is received, which is the last
 * informative block to be received, containing no image
 * pixel data but image related information.
 * The request action is handled asynchronously by the 
 * driver's main routine.
 *  
 * @return T_U3VCamDriverStatus Status of the driver that
 * indicates the operability of the driver.
 * @note When a new image block is requested, the app shall
 * wait until the app callback is triggered. Multiple calls 
 * of the U3VCamDriver_RequestNewImagePayloadBlock in the 
 * time between, may not result in equal amount of blocks
 * returned, as the requests are handled asynchronously.
 * The order shall be like:
 * -request new img block
 * -wait for callback
 * -callback called
 * -request new img block
 * -...
 * -until all packets are received (trailer packet signals end)
 */
T_U3VCamDriverStatus U3VCamDriver_RequestNewImagePayloadBlock(void);

/**
 * Cancel ongoing image acquisition request from U3VCamDriver.
 * 
 * This function can be called during an ongoing image
 * acquisition request which is not yet complete. This 
 * action will stop the ongoing image payload transfer.
 * The cancel action is handled asynchronously by the 
 * driver's main routine.
 */
void U3VCamDriver_CancelImageAcqRequest(void);

/**
 * Get a selected text descriptor from the connected camera.
 * 
 * Get a text descriptor (T_U3VCamDriverDeviceDescriptorTextType)
 * from the connected camera regarding device information.
 * The text descriptors are received by the connected device
 * during power-on time and they are stored in driver's local
 * data (RAM). Every time this function is called, the text 
 * descriptors are copied by the RAM area and not by the camera 
 * directly.
 * 
 * @param textType Enum to select text descriptor type.
 * @param buffer Data buffer for the received text descriptor.
 * @return T_U3VCamDriverStatus Status of the driver that
 * indicates the operability of the driver.
 * @warning The input buffer to hold text values must be 64 
 * bytes (at least) in length and its basetype must be 'char'
 * or 'uint8_t', for every text descriptor selection.
 */
T_U3VCamDriverStatus U3VCamDriver_GetDeviceTextDescriptor(T_U3VCamDriverDeviceDescriptorTextType textType, void *buffer);

/**
 * Get camera's temperature in Celcius. 
 * 
 * Get the connected camera's last temperature reading in
 * Celcius scale. The temperature is read during power-on
 * time and is stored in driver's local data (RAM). Every 
 * time this function is called, the temperature is copied 
 * by the RAM area and not by the camera directly, thus 
 * the reading may be as much 'old' as the time that passed
 * since the powering-on of the camera and shall not be
 * considered as a precise measurement for critical
 * operations.
 * 
 * @param temperatureC Float type pointer of the memory 
 * area where the temperature reading will be copied into.
 * @return T_U3VCamDriverStatus Status of the driver that
 * indicates the operability of the driver.
 */
T_U3VCamDriverStatus U3VCamDriver_GetDeviceTemperature(float *temperatureC);

/**
 * Request Camera software reset via U3VCamDriver.
 * 
 * This function may be used to request a software reset
 * (soft reset) of the connected camera. This action will
 * be handled asynchronously by the driver's main routine.
 * This is not an essential functionality by any means
 * and may be used by the higher level app in cases where 
 * the Power Reset to the camera is not an option.
 * 
 * @return T_U3VCamDriverStatus Status of the driver that
 * indicates the operability of the driver.
 */
T_U3VCamDriverStatus U3VCamDriver_CamSwReset(void);

/**
 * Get the image payload block maximum size of the U3VCamDriver.
 * 
 * This function returns the maximum block size of the image
 * payload data received after every transfer request
 * (U3VCamDriver_RequestNewImagePayloadBlock). The size
 * value returned is a constant and the returned value 
 * will never change on runtime. May be used in cases when
 * the buffer size to hold image data is allocated in
 * runtime without a constant size value. Another way to
 * find the appropriate buffer size for the higher level
 * app, is to check the value of U3V_IN_BUFFER_MAX_SIZE
 * before build and define a constant with equal or larger 
 * size.
 * 
 * @return size_t Max size of the image payload block 
 * @note The return value comes from a constant
 * (U3V_IN_BUFFER_MAX_SIZE)
 */
size_t U3VCamDriver_GetImagePayldMaxBlockSize(void);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAMDRIVER_H
