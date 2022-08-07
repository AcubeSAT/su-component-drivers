/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

#include "main.h"
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "config/default/peripheral/pio/plib_pio.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************


void xTaskLedBlink(void *pvParameters)
{
    for(;;)
    {
//       main_cpp();
        GPIO_PA23_Toggle(); // LED0 Toggle XULT board
        vTaskDelay(pdMS_TO_TICKS(500));
    }

};

void xTaskUsbHostTasks(void *pvParameters)
{
    for(;;)
    {
        /* USB Host layer Task Routine */
        USB_HOST_Tasks(sysObj.usbHostObject0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
};

void xTaskDrvUsbHsV1Tasks(void *pvParameters)
{
    for(;;)
    {
        /* USB HS Driver Task Routine */
        DRV_USBHSV1_Tasks(sysObj.drvUSBHSV1Object);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
};

void xTaskU3VCamDriverTasks(void *pvParameters)
{
    for(;;)
    {
        /* USB Camera Driver Task Routine */
        U3VCamDriver_Tasks();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
};

void xTaskCamImgDataTestTasks(void *pvParameters)
{
    for(;;)
    {
        APP_Tasks();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
};


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    xTaskCreate(xTaskLedBlink, "Task1_LED_Blink", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(xTaskUsbHostTasks, "USB_HOST_TASKS", 1024, NULL, tskIDLE_PRIORITY + 4, NULL);
    xTaskCreate(xTaskDrvUsbHsV1Tasks, "DRV_USBHSV1_TASKS", 1024, NULL, tskIDLE_PRIORITY + 5, NULL);
    xTaskCreate(xTaskU3VCamDriverTasks, "USB_CAM_DRV_TASKS", 1024, NULL, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(xTaskCamImgDataTestTasks, "CAM_DATA_TEST_TASKS", 1024, NULL, tskIDLE_PRIORITY + 3, NULL);

    vTaskStartScheduler(); // never exits

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        // SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}






/*******************************************************************************
 End of File
*/
