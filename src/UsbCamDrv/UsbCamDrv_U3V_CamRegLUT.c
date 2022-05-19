//
// Created by mojo on 11/05/22.
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
        .camRegBaseAddress              = 0xFFFFF0F00000ull,
        .SBRMOffset                     = 0x200000ull,
        .temperature_Reg                = 0x82Cull,
        .deviceReset_Reg                = 0x400cull,
        .singleFrameAcquisitionMode_Reg = 0x638ull,
        .acquisitionMode_Reg            = 0x4028ull,
        .acquisitionStart_Reg           = 0x4030ull,
        .acquisitionStop_Reg            = 0x614ull,
        .colorCodingID_Reg              = 0x4070ull,
        .pixelFormatCtrlVal_Int_Sel     = 0x04ul          /* U3V_PFNC_RGB8 = 0x02180014 -> 04 (from PixelFormatCtrlVal_Int in Manifest xml)*/
    },

    /* #1 U3V_XIMEA_XIQ_MQ013CG_E2_LUT_ENTRY */
    {
        .camRegBaseAddress              = 0x0ull,   //tbd
        .SBRMOffset                     = 0x0ull,   //tbd
        .temperature_Reg                = 0x0ull,   //tbd
        .deviceReset_Reg                = 0x0ull,   //tbd
        .singleFrameAcquisitionMode_Reg = 0x0ull,   //tbd
        .acquisitionMode_Reg            = 0x0ull,   //tbd
        .acquisitionStart_Reg           = 0x0ull,   //tbd
        .acquisitionStop_Reg            = 0x0ull,   //tbd
        .colorCodingID_Reg              = 0x0ull,   //tbd
        .pixelFormatCtrlVal_Int_Sel     = 0x0ul
    }
};

