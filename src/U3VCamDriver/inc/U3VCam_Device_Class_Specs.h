
#ifndef COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H
#define COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

/**
 * USB3 Vision / USB-IF Device class definitions
 * 
 */
#define U3V_DEVICE_CLASS_MISC                   0xEFU
#define U3V_DEVICE_SUBCLASS_COMMON              0x02U
#define U3V_DEVICE_PROTOCOL_IAD                 0x01U
#define U3V_DESCRIPTOR_TYPE_DEVICE              0x01U
#define U3V_DESCRIPTOR_TYPE_CONFIGURATION       0x02U
#define U3V_DESCRIPTOR_TYPE_INTERFACE           0x04U
#define U3V_DESCRIPTOR_TYPE_ENDPOINT            0x05U
#define U3V_DESCRIPTOR_TYPE_IAD                 0x0BU
#define U3V_INTERFACE_U3V_SUBLCASS              0x05U
#define U3V_INTERFACE_CONTROL                   0x00U
#define U3V_INTERFACE_EVENT                     0x01U
#define U3V_INTERFACE_DATASTREAM                0x02U

#define U3V_INFO_IS_U3V_INTERFACE               0x24U
#define U3V_INFO_IS_DEVICEINFO                  0x01U
#define U3V_INFO_MIN_LENGTH_STR                 20U
#define U3V_MAX_DESCR_STR_LENGTH                64U
#define U3V_ERR_NO_ERROR                        0x0000U

/**
 * U3V Magic Key prefixes.
 * 
 * Magic keys are used in the prefix of a packet to identify the packet content. 
 * @note Image data contain no prefix.
 */
typedef enum
{
    U3V_CONTROL_MGK_PREFIX                    = 0x43563355U, /* "U3VC" in ASCII */
    U3V_LEADER_MGK_PREFIX                     = 0x4C563355U, /* "U3VL" in ASCII */
    U3V_TRAILER_MGK_PREFIX                    = 0x54563355U  /* "U3VT" in ASCII */
} T_U3VMagicKeyPrefix;

/**
 * U3V Control Interface flag for request ACK.
 * 
 */
#define U3V_CTRL_REQ_ACK                        0x4000U

/**
 * U3V Control Interface CMD IDs. 
 * 
 */
typedef enum
{
    U3V_CTRL_READMEM_CMD                      = 0x0800U,
    U3V_CTRL_READMEM_ACK                      = 0x0801U,
    U3V_CTRL_WRITEMEM_CMD                     = 0x0802U,
    U3V_CTRL_WRITEMEM_ACK                     = 0x0803U,
    U3V_CTRL_PENDING_ACK                      = 0x0805U
} T_U3VCtrlIfCmdId;

