
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
    U3V_CONTROL_MGK_PREFIX                  =   0x43563355U, /* "U3VC" in ASCII */
    U3V_LEADER_MGK_PREFIX                   =   0x4C563355U, /* "U3VL" in ASCII */
    U3V_TRAILER_MGK_PREFIX                  =   0x54563355U  /* "U3VT" in ASCII */
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
    U3V_CTRL_READMEM_CMD                    =   0x0800U,
    U3V_CTRL_READMEM_ACK                    =   0x0801U,
    U3V_CTRL_WRITEMEM_CMD                   =   0x0802U,
    U3V_CTRL_WRITEMEM_ACK                   =   0x0803U,
    U3V_CTRL_PENDING_ACK                    =   0x0805U
} T_U3VCtrlIfCmdId;

/**
 * U3V / GenCP Technology Agnostic Bootstrap Register Map (ABRM)
 * 
 * Register map for the ABRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_ABRM_GENCP_VERSION_OFS              =   0x00000U,
    U3V_ABRM_MANUFACTURER_NAME_OFS          =   0x00004U,
    U3V_ABRM_MODEL_NAME_OFS                 =   0x00044U,
    U3V_ABRM_FAMILY_NAME_OFS                =   0x00084U,
    U3V_ABRM_DEVICE_VERSION_OFS             =   0x000C4U,
    U3V_ABRM_MANUFACTURER_INFO_OFS          =   0x00104U,
    U3V_ABRM_SERIAL_NUMBER_OFS              =   0x00144U,
    U3V_ABRM_USER_DEFINED_NAME_OFS          =   0x00184U,
    U3V_ABRM_DEVICE_CAPABILITY_OFS          =   0x001C4U,
    U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS   =   0x001CCU,
    U3V_ABRM_MANIFEST_TABLE_ADDRESS_OFS     =   0x001D0U,
    U3V_ABRM_SBRM_ADDRESS_OFS               =   0x001D8U,
    U3V_ABRM_DEVICE_CONFIGURATION_OFS       =   0x001E0U,
    U3V_ABRM_HEARTBEAT_TIMEOUT_OFS          =   0x001E8U,
    U3V_ABRM_MESSAGE_CHANNEL_ID_OFS         =   0x001ECU,
    U3V_ABRM_TIMESTAMP_OFS                  =   0x001F0U,
    U3V_ABRM_TIMESTAMP_LATCH_OFS            =   0x001F8U,
    U3V_ABRM_TIMESTAMP_INCREMENT_OFS        =   0x001FCU,
    U3V_ABRM_ACCESS_PRIVILEGE_OFS           =   0x00204U,
    U3V_ABRM_RESERVED_DPRCTD_AREA_OFS       =   0x00208U,
    U3V_ABRM_IMPLEMENTATION_ENDIANESS_OFS   =   0x0020CU,
    U3V_ABRM_RESERVED_SPACE_OFS             =   0x00210U
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
    U3V_SBRM_U3V_VERSION_OFS		        =   0x00000U,
    U3V_SBRM_U3VCP_CAPABILITY_OFS	        =   0x00004U,
    U3V_SBRM_U3VCP_CONFIGURATION_OFS        =   0x0000CU,
    U3V_SBRM_MAX_CMD_TRANSFER_OFS	        =   0x00014U,
    U3V_SBRM_MAX_ACK_TRANSFER_OFS	        =   0x00018U,
    U3V_SBRM_NUM_STREAM_CHANNELS_OFS        =   0x0001CU,
    U3V_SBRM_SIRM_ADDRESS_OFS		        =   0x00020U,
    U3V_SBRM_SIRM_LENGTH_OFS		        =   0x00028U,
    U3V_SBRM_EIRM_ADDRESS_OFS		        =   0x0002CU,
    U3V_SBRM_EIRM_LENGTH_OFS		        =   0x00034U,
    U3V_SBRM_IIDC2_ADDRESS_OFS		        =   0x00038U,
    U3V_SBRM_CURRENT_SPEED_OFS		        =   0x00040U,
    U3V_SBRM_RESERVED_OFS			        =   0x00044U
} T_U3VSbrmOffset;

/**
 * U3V / GenCP Technology Streaming Interface Register Map (SIRM) 
 * 
 * Register map for the SIRM space as specified by the USB3 Vision / GenCP 
 * standard.
 */
