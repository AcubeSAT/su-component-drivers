//
// Created by fomarko on 11/05/22.
//

#ifndef COMPONENT_DRIVERS_U3VCAM_REGISTERS_LUT_H
#define COMPONENT_DRIVERS_U3VCAM_REGISTERS_LUT_H


#include <stdint.h>


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
    uint64_t temperature_Reg;                       /* length of val = 4 */
    uint64_t deviceReset_Reg;                       /* length of val = 4 */
    uint64_t singleFrameAcquisitionMode_Reg;        /* length of val = 4 */
    uint64_t acquisitionMode_Reg;                   /* length of val = 4 */
    uint64_t acquisitionStart_Reg;                  /* length of val = 4 */
    uint64_t acquisitionStop_Reg;                   /* length of val = 4 */
    uint64_t colorCodingID_Reg;                     /* length of val = 4 */
    uint64_t payloadSizeVal_Reg;                    /* length of val = 4 */
    uint32_t pixelFormatCtrlVal_Int_Sel;
} T_U3VCamRegisterLUT;


/********************************************************
* Constant declarations
*********************************************************/


/********************************************************
* Variable declarations
*********************************************************/

extern const T_U3VCamRegisterLUT U3VCamRegisterLUT[2U];


/********************************************************
* Function declarations
*********************************************************/



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_REGISTERS_LUT_H