/**
 * U3V / GenCP Technology Agnostic Bootstrap Register Map (ABRM)
 * 
 * Register map for the ABRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_ABRM_GENCP_VERSION_OFS                = 0x00000U,
    U3V_ABRM_MANUFACTURER_NAME_OFS            = 0x00004U,
    U3V_ABRM_MODEL_NAME_OFS                   = 0x00044U,
    U3V_ABRM_FAMILY_NAME_OFS                  = 0x00084U,
    U3V_ABRM_DEVICE_VERSION_OFS               = 0x000C4U,
    U3V_ABRM_MANUFACTURER_INFO_OFS            = 0x00104U,
    U3V_ABRM_SERIAL_NUMBER_OFS                = 0x00144U,
    U3V_ABRM_USER_DEFINED_NAME_OFS            = 0x00184U,
    U3V_ABRM_DEVICE_CAPABILITY_OFS            = 0x001C4U,
    U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS     = 0x001CCU,
    U3V_ABRM_MANIFEST_TABLE_ADDRESS_OFS       = 0x001D0U,
    U3V_ABRM_SBRM_ADDRESS_OFS                 = 0x001D8U,
    U3V_ABRM_DEVICE_CONFIGURATION_OFS         = 0x001E0U,
    U3V_ABRM_HEARTBEAT_TIMEOUT_OFS            = 0x001E8U,
    U3V_ABRM_MESSAGE_CHANNEL_ID_OFS           = 0x001ECU,
    U3V_ABRM_TIMESTAMP_OFS                    = 0x001F0U,
    U3V_ABRM_TIMESTAMP_LATCH_OFS              = 0x001F8U,
    U3V_ABRM_TIMESTAMP_INCREMENT_OFS          = 0x001FCU,
    U3V_ABRM_ACCESS_PRIVILEGE_OFS             = 0x00204U,
    U3V_ABRM_RESERVED_DPRCTD_AREA_OFS         = 0x00208U,
    U3V_ABRM_IMPLEMENTATION_ENDIANESS_OFS     = 0x0020CU,
    U3V_ABRM_RESERVED_SPACE_OFS               = 0x00210U
} T_U3VAbrmOffset;

/* U3V / GenCP Register size of ABRM space */
#define U3V_REG_GENCP_VERSION_SIZE              4U
#define U3V_REG_MANUFACTURER_NAME_SIZE          64U
#define U3V_REG_MODEL_NAME_SIZE                 64U
#define U3V_REG_FAMILY_NAME_SIZE                64U
#define U3V_REG_DEVICE_VERSION_SIZE             64U
#define U3V_REG_MANUFACTURER_INFO_SIZE          64U
#define U3V_REG_SERIAL_NUMBER_SIZE              64U
#define U3V_REG_USER_DEFINED_NAME_SIZE          64U
#define U3V_REG_DEVICE_CAPABILITY_SIZE          8U
#define U3V_REG_MAX_DEV_RESPONSE_TIME_MS_SIZE   4U
#define U3V_REG_MANIFEST_TABLE_ADDRESS_SIZE     8U
#define U3V_REG_SBRM_ADDRESS_SIZE               8U
#define U3V_REG_DEVICE_CONFIGURATION_SIZE       8U
#define U3V_REG_HEARTBEAT_TIMEOUT_SIZE          4U
#define U3V_REG_MESSAGE_CHANNEL_ID_SIZE         4U
#define U3V_REG_TIMESTAMP_SIZE                  8U
#define U3V_REG_TIMESTAMP_LATCH_SIZE            4U
#define U3V_REG_TIMESTAMP_INCREMENT_SIZE        8U
#define U3V_REG_ACCESS_PRIVILEGE_SIZE           4U
#define U3V_REG_RESERVED_DPRCTD_AREA_SIZE       4U
#define U3V_REG_IMPLEMENTATION_ENDIANESS_SIZE   4U
#define U3V_REG_RESERVED_SPACE_SIZE             65008U

/**
 * U3V / GenCP Technology Specific Bootstrap Register Map (SBRM) 
 * 
 * Register map for the SBRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_SBRM_U3V_VERSION_OFS		          = 0x00000U,
    U3V_SBRM_U3VCP_CAPABILITY_OFS	          = 0x00004U,
    U3V_SBRM_U3VCP_CONFIGURATION_OFS          = 0x0000CU,
    U3V_SBRM_MAX_CMD_TRANSFER_OFS	          = 0x00014U,
    U3V_SBRM_MAX_ACK_TRANSFER_OFS	          = 0x00018U,
    U3V_SBRM_NUM_STREAM_CHANNELS_OFS          = 0x0001CU,
    U3V_SBRM_SIRM_ADDRESS_OFS		          = 0x00020U,
    U3V_SBRM_SIRM_LENGTH_OFS		          = 0x00028U,
    U3V_SBRM_EIRM_ADDRESS_OFS		          = 0x0002CU,
    U3V_SBRM_EIRM_LENGTH_OFS		          = 0x00034U,
    U3V_SBRM_IIDC2_ADDRESS_OFS		          = 0x00038U,
    U3V_SBRM_CURRENT_SPEED_OFS		          = 0x00040U,
    U3V_SBRM_RESERVED_OFS			          = 0x00044U
} T_U3VSbrmOffset;

/**
 * U3V / GenCP Technology Streaming Interface Register Map (SIRM) 
 * 
 * Register map for the SIRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_SIRM_INFO_OFS			              = 0x00U,
    U3V_SIRM_CONTROL_OFS		              = 0x04U,
    U3V_SIRM_REQ_PAYLOAD_SIZE_OFS	          = 0x08U,
    U3V_SIRM_REQ_LEADER_SIZE_OFS	          = 0x10U,
    U3V_SIRM_REQ_TRAILER_SIZE_OFS	          = 0x14U,
    U3V_SIRM_MAX_LEADER_SIZE_OFS	          = 0x18U,
    U3V_SIRM_PAYLOAD_SIZE_OFS		          = 0x1CU,
    U3V_SIRM_PAYLOAD_COUNT_OFS	              = 0x20U,
    U3V_SIRM_TRANSFER1_SIZE_OFS	              = 0x24U,
    U3V_SIRM_TRANSFER2_SIZE_OFS	              = 0x28U,
    U3V_SIRM_MAX_TRAILER_SIZE_OFS	          = 0x2CU
} T_U3VSirmOffset;

/* USB3 Vision / GenCP - Technology Streaming Interface - Other */
#define U3V_SIRM_AVAILABLE_MASK		            0x00000001U
#define U3V_SIRM_INFO_ALIGNMENT_MASK	        0xFF000000U
#define U3V_SIRM_INFO_ALIGNMENT_SHIFT	        0x18U

