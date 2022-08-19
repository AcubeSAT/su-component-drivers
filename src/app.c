/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "peripheral/pio/plib_pio.h"
#include "U3VCamDriver.h"
#include "peripheral/usart/plib_usart1.h"
#include "system/dma/sys_dma.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
static void _APP_U3vImgPldBlkRcvdCbk(T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, size_t blockSize, uint32_t blockCnt);
static void _APP_PioSw1PrsdCbk(PIO_PIN pin, uintptr_t context); // SW1 interrupt handler
/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.imgRequested = false;
    appData.imgPldPending = false;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
		case APP_STATE_INIT:
		{
      if ((U3V_CAM_DRV_OK == U3VCamDriver_SetImagePayldTransfParams(_APP_U3vImgPldBlkRcvdCbk, appData.imgData)) &&
          (PIO_PinInterruptCallbackRegister(GPIO_PB12_PIN, _APP_PioSw1PrsdCbk, 0)))
      {
        appData.usrtDrv = DRV_USART_Open(0, DRV_IO_INTENT_WRITE);
				PIO_PinInterruptEnable(GPIO_PB12_PIN);
				appData.state = APP_STATE_SERVICE_TASKS;
      }
      break;
		}

		case APP_STATE_SERVICE_TASKS:
        {
			if (appData.imgRequested && !appData.imgPldPending)
			{
				if (!SYS_DMA_ChannelIsBusy(SYS_DMA_CHANNEL_0))
				{
					if (U3V_CAM_DRV_OK == U3VCamDriver_RequestNewImagePayloadBlock())
					{
						appData.imgPldPending = true;
					}
				}
				else
				{
					appData.state = APP_STATE_SERVICE_TASKS; // debug
				}
			}
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


static void _APP_U3vImgPldBlkRcvdCbk(T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, size_t blockSize, uint32_t blockCnt)
{
	void *srcAddr = imgData;
	size_t size = blockSize;

	switch (event)
	{
		case U3V_CAM_DRV_IMG_LEADER_DATA:
			break;

		case U3V_CAM_DRV_IMG_TRAILER_DATA:
			appData.imgRequested = false;
			break;

		case U3V_CAM_DRV_IMG_PAYLOAD_DATA:
			if (true == DRV_USART_WriteBuffer(appData.usrtDrv, srcAddr, size))
			{
				/* Tx ok */
			}
			else
			{
				size = blockSize; // error breakpoint position, should never pass from here
			}
			break;

		default:
			break;
	}
	appData.imgPldPending = false;
}


static void _APP_PioSw1PrsdCbk(PIO_PIN pin, uintptr_t context) // SW1 interrupt handler
{
	(void)pin;
	(void)context;
	appData.imgRequested = true;
}


/*******************************************************************************
 End of File
 */
