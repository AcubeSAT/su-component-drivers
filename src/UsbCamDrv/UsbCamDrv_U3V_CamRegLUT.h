//
// Created by mojo on 11/05/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_U3V_CAMREGLUT_H
#define COMPONENT_DRIVERS_USBCAMDRV_U3V_CAMREGLUT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

#define U3V_FLIR_CM3_U3_12S2C_CS_LUT_ENTRY      (0)
#define U3V_XIMEA_XIQ_MQ013CG_E2_LUT_ENTRY      (1)


/********************************************************
* Type definitions
*********************************************************/

typedef struct
{
    uint64_t camRegBaseAddress;
    uint64_t SBRMOffset;
    uint64_t temperature_Reg;
    uint64_t deviceReset_Reg;
    uint64_t singleFrameAcquisitionMode_Reg;
    uint64_t acquisitionMode_Reg;
    uint64_t acquisitionStart_Reg;
    uint64_t acquisitionStop_Reg;
    uint64_t colorCodingID_Reg;
    uint32_t pixelFormatCtrlVal_Int_Sel;

} UsbCamDrv_U3V_CamRegLuTable;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/

extern const UsbCamDrv_U3V_CamRegLuTable U3V_CamRegAdrLUT[2];


/********************************************************
* Function declarations
*********************************************************/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_U3V_CAMREGLUT_H
