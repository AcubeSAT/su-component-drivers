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

#include "SEGGER_RTT/SEGGER_RTT.h"
#include "MCP9808/Inc/MCP9808.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

volatile uint8_t pinval = 0;
volatile int xTask1 = 1;

MCP9808 TempSensor;
float temp;

void xTask1Code(void *pvParameters){

    for(;;){
        TempSensor.getTemp(temp);


        SEGGER_RTT_printf(0, "the value is %f\n", temp);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

};

void xTask2Code(void *pvParameters){

    for(;;){
        //PIO_PinToggle(PIO_PIN_PA23);
        pinval = PIO_PinRead(PIO_PIN_PA23);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

};


int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );



    xTaskCreate(xTask1Code, "Task1",100, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(xTask2Code, "Task2",100, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();
    while ( true )
    {


        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}





/*******************************************************************************
 End of File
*/
