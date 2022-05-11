//
// Created by mojo on 17/02/22.
//

#include "UsbCamDrv_U3V_CamRegLUT.h"



/********************************************************
* Local function declarations
*********************************************************/

/********************************************************
 * Constant & Variable declarations
 *********************************************************/

const UsbCamDrv_U3V_CamRegLuTable U3V_CamRegAdrLUT[2] =
{
    /* #0 FLIR Chameleon3 CM3-U3-13S2C Manifest Entry */
    {
        .CamRegBaseAddress  = 0xFFFFF0F00000,
        .SBRMOffset         = 0x200000,
        .ColorCodingID_Reg  = 0x4070,
        .DeviceReset_Reg    = 0x400c

    },

    /* #1 XIMEA xiQ MQ013CG-E2 Manifest Entry */
    {
        .CamRegBaseAddress  = 0x0,  //tbd
        .SBRMOffset         = 0x0,  //tbd
        .ColorCodingID_Reg  = 0x0,  //tbd
        .DeviceReset_Reg    = 0x0   //tbd

    }
};

