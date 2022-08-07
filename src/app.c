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

static void _U3vImagePartReceivedCbk(T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, uint32_t blockSize, uint32_t blockCnt);

static void _PioSw1_U3VAcquireNewImage(PIO_PIN pin, uintptr_t context) // SW1 interrupt handler
{
  (void)pin;
  (void)context;
  U3VCamDriver_AcquireNewImage(NULL);
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

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

    PIO_PinInterruptCallbackRegister(GPIO_PB12_PIN, _PioSw1_U3VAcquireNewImage, 0);
    PIO_PinInterruptEnable(GPIO_PB12_PIN);

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
            bool appInitialized = true;

            if (U3V_CAM_DRV_OK == U3VCamDriver_SetImageAcqPayloadEventCbk(_U3vImagePartReceivedCbk, 0))
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {

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


static void _U3vImagePartReceivedCbk(T_U3VCamDriverImageAcqPayloadEvent event, void *imgData, uint32_t blockSize, uint32_t blockCnt)
{    

  switch (event)
  {
    case U3V_CAM_DRV_IMG_LEADER_DATA:
      break;

    case U3V_CAM_DRV_IMG_TRAILER_DATA:
      break;

    case U3V_CAM_DRV_IMG_PAYLOAD_DATA:

      break;

    default:
      break;
  }
}

/*******************************************************************************
 End of File
 */
