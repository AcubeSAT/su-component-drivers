/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "definitions.h"
#include "device.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************
#pragma config TCM_CONFIGURATION = 0
#pragma config SECURITY_BIT = CLEAR
#pragma config BOOT_MODE = SET


// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="DRV_USART Instance 0 Initialization Data">

static DRV_USART_CLIENT_OBJ drvUSART0ClientObjPool[DRV_USART_CLIENTS_NUMBER_IDX0];


const DRV_USART_PLIB_INTERFACE drvUsart0PlibAPI = {
    .readCallbackRegister = (DRV_USART_PLIB_READ_CALLBACK_REG)USART1_ReadCallbackRegister,
    .read = (DRV_USART_PLIB_READ)USART1_Read,
    .readIsBusy = (DRV_USART_PLIB_READ_IS_BUSY)USART1_ReadIsBusy,
    .readCountGet = (DRV_USART_PLIB_READ_COUNT_GET)USART1_ReadCountGet,
    .readAbort = (DRV_USART_PLIB_READ_ABORT)USART1_ReadAbort,
    .writeCallbackRegister = (DRV_USART_PLIB_WRITE_CALLBACK_REG)USART1_WriteCallbackRegister,
    .write = (DRV_USART_PLIB_WRITE)USART1_Write,
    .writeIsBusy = (DRV_USART_PLIB_WRITE_IS_BUSY)USART1_WriteIsBusy,
    .writeCountGet = (DRV_USART_PLIB_WRITE_COUNT_GET)USART1_WriteCountGet,
    .errorGet = (DRV_USART_PLIB_ERROR_GET)USART1_ErrorGet,
    .serialSetup = (DRV_USART_PLIB_SERIAL_SETUP)USART1_SerialSetup
};

const uint32_t drvUsart0remapDataWidth[] = { 0x0, 0x40, 0x80, 0xC0, 0x20000 };
const uint32_t drvUsart0remapParity[] = { 0x800, 0x0, 0x200, 0x600, 0x400, 0xC00 };
const uint32_t drvUsart0remapStopBits[] = { 0x0, 0x1000, 0x2000 };
const uint32_t drvUsart0remapError[] = { 0x20, 0x80, 0x40 };

const DRV_USART_INIT drvUsart0InitData =
{
    .usartPlib = &drvUsart0PlibAPI,
    .numClients = DRV_USART_CLIENTS_NUMBER_IDX0,
    .clientObjPool = (uintptr_t)&drvUSART0ClientObjPool[0],
    .dmaChannelTransmit = DRV_USART_XMIT_DMA_CH_IDX0,
    .usartTransmitAddress = (void *)&(USART1_REGS->US_THR),
    .dmaChannelReceive = SYS_DMA_CHANNEL_NONE,
    .remapDataWidth = drvUsart0remapDataWidth,
    .remapParity = drvUsart0remapParity,
    .remapStopBits = drvUsart0remapStopBits,
    .remapError = drvUsart0remapError,
    .dataWidth = DRV_USART_DATA_8_BIT,
};

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;


// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************
/******************************************************
 * USB Driver Initialization
 ******************************************************/

void DRV_USB_VBUSPowerEnable(uint8_t port, bool enable)
{
    /* Note: When operating in Host mode, the application can specify a Root
       hub port enable function. The USB Host Controller driver init data
       structure has a member for specifying the root hub enable function.
       This parameter should point to Root hub port enable function. If this
       parameter is NULL, it implies that the port is always enabled.

       This function generated by MHC to let the user enable the root hub.
       User must use the MHC pin configuration utility to configure the pin
       used for enabling VBUS  */
    if (enable == true)
    {
        /* Enable the VBUS */
//		VBUS_AH_PowerEnable();
    }
    else
    {
        /* Disable the VBUS */
//		VBUS_AH_PowerDisable();
    }
}

const DRV_USBHSV1_INIT drvUSBInit =
        {
                .interruptSource = USBHS_IRQn,
                .moduleInit = {0},
                .operationMode = DRV_USBHSV1_OPMODE_HOST,
                .operationSpeed = DRV_USBHSV1_DEVICE_SPEEDCONF_NORMAL,
                .usbID = USBHS_REGS,
                .portPowerEnable = NULL, //DRV_USB_VBUSPowerEnable,
                .rootHubAvailableCurrent = 500,
        };


// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
// <editor-fold defaultstate="collapsed" desc="SYS_TIME Initialization Data">

const SYS_TIME_PLIB_INTERFACE sysTimePlibAPI = {
        .timerCallbackSet = (SYS_TIME_PLIB_CALLBACK_REGISTER)TC0_CH0_TimerCallbackRegister,
        .timerStart = (SYS_TIME_PLIB_START)TC0_CH0_TimerStart,
        .timerStop = (SYS_TIME_PLIB_STOP)TC0_CH0_TimerStop ,
        .timerFrequencyGet = (SYS_TIME_PLIB_FREQUENCY_GET)TC0_CH0_TimerFrequencyGet,
        .timerPeriodSet = (SYS_TIME_PLIB_PERIOD_SET)TC0_CH0_TimerPeriodSet,
        .timerCompareSet = (SYS_TIME_PLIB_COMPARE_SET)TC0_CH0_TimerCompareSet,
        .timerCounterGet = (SYS_TIME_PLIB_COUNTER_GET)TC0_CH0_TimerCounterGet,
};

const SYS_TIME_INIT sysTimeInitData =
        {
                .timePlib = &sysTimePlibAPI,
                .hwTimerIntNum = TC0_CH0_IRQn,
        };

// </editor-fold>


// *****************************************************************************
// *****************************************************************************
// Section: Local initialization functions
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{

    EFC_Initialize();

    CLOCK_Initialize();
    PIO_Initialize();

    XDMAC_Initialize();

    RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;	// Disable RSWDT

    WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk; 		// Disable WDT

    TC0_CH0_TimerInitialize();


    USART1_Initialize();

    sysObj.drvUsart0 = DRV_USART_Initialize(DRV_USART_INDEX_0, (SYS_MODULE_INIT *)&drvUsart0InitData);

    sysObj.sysTime = SYS_TIME_Initialize(SYS_TIME_INDEX_0, (SYS_MODULE_INIT *)&sysTimeInitData);

    sysObj.usbHostObject0 = USB_HOST_Initialize (( SYS_MODULE_INIT *)& usbHostInitData );

    sysObj.drvUSBHSV1Object = DRV_USBHSV1_Initialize(DRV_USBHSV1_INDEX_0, (SYS_MODULE_INIT *) &drvUSBInit);

    U3VCamDriver_Initialize();

    APP_Initialize();

    NVIC_Initialize();

}


/*******************************************************************************
 End of File
*/
