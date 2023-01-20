#pragma once

#ifdef __cplusplus
extern "C" {
#endif


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
#define U3V_ERR_NO_ERROR                        0x00U

/**
 * U3V Magic Key prefixes.
 * 
 * Magic keys are used in the prefix of a packet to identify the packet content. 
 * @note Image data contain no prefix.
 */
typedef enum
{
    U3V_CONTROL_MGK_PREFIX                    = 0x43563355, /* "U3VC" in ASCII */
    U3V_LEADER_MGK_PREFIX                     = 0x4C563355, /* "U3VL" in ASCII */
    U3V_TRAILER_MGK_PREFIX                    = 0x54563355  /* "U3VT" in ASCII */
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
    U3V_CTRL_READMEM_CMD                      = 0x0800,
    U3V_CTRL_READMEM_ACK                      = 0x0801,
    U3V_CTRL_WRITEMEM_CMD                     = 0x0802,
    U3V_CTRL_WRITEMEM_ACK                     = 0x0803,
    U3V_CTRL_PENDING_ACK                      = 0x0805
} T_U3VCtrlIfCmdId;

/**
 * U3V / GenCP Technology Agnostic Bootstrap Register Map (ABRM)
 * 
 * Register map for the ABRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_ABRM_GENCP_VERSION_OFS                = 0x0000,
    U3V_ABRM_MANUFACTURER_NAME_OFS            = 0x0004,
    U3V_ABRM_MODEL_NAME_OFS                   = 0x0044,
    U3V_ABRM_FAMILY_NAME_OFS                  = 0x0084,
    U3V_ABRM_DEVICE_VERSION_OFS               = 0x00C4,
    U3V_ABRM_MANUFACTURER_INFO_OFS            = 0x0104,
    U3V_ABRM_SERIAL_NUMBER_OFS                = 0x0144,
    U3V_ABRM_USER_DEFINED_NAME_OFS            = 0x0184,
    U3V_ABRM_DEVICE_CAPABILITY_OFS            = 0x01C4,
    U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS     = 0x01CC,
    U3V_ABRM_MANIFEST_TABLE_ADDRESS_OFS       = 0x01D0,
    U3V_ABRM_SBRM_ADDRESS_OFS                 = 0x01D8,
    U3V_ABRM_DEVICE_CONFIGURATION_OFS         = 0x01E0,
    U3V_ABRM_HEARTBEAT_TIMEOUT_OFS            = 0x01E8,
    U3V_ABRM_MESSAGE_CHANNEL_ID_OFS           = 0x01EC,
    U3V_ABRM_TIMESTAMP_OFS                    = 0x01F0,
    U3V_ABRM_TIMESTAMP_LATCH_OFS              = 0x01F8,
    U3V_ABRM_TIMESTAMP_INCREMENT_OFS          = 0x01FC,
    U3V_ABRM_ACCESS_PRIVILEGE_OFS             = 0x0204,
    U3V_ABRM_RESERVED_DPRCTD_AREA_OFS         = 0x0208,
    U3V_ABRM_IMPLEMENTATION_ENDIANESS_OFS     = 0x020C,
    U3V_ABRM_RESERVED_SPACE_OFS               = 0x0210
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
    U3V_SBRM_U3V_VERSION_OFS		          = 0x00,
    U3V_SBRM_U3VCP_CAPABILITY_OFS	          = 0x04,
    U3V_SBRM_U3VCP_CONFIGURATION_OFS          = 0x0C,
    U3V_SBRM_MAX_CMD_TRANSFER_OFS	          = 0x14,
    U3V_SBRM_MAX_ACK_TRANSFER_OFS	          = 0x18,
    U3V_SBRM_NUM_STREAM_CHANNELS_OFS          = 0x1C,
    U3V_SBRM_SIRM_ADDRESS_OFS		          = 0x20,
    U3V_SBRM_SIRM_LENGTH_OFS		          = 0x28,
    U3V_SBRM_EIRM_ADDRESS_OFS		          = 0x2C,
    U3V_SBRM_EIRM_LENGTH_OFS		          = 0x34,
    U3V_SBRM_IIDC2_ADDRESS_OFS		          = 0x38,
    U3V_SBRM_CURRENT_SPEED_OFS		          = 0x40,
    U3V_SBRM_RESERVED_OFS			          = 0x44
} T_U3VSbrmOffset;

/**
 * U3V / GenCP Technology Streaming Interface Register Map (SIRM) 
 * 
 * Register map for the SIRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_SIRM_INFO_OFS			              = 0x00,
    U3V_SIRM_CONTROL_OFS		              = 0x04,
    U3V_SIRM_REQ_PAYLOAD_SIZE_OFS	          = 0x08,
    U3V_SIRM_REQ_LEADER_SIZE_OFS	          = 0x10,
    U3V_SIRM_REQ_TRAILER_SIZE_OFS	          = 0x14,
    U3V_SIRM_MAX_LEADER_SIZE_OFS	          = 0x18,
    U3V_SIRM_PAYLOAD_SIZE_OFS		          = 0x1C,
    U3V_SIRM_PAYLOAD_COUNT_OFS	              = 0x20,
    U3V_SIRM_TRANSFER1_SIZE_OFS	              = 0x24,
    U3V_SIRM_TRANSFER2_SIZE_OFS	              = 0x28,
    U3V_SIRM_MAX_TRAILER_SIZE_OFS	          = 0x2C
} T_U3VSirmOffset;

/* USB3 Vision / GenCP - Technology Streaming Interface - Other */
#define U3V_SIRM_AVAILABLE_MASK		            0x00000001
#define U3V_SIRM_INFO_ALIGNMENT_MASK	        0xFF000000
#define U3V_SIRM_INFO_ALIGNMENT_SHIFT	        0x18

/**
 * U3V Pixel Format Naming Convention.
 * 
 * Naming convention used for pixel format selection as speficied by the 
 * USB3 Vision / GenCP standard.
 */
typedef enum
{
    U3V_PFNC_Mono1p                           = 0x01010037, /* Monochrome 1-bit packed */
    U3V_PFNC_Mono2p                           = 0x01020038, /* Monochrome 2-bit packed */
    U3V_PFNC_Mono4p                           = 0x01040039, /* Monochrome 4-bit packed */
    U3V_PFNC_Mono8                            = 0x01080001, /* Monochrome 8-bit */
    U3V_PFNC_Mono8s                           = 0x01080002, /* Monochrome 8-bit signed */
    U3V_PFNC_Mono10                           = 0x01100003, /* Monochrome 10-bit unpacked */
    U3V_PFNC_Mono10p                          = 0x010A0046, /* Monochrome 10-bit packed */
    U3V_PFNC_Mono12                           = 0x01100005, /* Monochrome 12-bit unpacked */
    U3V_PFNC_Mono12p                          = 0x010C0047, /* Monochrome 12-bit packed */
    U3V_PFNC_Mono14                           = 0x01100025, /* Monochrome 14-bit unpacked */
    U3V_PFNC_Mono14p                          = 0x010E0104, /* Monochrome 14-bit packed */
    U3V_PFNC_Mono16                           = 0x01100007, /* Monochrome 16-bit */
    U3V_PFNC_Mono32                           = 0x01200111, /* Monochrome 32-bit */
    U3V_PFNC_BayerBG4p                        = 0x01040110, /* Bayer Blue-Green 4-bit packed */
    U3V_PFNC_BayerBG8                         = 0x0108000B, /* Bayer Blue-Green 8-bit */
    U3V_PFNC_BayerBG10                        = 0x0110000F, /* Bayer Blue-Green 10-bit unpacked */
    U3V_PFNC_BayerBG10p                       = 0x010A0052, /* Bayer Blue-Green 10-bit packed */
    U3V_PFNC_BayerBG12                        = 0x01100013, /* Bayer Blue-Green 12-bit unpacked */
    U3V_PFNC_BayerBG12p                       = 0x010C0053, /* Bayer Blue-Green 12-bit packed */
    U3V_PFNC_BayerBG14                        = 0x0110010C, /* Bayer Blue-Green 14-bit */
    U3V_PFNC_BayerBG14p                       = 0x010E0108, /* Bayer Blue-Green 14-bit packed */
    U3V_PFNC_BayerBG16                        = 0x01100031, /* Bayer Blue-Green 16-bit */
    U3V_PFNC_BayerGB4p                        = 0x0104010F, /* Bayer Green-Blue 4-bit packed */
    U3V_PFNC_BayerGB8                         = 0x0108000A, /* Bayer Green-Blue 8-bit */
    U3V_PFNC_BayerGB10                        = 0x0110000E, /* Bayer Green-Blue 10-bit unpacked */
    U3V_PFNC_BayerGB10p                       = 0x010A0054, /* Bayer Green-Blue 10-bit packed */
    U3V_PFNC_BayerGB12                        = 0x01100012, /* Bayer Green-Blue 12-bit unpacked */
    U3V_PFNC_BayerGB12p                       = 0x010C0055, /* Bayer Green-Blue 12-bit packed */
    U3V_PFNC_BayerGB14                        = 0x0110010B, /* Bayer Green-Blue 14-bit */
    U3V_PFNC_BayerGB14p                       = 0x010E0107, /* Bayer Green-Blue 14-bit packed */
    U3V_PFNC_BayerGB16                        = 0x01100030, /* Bayer Green-Blue 16-bit */
    U3V_PFNC_BayerGR4p                        = 0x0104010D, /* Bayer Green-Red 4-bit packed */
    U3V_PFNC_BayerGR8                         = 0x01080008, /* Bayer Green-Red 8-bit */
    U3V_PFNC_BayerGR10                        = 0x0110000C, /* Bayer Green-Red 10-bit unpacked */
    U3V_PFNC_BayerGR10p                       = 0x010A0056, /* Bayer Green-Red 10-bit packed */
    U3V_PFNC_BayerGR12                        = 0x01100010, /* Bayer Green-Red 12-bit unpacked */
    U3V_PFNC_BayerGR12p                       = 0x010C0057, /* Bayer Green-Red 12-bit packed */
    U3V_PFNC_BayerGR14                        = 0x01100109, /* Bayer Green-Red 14-bit */
    U3V_PFNC_BayerGR14p                       = 0x010E0105, /* Bayer Green-Red 14-bit packed */
    U3V_PFNC_BayerGR16                        = 0x0110002E, /* Bayer Green-Red 16-bit */
    U3V_PFNC_BayerRG4p                        = 0x0104010E, /* Bayer Red-Green 4-bit packed */
    U3V_PFNC_BayerRG8                         = 0x01080009, /* Bayer Red-Green 8-bit */
    U3V_PFNC_BayerRG10                        = 0x0110000D, /* Bayer Red-Green 10-bit unpacked */
    U3V_PFNC_BayerRG10p                       = 0x010A0058, /* Bayer Red-Green 10-bit packed */
    U3V_PFNC_BayerRG12                        = 0x01100011, /* Bayer Red-Green 12-bit unpacked */
    U3V_PFNC_BayerRG12p                       = 0x010C0059, /* Bayer Red-Green 12-bit packed */
    U3V_PFNC_BayerRG14                        = 0x0110010A, /* Bayer Red-Green 14-bit */
    U3V_PFNC_BayerRG14p                       = 0x010E0106, /* Bayer Red-Green 14-bit packed */
    U3V_PFNC_BayerRG16                        = 0x0110002F, /* Bayer Red-Green 16-bit */
    U3V_PFNC_RGBa8                            = 0x02200016, /* Red-Green-Blue-alpha 8-bit */
    U3V_PFNC_RGBa10                           = 0x0240005F, /* Red-Green-Blue-alpha 10-bit unpacked */
    U3V_PFNC_RGBa10p                          = 0x02280060, /* Red-Green-Blue-alpha 10-bit packed */
    U3V_PFNC_RGBa12                           = 0x02400061, /* Red-Green-Blue-alpha 12-bit unpacked */
    U3V_PFNC_RGBa12p                          = 0x02300062, /* Red-Green-Blue-alpha 12-bit packed */
    U3V_PFNC_RGBa14                           = 0x02400063, /* Red-Green-Blue-alpha 14-bit unpacked */
    U3V_PFNC_RGBa16                           = 0x02400064, /* Red-Green-Blue-alpha 16-bit */
    U3V_PFNC_RGB8                             = 0x02180014, /* Red-Green-Blue 8-bit */
    U3V_PFNC_RGB8_Planar                      = 0x02180021, /* Red-Green-Blue 8-bit planar */
    U3V_PFNC_RGB10                            = 0x02300018, /* Red-Green-Blue 10-bit unpacked */
    U3V_PFNC_RGB10_Planar                     = 0x02300022, /* Red-Green-Blue 10-bit unpacked planar */
    U3V_PFNC_RGB10p                           = 0x021E005C, /* Red-Green-Blue 10-bit packed */
    U3V_PFNC_RGB10p32                         = 0x0220001D, /* Red-Green-Blue 10-bit packed into 32-bit */
    U3V_PFNC_RGB12                            = 0x0230001A, /* Red-Green-Blue 12-bit unpacked */
    U3V_PFNC_RGB12_Planar                     = 0x02300023, /* Red-Green-Blue 12-bit unpacked planar */
    U3V_PFNC_RGB12p                           = 0x0224005D, /* Red-Green-Blue 12-bit packed */
    U3V_PFNC_RGB14                            = 0x0230005E, /* Red-Green-Blue 14-bit unpacked */
    U3V_PFNC_RGB16                            = 0x02300033, /* Red-Green-Blue 16-bit */
    U3V_PFNC_RGB16_Planar                     = 0x02300024, /* Red-Green-Blue 16-bit planar */
    U3V_PFNC_RGB565p                          = 0x02100035, /* Red-Green-Blue 5/6/5-bit packed */
    U3V_PFNC_BGRa8                            = 0x02200017, /* Blue-Green-Red-alpha 8-bit */
    U3V_PFNC_BGRa10                           = 0x0240004C, /* Blue-Green-Red-alpha 10-bit unpacked */
    U3V_PFNC_BGRa10p                          = 0x0228004D, /* Blue-Green-Red-alpha 10-bit packed */
    U3V_PFNC_BGRa12                           = 0x0240004E, /* Blue-Green-Red-alpha 12-bit unpacked */
    U3V_PFNC_BGRa12p                          = 0x0230004F, /* Blue-Green-Red-alpha 12-bit packed */
    U3V_PFNC_BGRa14                           = 0x02400050, /* Blue-Green-Red-alpha 14-bit unpacked */
    U3V_PFNC_BGRa16                           = 0x02400051, /* Blue-Green-Red-alpha 16-bit */
    U3V_PFNC_BGR8                             = 0x02180015, /* Blue-Green-Red 8-bit */
    U3V_PFNC_BGR10                            = 0x02300019, /* Blue-Green-Red 10-bit unpacked */
    U3V_PFNC_BGR10p                           = 0x021E0048, /* Blue-Green-Red 10-bit packed */
    U3V_PFNC_BGR12                            = 0x0230001B, /* Blue-Green-Red 12-bit unpacked */
    U3V_PFNC_BGR12p                           = 0x02240049, /* Blue-Green-Red 12-bit packed */
    U3V_PFNC_BGR14                            = 0x0230004A, /* Blue-Green-Red 14-bit unpacked */
    U3V_PFNC_BGR16                            = 0x0230004B, /* Blue-Green-Red 16-bit */
    U3V_PFNC_BGR565p                          = 0x02100036, /* Blue-Green-Red 5/6/5-bit packed */
    U3V_PFNC_R8                               = 0x010800C9, /* Red 8-bit */
    U3V_PFNC_R10                              = 0x01100120, /* Red 10-bit */
    U3V_PFNC_R10_Deprecated                   = 0x010A00CA, /* Deprecated because size field is wrong */
    U3V_PFNC_R12                              = 0x01100121, /* Red 12-bit */
    U3V_PFNC_R12_Deprecated                   = 0x010C00CB, /* Deprecated because size field is wrong */
    U3V_PFNC_R16                              = 0x011000CC, /* Red 16-bit */
    U3V_PFNC_G8                               = 0x010800CD, /* Green 8-bit */
    U3V_PFNC_G10                              = 0x01100122, /* Green 10-bit */
    U3V_PFNC_G10_Deprecated                   = 0x010A00CE, /* Deprecated because size field is wrong */
    U3V_PFNC_G12                              = 0x01100123, /* Green 12-bit */
    U3V_PFNC_G12_Deprecated                   = 0x010C00CF, /* Deprecated because size field is wrong */
    U3V_PFNC_G16                              = 0x011000D0, /* Green 16-bit */
    U3V_PFNC_B8                               = 0x010800D1, /* Blue 8-bit */
    U3V_PFNC_B10                              = 0x01100124, /* Blue 10-bit */
    U3V_PFNC_B10_Deprecated                   = 0x010A00D2, /* Deprecated because size field is wrong */
    U3V_PFNC_B12                              = 0x01100125, /* Blue 12-bit */
    U3V_PFNC_B12_Deprecated                   = 0x010C00D3, /* Deprecated because size field is wrong */
    U3V_PFNC_B16                              = 0x011000D4, /* Blue 16-bit */
    U3V_PFNC_BiColorBGRG8                     = 0x021000A6, /* Bi-color Blue/Green - Red/Green 8-bit */
    U3V_PFNC_BiColorBGRG10                    = 0x022000A9, /* Bi-color Blue/Green - Red/Green 10-bit unpacked */
    U3V_PFNC_BiColorBGRG10p                   = 0x021400AA, /* Bi-color Blue/Green - Red/Green 10-bit packed */
    U3V_PFNC_BiColorBGRG12                    = 0x022000AD, /* Bi-color Blue/Green - Red/Green 12-bit unpacked */
    U3V_PFNC_BiColorBGRG12p                   = 0x021800AE, /* Bi-color Blue/Green - Red/Green 12-bit packed */
    U3V_PFNC_BiColorRGBG8                     = 0x021000A5, /* Bi-color Red/Green - Blue/Green 8-bit */
    U3V_PFNC_BiColorRGBG10                    = 0x022000A7, /* Bi-color Red/Green - Blue/Green 10-bit unpacked */
    U3V_PFNC_BiColorRGBG10p                   = 0x021400A8, /* Bi-color Red/Green - Blue/Green 10-bit packed */
    U3V_PFNC_BiColorRGBG12                    = 0x022000AB, /* Bi-color Red/Green - Blue/Green 12-bit unpacked */
    U3V_PFNC_BiColorRGBG12p                   = 0x021800AC, /* Bi-color Red/Green - Blue/Green 12-bit packed */
    U3V_PFNC_Data8                            = 0x01080116, /* Data 8-bit */
    U3V_PFNC_Data8s                           = 0x01080117, /* Data 8-bit signed */
    U3V_PFNC_Data16                           = 0x01100118, /* Data 16-bit */
    U3V_PFNC_Data16s                          = 0x01100119, /* Data 16-bit signed */
    U3V_PFNC_Data32                           = 0x0120011A, /* Data 32-bit */
    U3V_PFNC_Data32f                          = 0x0120011C, /* Data 32-bit floating point */
    U3V_PFNC_Data32s                          = 0x0120011B, /* Data 32-bit signed */
    U3V_PFNC_Data64                           = 0x0140011D, /* Data 64-bit */
    U3V_PFNC_Data64f                          = 0x0140011F, /* Data 64-bit floating point */
    U3V_PFNC_Data64s                          = 0x0140011E, /* Data 64-bit signed */
    U3V_PFNC_YCbCr8                           = 0x0218005B, /* YCbCr 4:4:4 8-bit */
    U3V_PFNC_YCbCr8_CbYCr                     = 0x0218003A, /* YCbCr 4:4:4 8-bit */
    U3V_PFNC_YCbCr10_CbYCr                    = 0x02300083, /* YCbCr 4:4:4 10-bit unpacked */
    U3V_PFNC_YCbCr10p_CbYCr                   = 0x021E0084, /* YCbCr 4:4:4 10-bit packed */
    U3V_PFNC_YCbCr12_CbYCr                    = 0x02300085, /* YCbCr 4:4:4 12-bit unpacked */
    U3V_PFNC_YCbCr12p_CbYCr                   = 0x02240086, /* YCbCr 4:4:4 12-bit packed */
    U3V_PFNC_YCbCr411_8                       = 0x020C005A, /* YCbCr 4:1:1 8-bit */
    U3V_PFNC_YCbCr411_8_CbYYCrYY              = 0x020C003C, /* YCbCr 4:1:1 8-bit */
    U3V_PFNC_YCbCr420_8_YY_CbCr_Semiplanar    = 0x020C0112, /* YCbCr 4:2:0 8-bit YY/CbCr Semiplanar */
    U3V_PFNC_YCbCr420_8_YY_CrCb_Semiplanar    = 0x020C0114, /* YCbCr 4:2:0 8-bit YY/CrCb Semiplanar */
    U3V_PFNC_YCbCr422_8                       = 0x0210003B, /* YCbCr 4:2:2 8-bit */
    U3V_PFNC_YCbCr422_8_CbYCrY                = 0x02100043, /* YCbCr 4:2:2 8-bit */
    U3V_PFNC_YCbCr422_8_YY_CbCr_Semiplanar    = 0x02100113, /* YCbCr 4:2:2 8-bit YY/CbCr Semiplanar */
    U3V_PFNC_YCbCr422_8_YY_CrCb_Semiplanar    = 0x02100115, /* YCbCr 4:2:2 8-bit YY/CrCb Semiplanar */
    U3V_PFNC_YCbCr422_10                      = 0x02200065, /* YCbCr 4:2:2 10-bit unpacked */
    U3V_PFNC_YCbCr422_10_CbYCrY               = 0x02200099, /* YCbCr 4:2:2 10-bit unpacked */
    U3V_PFNC_YCbCr422_10p                     = 0x02140087, /* YCbCr 4:2:2 10-bit packed */
    U3V_PFNC_YCbCr422_10p_CbYCrY              = 0x0214009A, /* YCbCr 4:2:2 10-bit packed */
    U3V_PFNC_YCbCr422_12                      = 0x02200066, /* YCbCr 4:2:2 12-bit unpacked */
    U3V_PFNC_YCbCr422_12_CbYCrY               = 0x0220009B, /* YCbCr 4:2:2 12-bit unpacked */
    U3V_PFNC_YCbCr422_12p                     = 0x02180088, /* YCbCr 4:2:2 12-bit packed */
    U3V_PFNC_YCbCr422_12p_CbYCrY              = 0x0218009C, /* YCbCr 4:2:2 12-bit packed */
    U3V_PFNC_YCbCr601_8_CbYCr                 = 0x0218003D, /* YCbCr 4:4:4 8-bit BT.601 */
    U3V_PFNC_YCbCr601_10_CbYCr                = 0x02300089, /* YCbCr 4:4:4 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_10p_CbYCr               = 0x021E008A, /* YCbCr 4:4:4 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_12_CbYCr                = 0x0230008B, /* YCbCr 4:4:4 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_12p_CbYCr               = 0x0224008C, /* YCbCr 4:4:4 12-bit packed BT.601 */
    U3V_PFNC_YCbCr601_411_8_CbYYCrYY          = 0x020C003F, /* YCbCr 4:1:1 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_8                   = 0x0210003E, /* YCbCr 4:2:2 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_8_CbYCrY            = 0x02100044, /* YCbCr 4:2:2 8-bit BT.601 */
    U3V_PFNC_YCbCr601_422_10                  = 0x0220008D, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_10_CbYCrY           = 0x0220009D, /* YCbCr 4:2:2 10-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_10p                 = 0x0214008E, /* YCbCr 4:2:2 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_10p_CbYCrY          = 0x0214009E, /* YCbCr 4:2:2 10-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_12                  = 0x0220008F, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_12_CbYCrY           = 0x0220009F, /* YCbCr 4:2:2 12-bit unpacked BT.601 */
    U3V_PFNC_YCbCr601_422_12p                 = 0x02180090, /* YCbCr 4:2:2 12-bit packed BT.601 */
    U3V_PFNC_YCbCr601_422_12p_CbYCrY          = 0x021800A0, /* YCbCr 4:2:2 12-bit packed BT.601 */
    U3V_PFNC_YCbCr709_8_CbYCr                 = 0x02180040, /* YCbCr 4:4:4 8-bit BT.709 */
    U3V_PFNC_YCbCr709_10_CbYCr                = 0x02300091, /* YCbCr 4:4:4 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_10p_CbYCr               = 0x021E0092, /* YCbCr 4:4:4 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_12_CbYCr                = 0x02300093, /* YCbCr 4:4:4 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_12p_CbYCr               = 0x02240094, /* YCbCr 4:4:4 12-bit packed BT.709 */
    U3V_PFNC_YCbCr709_411_8_CbYYCrYY          = 0x020C0042, /* YCbCr 4:1:1 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_8                   = 0x02100041, /* YCbCr 4:2:2 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_8_CbYCrY            = 0x02100045, /* YCbCr 4:2:2 8-bit BT.709 */
    U3V_PFNC_YCbCr709_422_10                  = 0x02200095, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_10_CbYCrY           = 0x022000A1, /* YCbCr 4:2:2 10-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_10p                 = 0x02140096, /* YCbCr 4:2:2 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_10p_CbYCrY          = 0x021400A2, /* YCbCr 4:2:2 10-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_12                  = 0x02200097, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_12_CbYCrY           = 0x022000A3, /* YCbCr 4:2:2 12-bit unpacked BT.709 */
    U3V_PFNC_YCbCr709_422_12p                 = 0x02180098, /* YCbCr 4:2:2 12-bit packed BT.709 */
    U3V_PFNC_YCbCr709_422_12p_CbYCrY          = 0x021800A4, /* YCbCr 4:2:2 12-bit packed BT.709 */
    U3V_PFNC_YCbCr2020_8_CbYCr                = 0x021800F4, /* YCbCr 4:4:4 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_10_CbYCr               = 0x023000F5, /* YCbCr 4:4:4 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_10p_CbYCr              = 0x021E00F6, /* YCbCr 4:4:4 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_12_CbYCr               = 0x023000F7, /* YCbCr 4:4:4 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_12p_CbYCr              = 0x022400F8, /* YCbCr 4:4:4 12-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_411_8_CbYYCrYY         = 0x020C00F9, /* YCbCr 4:1:1 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_8                  = 0x021000FA, /* YCbCr 4:2:2 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_8_CbYCrY           = 0x021000FB, /* YCbCr 4:2:2 8-bit BT.2020 */
    U3V_PFNC_YCbCr2020_422_10                 = 0x022000FC, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_10_CbYCrY          = 0x022000FD, /* YCbCr 4:2:2 10-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_10p                = 0x021400FE, /* YCbCr 4:2:2 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_10p_CbYCrY         = 0x021400FF, /* YCbCr 4:2:2 10-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_12                 = 0x02200100, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_12_CbYCrY          = 0x02200101, /* YCbCr 4:2:2 12-bit unpacked BT.2020 */
    U3V_PFNC_YCbCr2020_422_12p                = 0x02180102, /* YCbCr 4:2:2 12-bit packed BT.2020 */
    U3V_PFNC_YCbCr2020_422_12p_CbYCrY         = 0x02180103, /* YCbCr 4:2:2 12-bit packed BT.2020 */
    U3V_PFNC_YUV8_UYV                         = 0x02180020, /* YUV 4:4:4 8-bit */
    U3V_PFNC_YUV411_8_UYYVYY                  = 0x020C001E, /* YUV 4:1:1 8-bit */
    U3V_PFNC_YUV422_8                         = 0x02100032, /* YUV 4:2:2 8-bit */
    U3V_PFNC_YUV422_8_UYVY                    = 0x0210001F, /* YUV 4:2:2 8-bit */
    U3V_PFNC_InvalidPixelFormat               = 0x00000000
} T_U3VPfnc;

/**
 * U3V Acquisition control command value.
 * 
 */
typedef enum
{
    U3V_ACQ_START_CMD_VAL                     = 0x01,
    U3V_ACQ_STOP_CMD_VAL                      = 0x00
} T_U3VAcqCmdVal;

/**
 * U3V Stream control command value.
 * 
 */
typedef enum
{
    U3V_SI_CTRL_CMD_ENABLE                    = 0x01,
    U3V_SI_CTRL_CMD_DISABLE                   = 0x00
} T_U3VSiCtrlCmd;

/**
 * U3V Stream payload type identifier.
 * 
 */
typedef enum
{
    U3V_STREAM_PLD_TYPE_IMAGE                 = 0x0001,
    U3V_STREAM_PLD_TYPE_IMAGE_EXT_CHUNK       = 0x4001,
    U3V_STREAM_PLD_TYPE_CHUNK                 = 0x4000
} T_U3VStreamPayloadType;


#ifdef __cplusplus
}
#endif //__cplusplus