typedef enum
{
    U3V_SIRM_INFO_OFS			            =   0x00U,
    U3V_SIRM_CONTROL_OFS		            =   0x04U,
    U3V_SIRM_REQ_PAYLOAD_SIZE_OFS	        =   0x08U,
    U3V_SIRM_REQ_LEADER_SIZE_OFS	        =   0x10U,
    U3V_SIRM_REQ_TRAILER_SIZE_OFS	        =   0x14U,
    U3V_SIRM_MAX_LEADER_SIZE_OFS	        =   0x18U,
    U3V_SIRM_PAYLOAD_SIZE_OFS		        =   0x1CU,
    U3V_SIRM_PAYLOAD_COUNT_OFS	            =   0x20U,
    U3V_SIRM_TRANSFER1_SIZE_OFS	            =   0x24U,
    U3V_SIRM_TRANSFER2_SIZE_OFS	            =   0x28U,
    U3V_SIRM_MAX_TRAILER_SIZE_OFS	        =   0x2CU
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
    U3V_PFNC_RGBa8                          =   0x02200016U, /* Red-Green-Blue-alpha 8-bit */
    U3V_PFNC_RGBa10                         =   0x0240005FU, /* Red-Green-Blue-alpha 10-bit unpacked */
    U3V_PFNC_RGBa10p                        =   0x02280060U, /* Red-Green-Blue-alpha 10-bit packed */
    U3V_PFNC_RGBa12                         =   0x02400061U, /* Red-Green-Blue-alpha 12-bit unpacked */
    U3V_PFNC_RGBa12p                        =   0x02300062U, /* Red-Green-Blue-alpha 12-bit packed */
    U3V_PFNC_RGBa14                         =   0x02400063U, /* Red-Green-Blue-alpha 14-bit unpacked */
    U3V_PFNC_RGBa16                         =   0x02400064U, /* Red-Green-Blue-alpha 16-bit */
    U3V_PFNC_RGB8                           =   0x02180014U, /* Red-Green-Blue 8-bit */
    U3V_PFNC_RGB8_Planar                    =   0x02180021U, /* Red-Green-Blue 8-bit planar */
    U3V_PFNC_RGB10                          =   0x02300018U, /* Red-Green-Blue 10-bit unpacked */
    U3V_PFNC_RGB10_Planar                   =   0x02300022U, /* Red-Green-Blue 10-bit unpacked planar */
    U3V_PFNC_RGB10p                         =   0x021E005CU, /* Red-Green-Blue 10-bit packed */
    U3V_PFNC_RGB10p32                       =   0x0220001DU, /* Red-Green-Blue 10-bit packed into 32-bit */
    U3V_PFNC_RGB12                          =   0x0230001AU, /* Red-Green-Blue 12-bit unpacked */
    U3V_PFNC_RGB12_Planar                   =   0x02300023U, /* Red-Green-Blue 12-bit unpacked planar */
    U3V_PFNC_RGB12p                         =   0x0224005DU, /* Red-Green-Blue 12-bit packed */
    U3V_PFNC_RGB14                          =   0x0230005EU, /* Red-Green-Blue 14-bit unpacked */
    U3V_PFNC_RGB16                          =   0x02300033U, /* Red-Green-Blue 16-bit */
    U3V_PFNC_RGB16_Planar                   =   0x02300024U  /* Red-Green-Blue 16-bit planar */
} T_U3VPfnc;

/**
 * U3V Image Acquisition mode.
 * 
 */
typedef enum
{
    U3V_ACQUISITION_MODE_CONTINUOUS         =   0x0U,
    U3V_ACQUISITION_MODE_SINGLE_FRAME       =   0x1U,
    U3V_ACQUISITION_MODE_MULTI_FRAME        =   0x2U
} T_U3VImgAcqMode;

/**
 * U3V Acquisition control command value.
 * 
 */
typedef enum
{
    U3V_ACQ_START_CMD_VAL                   =   0x80000000U,
    U3V_ACQ_STOP_CMD_VAL                    =   0x00000000U
} T_U3VAcqCmdVal;

/**
 * U3V Stream control command value.
 * 
 */
typedef enum
{
    U3V_SI_CTRL_CMD_ENABLE                  =   0x00000001U,
    U3V_SI_CTRL_CMD_DISABLE                 =   0x00000000U
} T_U3VSiCtrlCmd;

/**
 * U3V Stream payload type identifier.
 * 
 */
typedef enum
{
    U3V_STREAM_PLD_TYPE_IMAGE               =   0x0001U,
    U3V_STREAM_PLD_TYPE_IMAGE_EXT_CHUNK     =   0x4001U,
    U3V_STREAM_PLD_TYPE_CHUNK               =   0x4000U
} T_U3VStreamPayloadType;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H
