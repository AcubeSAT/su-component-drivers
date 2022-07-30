//
// Created by fomarko on 11/05/22.
//

#include "U3VCam_Config.h"



/********************************************************
 * Constant & Variable declarations
 *********************************************************/

const T_U3VCamRegisterCfg U3VCamRegisterCfgTable =
{
#if (U3V_CAM_MODEL_SELECTED == U3V_FLIR_CM3_U3_12S2C_CS)
    .camRegBaseAddress              = 0xFFFFF0F00000ULL,
    .SBRMOffset                     = 0x200000ULL,
    .temperature_Reg                = 0x82CULL,
    .deviceReset_Reg                = 0x400cULL,
    .singleFrameAcquisitionMode_Reg = 0x638ULL,
    .acquisitionMode_Reg            = 0x4028ULL,
    .acquisitionStart_Reg           = 0x4030ULL,
    .acquisitionStop_Reg            = 0x614ULL,
    .colorCodingID_Reg              = 0x4070ULL,
    .payloadSizeVal_Reg             = 0x5410ULL,
    .pixelFormatCtrlVal_Int_Sel     = 0x04UL          /* U3V_PFNC_RGB8 = 0x02180014 -> 04 (from PixelFormatCtrlVal_Int in Manifest xml)*/
#elif (U3V_CAM_MODEL_SELECTED == U3V_XIMEA_XIQ_MQ013CG_E2)
    .camRegBaseAddress              = 0x0ULL,   //TODO: add
    .SBRMOffset                     = 0x0ULL,   //TODO: add
    .temperature_Reg                = 0x0ULL,   //TODO: add
    .deviceReset_Reg                = 0x0ULL,   //TODO: add
    .singleFrameAcquisitionMode_Reg = 0x0ULL,   //TODO: add
    .acquisitionMode_Reg            = 0x0ULL,   //TODO: add
    .acquisitionStart_Reg           = 0x0ULL,   //TODO: add
    .acquisitionStop_Reg            = 0x0ULL,   //TODO: add
    .colorCodingID_Reg              = 0x0ULL,   //TODO: add
    .payloadSizeVal_Reg             = 0x0ULL,   //TODO: add
    .pixelFormatCtrlVal_Int_Sel     = 0x0UL     //TODO: add
#else
    #error "Not valid USB3 Vision camera model configured"
#endif
};