/**
 * U3V Pixel Format Naming Convention.
 * 
 * Naming convention used for pixel format selection as speficied by the 
 * USB3 Vision / GenCP standard.
 */
typedef enum
{
    U3V_PFNC_Mono1p                           = 0x01010037U, /* Monochrome 1-bit packed */
    U3V_PFNC_Mono2p                           = 0x01020038U, /* Monochrome 2-bit packed */
    U3V_PFNC_Mono4p                           = 0x01040039U, /* Monochrome 4-bit packed */
    U3V_PFNC_Mono8                            = 0x01080001U, /* Monochrome 8-bit */
    U3V_PFNC_Mono8s                           = 0x01080002U, /* Monochrome 8-bit signed */
    U3V_PFNC_Mono10                           = 0x01100003U, /* Monochrome 10-bit unpacked */
    U3V_PFNC_Mono10p                          = 0x010A0046U, /* Monochrome 10-bit packed */
    U3V_PFNC_Mono12                           = 0x01100005U, /* Monochrome 12-bit unpacked */
    U3V_PFNC_Mono12p                          = 0x010C0047U, /* Monochrome 12-bit packed */
    U3V_PFNC_Mono14                           = 0x01100025U, /* Monochrome 14-bit unpacked */
    U3V_PFNC_Mono14p                          = 0x010E0104U, /* Monochrome 14-bit packed */
    U3V_PFNC_Mono16                           = 0x01100007U, /* Monochrome 16-bit */
    U3V_PFNC_Mono32                           = 0x01200111U, /* Monochrome 32-bit */
    U3V_PFNC_BayerBG4p                        = 0x01040110U, /* Bayer Blue-Green 4-bit packed */
    U3V_PFNC_BayerBG8                         = 0x0108000BU, /* Bayer Blue-Green 8-bit */
    U3V_PFNC_BayerBG10                        = 0x0110000FU, /* Bayer Blue-Green 10-bit unpacked */
    U3V_PFNC_BayerBG10p                       = 0x010A0052U, /* Bayer Blue-Green 10-bit packed */
    U3V_PFNC_BayerBG12                        = 0x01100013U, /* Bayer Blue-Green 12-bit unpacked */
    U3V_PFNC_BayerBG12p                       = 0x010C0053U, /* Bayer Blue-Green 12-bit packed */
    U3V_PFNC_BayerBG14                        = 0x0110010CU, /* Bayer Blue-Green 14-bit */
    U3V_PFNC_BayerBG14p                       = 0x010E0108U, /* Bayer Blue-Green 14-bit packed */
    U3V_PFNC_BayerBG16                        = 0x01100031U, /* Bayer Blue-Green 16-bit */
    U3V_PFNC_BayerGB4p                        = 0x0104010FU, /* Bayer Green-Blue 4-bit packed */
    U3V_PFNC_BayerGB8                         = 0x0108000AU, /* Bayer Green-Blue 8-bit */
    U3V_PFNC_BayerGB10                        = 0x0110000EU, /* Bayer Green-Blue 10-bit unpacked */
    U3V_PFNC_BayerGB10p                       = 0x010A0054U, /* Bayer Green-Blue 10-bit packed */
    U3V_PFNC_BayerGB12                        = 0x01100012U, /* Bayer Green-Blue 12-bit unpacked */
    U3V_PFNC_BayerGB12p                       = 0x010C0055U, /* Bayer Green-Blue 12-bit packed */
    U3V_PFNC_BayerGB14                        = 0x0110010BU, /* Bayer Green-Blue 14-bit */
    U3V_PFNC_BayerGB14p                       = 0x010E0107U, /* Bayer Green-Blue 14-bit packed */
    U3V_PFNC_BayerGB16                        = 0x01100030U, /* Bayer Green-Blue 16-bit */
    U3V_PFNC_BayerGR4p                        = 0x0104010DU, /* Bayer Green-Red 4-bit packed */
    U3V_PFNC_BayerGR8                         = 0x01080008U, /* Bayer Green-Red 8-bit */
    U3V_PFNC_BayerGR10                        = 0x0110000CU, /* Bayer Green-Red 10-bit unpacked */
    U3V_PFNC_BayerGR10p                       = 0x010A0056U, /* Bayer Green-Red 10-bit packed */
    U3V_PFNC_BayerGR12                        = 0x01100010U, /* Bayer Green-Red 12-bit unpacked */
    U3V_PFNC_BayerGR12p                       = 0x010C0057U, /* Bayer Green-Red 12-bit packed */
    U3V_PFNC_BayerGR14                        = 0x01100109U, /* Bayer Green-Red 14-bit */
    U3V_PFNC_BayerGR14p                       = 0x010E0105U, /* Bayer Green-Red 14-bit packed */
    U3V_PFNC_BayerGR16                        = 0x0110002EU, /* Bayer Green-Red 16-bit */
    U3V_PFNC_BayerRG4p                        = 0x0104010EU, /* Bayer Red-Green 4-bit packed */
    U3V_PFNC_BayerRG8                         = 0x01080009U, /* Bayer Red-Green 8-bit */
    U3V_PFNC_BayerRG10                        = 0x0110000DU, /* Bayer Red-Green 10-bit unpacked */
    U3V_PFNC_BayerRG10p                       = 0x010A0058U, /* Bayer Red-Green 10-bit packed */
    U3V_PFNC_BayerRG12                        = 0x01100011U, /* Bayer Red-Green 12-bit unpacked */
    U3V_PFNC_BayerRG12p                       = 0x010C0059U, /* Bayer Red-Green 12-bit packed */
    U3V_PFNC_BayerRG14                        = 0x0110010AU, /* Bayer Red-Green 14-bit */
    U3V_PFNC_BayerRG14p                       = 0x010E0106U, /* Bayer Red-Green 14-bit packed */
    U3V_PFNC_BayerRG16                        = 0x0110002FU, /* Bayer Red-Green 16-bit */
    U3V_PFNC_RGBa8                            = 0x02200016U, /* Red-Green-Blue-alpha 8-bit */
    U3V_PFNC_RGBa10                           = 0x0240005FU, /* Red-Green-Blue-alpha 10-bit unpacked */
    U3V_PFNC_RGBa10p                          = 0x02280060U, /* Red-Green-Blue-alpha 10-bit packed */
    U3V_PFNC_RGBa12                           = 0x02400061U, /* Red-Green-Blue-alpha 12-bit unpacked */
    U3V_PFNC_RGBa12p                          = 0x02300062U, /* Red-Green-Blue-alpha 12-bit packed */
    U3V_PFNC_RGBa14                           = 0x02400063U, /* Red-Green-Blue-alpha 14-bit unpacked */
    U3V_PFNC_RGBa16                           = 0x02400064U, /* Red-Green-Blue-alpha 16-bit */
    U3V_PFNC_RGB8                             = 0x02180014U, /* Red-Green-Blue 8-bit */
    U3V_PFNC_RGB8_Planar                      = 0x02180021U, /* Red-Green-Blue 8-bit planar */
    U3V_PFNC_RGB10                            = 0x02300018U, /* Red-Green-Blue 10-bit unpacked */
    U3V_PFNC_RGB10_Planar                     = 0x02300022U, /* Red-Green-Blue 10-bit unpacked planar */
    U3V_PFNC_RGB10p                           = 0x021E005CU, /* Red-Green-Blue 10-bit packed */
    U3V_PFNC_RGB10p32                         = 0x0220001DU, /* Red-Green-Blue 10-bit packed into 32-bit */
    U3V_PFNC_RGB12                            = 0x0230001AU, /* Red-Green-Blue 12-bit unpacked */
    U3V_PFNC_RGB12_Planar                     = 0x02300023U, /* Red-Green-Blue 12-bit unpacked planar */
    U3V_PFNC_RGB12p                           = 0x0224005DU, /* Red-Green-Blue 12-bit packed */
    U3V_PFNC_RGB14                            = 0x0230005EU, /* Red-Green-Blue 14-bit unpacked */
    U3V_PFNC_RGB16                            = 0x02300033U, /* Red-Green-Blue 16-bit */
    U3V_PFNC_RGB16_Planar                     = 0x02300024U, /* Red-Green-Blue 16-bit planar */
    U3V_PFNC_RGB565p                          = 0x02100035U, /* Red-Green-Blue 5/6/5-bit packed */
    U3V_PFNC_BGRa8                            = 0x02200017U, /* Blue-Green-Red-alpha 8-bit */
    U3V_PFNC_BGRa10                           = 0x0240004CU, /* Blue-Green-Red-alpha 10-bit unpacked */
    U3V_PFNC_BGRa10p                          = 0x0228004DU, /* Blue-Green-Red-alpha 10-bit packed */
    U3V_PFNC_BGRa12                           = 0x0240004EU, /* Blue-Green-Red-alpha 12-bit unpacked */
    U3V_PFNC_BGRa12p                          = 0x0230004FU, /* Blue-Green-Red-alpha 12-bit packed */
    U3V_PFNC_BGRa14                           = 0x02400050U, /* Blue-Green-Red-alpha 14-bit unpacked */
    U3V_PFNC_BGRa16                           = 0x02400051U, /* Blue-Green-Red-alpha 16-bit */
    U3V_PFNC_BGR8                             = 0x02180015U, /* Blue-Green-Red 8-bit */
    U3V_PFNC_BGR10                            = 0x02300019U, /* Blue-Green-Red 10-bit unpacked */
    U3V_PFNC_BGR10p                           = 0x021E0048U, /* Blue-Green-Red 10-bit packed */
    U3V_PFNC_BGR12                            = 0x0230001BU, /* Blue-Green-Red 12-bit unpacked */
    U3V_PFNC_BGR12p                           = 0x02240049U, /* Blue-Green-Red 12-bit packed */
    U3V_PFNC_BGR14                            = 0x0230004AU, /* Blue-Green-Red 14-bit unpacked */
    U3V_PFNC_BGR16                            = 0x0230004BU, /* Blue-Green-Red 16-bit */
    U3V_PFNC_BGR565p                          = 0x02100036U, /* Blue-Green-Red 5/6/5-bit packed */
    U3V_PFNC_R8                               = 0x010800C9U, /* Red 8-bit */
    U3V_PFNC_R10                              = 0x01100120U, /* Red 10-bit */
    U3V_PFNC_R10_Deprecated                   = 0x010A00CAU, /* Deprecated because size field is wrong */
    U3V_PFNC_R12                              = 0x01100121U, /* Red 12-bit */
    U3V_PFNC_R12_Deprecated                   = 0x010C00CBU, /* Deprecated because size field is wrong */
    U3V_PFNC_R16                              = 0x011000CCU, /* Red 16-bit */
    U3V_PFNC_G8                               = 0x010800CDU, /* Green 8-bit */
    U3V_PFNC_G10                              = 0x01100122U, /* Green 10-bit */
    U3V_PFNC_G10_Deprecated                   = 0x010A00CEU, /* Deprecated because size field is wrong */
    U3V_PFNC_G12                              = 0x01100123U, /* Green 12-bit */
    U3V_PFNC_G12_Deprecated                   = 0x010C00CFU, /* Deprecated because size field is wrong */
    U3V_PFNC_G16                              = 0x011000D0U, /* Green 16-bit */
    U3V_PFNC_B8                               = 0x010800D1U, /* Blue 8-bit */
    U3V_PFNC_B10                              = 0x01100124U, /* Blue 10-bit */
    U3V_PFNC_B10_Deprecated                   = 0x010A00D2U, /* Deprecated because size field is wrong */
    U3V_PFNC_B12                              = 0x01100125U, /* Blue 12-bit */
    U3V_PFNC_B12_Deprecated                   = 0x010C00D3U, /* Deprecated because size field is wrong */
    U3V_PFNC_B16                              = 0x011000D4U, /* Blue 16-bit */
    U3V_PFNC_BiColorBGRG8                     = 0x021000A6U, /* Bi-color Blue/Green - Red/Green 8-bit */
    U3V_PFNC_BiColorBGRG10                    = 0x022000A9U, /* Bi-color Blue/Green - Red/Green 10-bit unpacked */
    U3V_PFNC_BiColorBGRG10p                   = 0x021400AAU, /* Bi-color Blue/Green - Red/Green 10-bit packed */
    U3V_PFNC_BiColorBGRG12                    = 0x022000ADU, /* Bi-color Blue/Green - Red/Green 12-bit unpacked */
    U3V_PFNC_BiColorBGRG12p                   = 0x021800AEU, /* Bi-color Blue/Green - Red/Green 12-bit packed */
    U3V_PFNC_BiColorRGBG8                     = 0x021000A5U, /* Bi-color Red/Green - Blue/Green 8-bit */
    U3V_PFNC_BiColorRGBG10                    = 0x022000A7U, /* Bi-color Red/Green - Blue/Green 10-bit unpacked */
    U3V_PFNC_BiColorRGBG10p                   = 0x021400A8U, /* Bi-color Red/Green - Blue/Green 10-bit packed */
    U3V_PFNC_BiColorRGBG12                    = 0x022000ABU, /* Bi-color Red/Green - Blue/Green 12-bit unpacked */
    U3V_PFNC_BiColorRGBG12p                   = 0x021800ACU, /* Bi-color Red/Green - Blue/Green 12-bit packed */
    U3V_PFNC_Data8                            = 0x01080116U, /* Data 8-bit */
    U3V_PFNC_Data8s                           = 0x01080117U, /* Data 8-bit signed */
    U3V_PFNC_Data16                           = 0x01100118U, /* Data 16-bit */
    U3V_PFNC_Data16s                          = 0x01100119U, /* Data 16-bit signed */
    U3V_PFNC_Data32                           = 0x0120011AU, /* Data 32-bit */
    U3V_PFNC_Data32f                          = 0x0120011CU, /* Data 32-bit floating point */
    U3V_PFNC_Data32s                          = 0x0120011BU, /* Data 32-bit signed */
    U3V_PFNC_Data64                           = 0x0140011DU, /* Data 64-bit */
    U3V_PFNC_Data64f                          = 0x0140011FU, /* Data 64-bit floating point */
    U3V_PFNC_Data64s                          = 0x0140011EU, /* Data 64-bit signed */
    U3V_PFNC_YCbCr8                           = 0x0218005BU, /* YCbCr 4:4:4 8-bit */
    U3V_PFNC_YCbCr8_CbYCr                     = 0x0218003AU, /* YCbCr 4:4:4 8-bit */
    U3V_PFNC_YCbCr10_CbYCr                    = 0x02300083U, /* YCbCr 4:4:4 10-bit unpacked */
    U3V_PFNC_YCbCr10p_CbYCr                   = 0x021E0084U, /* YCbCr 4:4:4 10-bit packed */
    U3V_PFNC_YCbCr12_CbYCr                    = 0x02300085U, /* YCbCr 4:4:4 12-bit unpacked */
    U3V_PFNC_YCbCr12p_CbYCr                   = 0x02240086U, /* YCbCr 4:4:4 12-bit packed */
    U3V_PFNC_YCbCr411_8                       = 0x020C005AU, /* YCbCr 4:1:1 8-bit */
    U3V_PFNC_YCbCr411_8_CbYYCrYY              = 0x020C003CU, /* YCbCr 4:1:1 8-bit */
    U3V_PFNC_YCbCr420_8_YY_CbCr_Semiplanar    = 0x020C0112U, /* YCbCr 4:2:0 8-bit YY/CbCr Semiplanar */
    U3V_PFNC_YCbCr420_8_YY_CrCb_Semiplanar    = 0x020C0114U, /* YCbCr 4:2:0 8-bit YY/CrCb Semiplanar */
    U3V_PFNC_YCbCr422_8                       = 0x0210003BU, /* YCbCr 4:2:2 8-bit */
    U3V_PFNC_YCbCr422_8_CbYCrY                = 0x02100043U, /* YCbCr 4:2:2 8-bit */
    U3V_PFNC_YCbCr422_8_YY_CbCr_Semiplanar    = 0x02100113U, /* YCbCr 4:2:2 8-bit YY/CbCr Semiplanar */
    U3V_PFNC_YCbCr422_8_YY_CrCb_Semiplanar    = 0x02100115U, /* YCbCr 4:2:2 8-bit YY/CrCb Semiplanar */
    U3V_PFNC_YCbCr422_10                      = 0x02200065U, /* YCbCr 4:2:2 10-bit unpacked */
    U3V_PFNC_YCbCr422_10_CbYCrY               = 0x02200099U, /* YCbCr 4:2:2 10-bit unpacked */
    U3V_PFNC_YCbCr422_10p                     = 0x02140087U, /* YCbCr 4:2:2 10-bit packed */
    U3V_PFNC_YCbCr422_10p_CbYCrY              = 0x0214009AU, /* YCbCr 4:2:2 10-bit packed */
    U3V_PFNC_YCbCr422_12                      = 0x02200066U, /* YCbCr 4:2:2 12-bit unpacked */
    U3V_PFNC_YCbCr422_12_CbYCrY               = 0x0220009BU, /* YCbCr 4:2:2 12-bit unpacked */
    U3V_PFNC_YCbCr422_12p                     = 0x02180088U, /* YCbCr 4:2:2 12-bit packed */
    U3V_PFNC_YCbCr422_12p_CbYCrY              = 0x0218009CU, /* YCbCr 4:2:2 12-bit packed */
    U3V_PFNC_YCbCr601_8_CbYCr                 = 0x0218003DU, /* YCbCr 4:4:4 8-bit BT.601 */
    U3V_PFNC_YCbCr601_10_CbYCr                = 0x02300089U, /* YCbCr 4:4:4 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_10p_CbYCr               = 0x021E008AU, /* YCbCr 4:4:4 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_12_CbYCr                = 0x0230008BU, /* YCbCr 4:4:4 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_12p_CbYCr               = 0x0224008CU, /* YCbCr 4:4:4 12-bit packed BT.601 */
    U3V_PFNC_YCbCr601_411_8_CbYYCrYY          = 0x020C003FU, /* YCbCr 4:1:1 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_8                   = 0x0210003EU, /* YCbCr 4:2:2 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_8_CbYCrY            = 0x02100044U, /* YCbCr 4:2:2 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_10                  = 0x0220008DU, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_10_CbYCrY           = 0x0220009DU, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_10p                 = 0x0214008EU, /* YCbCr 4:2:2 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_10p_CbYCrY          = 0x0214009EU, /* YCbCr 4:2:2 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_12                  = 0x0220008FU, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_12_CbYCrY           = 0x0220009FU, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_12p                 = 0x02180090U, /* YCbCr 4:2:2 12-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_12p_CbYCrY          = 0x021800A0U, /* YCbCr 4:2:2 12-bit packed BT.601 */
    U3V_PFNC_YCbCr709_8_CbYCr                 = 0x02180040U, /* YCbCr 4:4:4 8-bit BT.709 */
    U3V_PFNC_YCbCr709_10_CbYCr                = 0x02300091U, /* YCbCr 4:4:4 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_10p_CbYCr               = 0x021E0092U, /* YCbCr 4:4:4 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_12_CbYCr                = 0x02300093U, /* YCbCr 4:4:4 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_12p_CbYCr               = 0x02240094U, /* YCbCr 4:4:4 12-bit packed BT.709 */
    U3V_PFNC_YCbCr709_411_8_CbYYCrYY          = 0x020C0042U, /* YCbCr 4:1:1 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_8                   = 0x02100041U, /* YCbCr 4:2:2 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_8_CbYCrY            = 0x02100045U, /* YCbCr 4:2:2 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_10                  = 0x02200095U, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_10_CbYCrY           = 0x022000A1U, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_10p                 = 0x02140096U, /* YCbCr 4:2:2 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_10p_CbYCrY          = 0x021400A2U, /* YCbCr 4:2:2 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_12                  = 0x02200097U, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_12_CbYCrY           = 0x022000A3U, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_12p                 = 0x02180098U, /* YCbCr 4:2:2 12-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_12p_CbYCrY          = 0x021800A4U, /* YCbCr 4:2:2 12-bit packed BT.709 */
    U3V_PFNC_YCbCr2020_8_CbYCr                = 0x021800F4U, /* YCbCr 4:4:4 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_10_CbYCr               = 0x023000F5U, /* YCbCr 4:4:4 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_10p_CbYCr              = 0x021E00F6U, /* YCbCr 4:4:4 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_12_CbYCr               = 0x023000F7U, /* YCbCr 4:4:4 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_12p_CbYCr              = 0x022400F8U, /* YCbCr 4:4:4 12-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_411_8_CbYYCrYY         = 0x020C00F9U, /* YCbCr 4:1:1 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_8                  = 0x021000FAU, /* YCbCr 4:2:2 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_8_CbYCrY           = 0x021000FBU, /* YCbCr 4:2:2 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_10                 = 0x022000FCU, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_10_CbYCrY          = 0x022000FDU, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_10p                = 0x021400FEU, /* YCbCr 4:2:2 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_10p_CbYCrY         = 0x021400FFU, /* YCbCr 4:2:2 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_12                 = 0x02200100U, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_12_CbYCrY          = 0x02200101U, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_12p                = 0x02180102U, /* YCbCr 4:2:2 12-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_12p_CbYCrY         = 0x02180103U, /* YCbCr 4:2:2 12-bit packed BT.2020 */
    U3V_PFNC_YUV8_UYV                         = 0x02180020U, /* YUV 4:4:4 8-bit */
    U3V_PFNC_YUV411_8_UYYVYY                  = 0x020C001EU, /* YUV 4:1:1 8-bit */
    U3V_PFNC_YUV422_8                         = 0x02100032U, /* YUV 4:2:2 8-bit */
    U3V_PFNC_YUV422_8_UYVY                    = 0x0210001FU, /* YUV 4:2:2 8-bit */
    U3V_PFNC_InvalidPixelFormat               = 0x00000000U
} T_U3VPfnc;

/**
 * U3V Acquisition control command value.
 * 
 */
typedef enum
{
    U3V_ACQ_START_CMD_VAL                     = 0x01U,
    U3V_ACQ_STOP_CMD_VAL                      = 0x00U
} T_U3VAcqCmdVal;

/**
 * U3V Stream control command value.
 * 
 */
typedef enum
{
    U3V_SI_CTRL_CMD_ENABLE                    = 0x01U,
    U3V_SI_CTRL_CMD_DISABLE                   = 0x00U
} T_U3VSiCtrlCmd;

/**
 * U3V Stream payload type identifier.
 * 
 */
typedef enum
{
    U3V_STREAM_PLD_TYPE_IMAGE                 = 0x0001U,
    U3V_STREAM_PLD_TYPE_IMAGE_EXT_CHUNK       = 0x4001U,
    U3V_STREAM_PLD_TYPE_CHUNK                 = 0x4000U
} T_U3VStreamPayloadType;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H
