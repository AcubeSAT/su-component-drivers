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
    /* #0 U3V_FLIR_CM3_U3_12S2C_CS_LUT_ENTRY */
    {
        .CamRegBaseAddress              = 0xFFFFF0F00000ull,
        .SBRMOffset                     = 0x200000ull,
        .ColorCodingID_Reg              = 0x4070ull,
        .DeviceReset_Reg                = 0x400cull,
        .SingleFrameAcquisitionMode_Reg = 0x638ull,
        .AcquisitionMode_Reg            = 0x4028ull,
        .AcquisitionStart_Reg           = 0x4030ull,
        .AcquisitionStop_Reg            = 0x614ull
    },

    /* #1 U3V_XIMEA_XIQ_MQ013CG_E2_LUT_ENTRY */
    {
        .CamRegBaseAddress              = 0x0ull,   //tbd
        .SBRMOffset                     = 0x0ull,   //tbd
        .ColorCodingID_Reg              = 0x0ull,   //tbd
        .DeviceReset_Reg                = 0x0ull,   //tbd
        .SingleFrameAcquisitionMode_Reg = 0x0ull,   //tbd
        .AcquisitionMode_Reg            = 0x0ull,   //tbd
        .AcquisitionStart_Reg           = 0x0ull,   //tbd
        .AcquisitionStop_Reg            = 0x0ull    //tbd
    }
};

