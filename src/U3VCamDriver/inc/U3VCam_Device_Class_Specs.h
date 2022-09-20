
#ifndef COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H
#define COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H


#ifdef __cplusplus
extern "C" {
#endif



/********************************************************
* Macro definitions
*********************************************************/

/*** U3V - USB IF Device class ***/
#define U3V_DEVICE_CLASS_MISC                   0xEF
#define U3V_DEVICE_SUBCLASS_COMMON              0x02
#define U3V_DEVICE_PROTOCOL_IAD                 0x01
#define U3V_DESCRIPTOR_TYPE_DEVICE              0x01
#define U3V_DESCRIPTOR_TYPE_CONFIGURATION       0x02
#define U3V_DESCRIPTOR_TYPE_INTERFACE           0x04
#define U3V_DESCRIPTOR_TYPE_ENDPOINT            0x05
#define U3V_DESCRIPTOR_TYPE_IAD                 0x0B
#define U3V_INTERFACE_U3V_SUBLCASS              0x05
#define U3V_INTERFACE_CONTROL                   0x00
#define U3V_INTERFACE_EVENT                     0x01
#define U3V_INTERFACE_DATASTREAM                0x02

#define U3V_INFO_IS_U3V_INTERFACE               0x24
#define U3V_INFO_IS_DEVICEINFO                  0x01
#define U3V_INFO_MIN_LENGTH_STR                 20U
#define U3V_MAX_DESCR_STR_LENGTH                64U
#define U3V_ERR_NO_ERROR				        0x0000

/*** U3V magic key codes ***/
#define U3V_CONTROL_MGK_PREFIX                  0x43563355  /* magic key "U3VC" in ASCII */
#define U3V_LEADER_MGK_PREFIX	                0x4C563355  /* magic key "U3VL" in ASCII */
#define U3V_TRAILER_MGK_PREFIX	                0x54563355  /* magic key "U3VT" in ASCII */

/*** U3V Control IF Codes ***/
#define U3V_CTRL_REQ_ACK                        0x4000
#define U3V_CTRL_READMEM_CMD	                0x0800
#define	U3V_CTRL_READMEM_ACK	                0x0801
#define	U3V_CTRL_WRITEMEM_CMD	                0x0802
#define	U3V_CTRL_WRITEMEM_ACK	                0x0803
#define	U3V_CTRL_PENDING_ACK	                0x0805
#define	U3V_CTRL_EVENT_CMD	                    0x0C00

/*** U3V - GenCP Registers Size ***/
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

/* USB3 Vision / GenCP - Technology Agnostic Bootstrap Register Map (ABRM) */
#define U3V_ABRM_GENCP_VERSION_OFS              0x00000
#define U3V_ABRM_MANUFACTURER_NAME_OFS          0x00004
#define U3V_ABRM_MODEL_NAME_OFS                 0x00044
#define U3V_ABRM_FAMILY_NAME_OFS                0x00084
#define U3V_ABRM_DEVICE_VERSION_OFS             0x000C4
#define U3V_ABRM_MANUFACTURER_INFO_OFS          0x00104
#define U3V_ABRM_SERIAL_NUMBER_OFS              0x00144
#define U3V_ABRM_USER_DEFINED_NAME_OFS          0x00184
#define U3V_ABRM_DEVICE_CAPABILITY_OFS          0x001C4
#define U3V_ABRM_MAX_DEV_RESPONSE_TIME_MS_OFS   0x001CC
#define U3V_ABRM_MANIFEST_TABLE_ADDRESS_OFS     0x001D0
#define U3V_ABRM_SBRM_ADDRESS_OFS               0x001D8
#define U3V_ABRM_DEVICE_CONFIGURATION_OFS       0x001E0
#define U3V_ABRM_HEARTBEAT_TIMEOUT_OFS          0x001E8
#define U3V_ABRM_MESSAGE_CHANNEL_ID_OFS         0x001EC
#define U3V_ABRM_TIMESTAMP_OFS                  0x001F0
#define U3V_ABRM_TIMESTAMP_LATCH_OFS            0x001F8
#define U3V_ABRM_TIMESTAMP_INCREMENT_OFS        0x001FC
#define U3V_ABRM_ACCESS_PRIVILEGE_OFS           0x00204
#define U3V_ABRM_RESERVED_DPRCTD_AREA_OFS       0x00208
#define U3V_ABRM_IMPLEMENTATION_ENDIANESS_OFS   0x0020C
#define U3V_ABRM_RESERVED_SPACE_OFS             0x00210

/* USB3 Vision / GenCP - Technology Specific Bootstrap Register Map (SBRM) */
#define U3V_SBRM_U3V_VERSION_OFS		        0x00000
#define U3V_SBRM_U3VCP_CAPABILITY_OFS	        0x00004
#define U3V_SBRM_U3VCP_CONFIGURATION_OFS        0x0000C
#define U3V_SBRM_MAX_CMD_TRANSFER_OFS	        0x00014
#define U3V_SBRM_MAX_ACK_TRANSFER_OFS	        0x00018
#define U3V_SBRM_NUM_STREAM_CHANNELS_OFS        0x0001C
#define U3V_SBRM_SIRM_ADDRESS_OFS		        0x00020
#define U3V_SBRM_SIRM_LENGTH_OFS		        0x00028
#define U3V_SBRM_EIRM_ADDRESS_OFS		        0x0002C
#define U3V_SBRM_EIRM_LENGTH_OFS		        0x00034
#define U3V_SBRM_IIDC2_ADDRESS_OFS		        0x00038
#define U3V_SBRM_CURRENT_SPEED_OFS		        0x00040
#define U3V_SBRM_RESERVED_OFS			        0x00044

/* USB3 Vision / GenCP - Technology Streaming Interface Register Map (SIRM) */
#define U3V_SIRM_INFO_OFS			            0x00
#define U3V_SIRM_CONTROL_OFS		            0x04
#define U3V_SIRM_REQ_PAYLOAD_SIZE_OFS	        0x08
#define U3V_SIRM_REQ_LEADER_SIZE_OFS	        0x10
#define U3V_SIRM_REQ_TRAILER_SIZE_OFS	        0x14
#define U3V_SIRM_MAX_LEADER_SIZE_OFS	        0x18
#define U3V_SIRM_PAYLOAD_SIZE_OFS		        0x1C
#define U3V_SIRM_PAYLOAD_COUNT_OFS	            0x20
#define U3V_SIRM_TRANSFER1_SIZE_OFS	            0x24
#define U3V_SIRM_TRANSFER2_SIZE_OFS	            0x28
#define U3V_SIRM_MAX_TRAILER_SIZE_OFS	        0x2C

/* USB3 Vision / GenCP - Technology Streaming Interface - Other */
#define U3V_SIRM_AVAILABLE_MASK		            0x00000001
#define U3V_SIRM_INFO_ALIGNMENT_MASK	        0xFF000000
#define U3V_SIRM_INFO_ALIGNMENT_SHIFT	        0x18

/* USB3 Vision / GenCP Pixel Format Naming Convention*/
#define U3V_PFNC_RGBa8                          0x02200016 /* Red-Green-Blue-alpha 8-bit */
#define U3V_PFNC_RGBa10                         0x0240005F /* Red-Green-Blue-alpha 10-bit unpacked */
#define U3V_PFNC_RGBa10p                        0x02280060 /* Red-Green-Blue-alpha 10-bit packed */
#define U3V_PFNC_RGBa12                         0x02400061 /* Red-Green-Blue-alpha 12-bit unpacked */
#define U3V_PFNC_RGBa12p                        0x02300062 /* Red-Green-Blue-alpha 12-bit packed */
#define U3V_PFNC_RGBa14                         0x02400063 /* Red-Green-Blue-alpha 14-bit unpacked */
#define U3V_PFNC_RGBa16                         0x02400064 /* Red-Green-Blue-alpha 16-bit */
#define U3V_PFNC_RGB8                           0x02180014 /* Red-Green-Blue 8-bit */
#define U3V_PFNC_RGB8_Planar                    0x02180021 /* Red-Green-Blue 8-bit planar */
#define U3V_PFNC_RGB10                          0x02300018 /* Red-Green-Blue 10-bit unpacked */
#define U3V_PFNC_RGB10_Planar                   0x02300022 /* Red-Green-Blue 10-bit unpacked planar */
#define U3V_PFNC_RGB10p                         0x021E005C /* Red-Green-Blue 10-bit packed */
#define U3V_PFNC_RGB10p32                       0x0220001D /* Red-Green-Blue 10-bit packed into 32-bit */
#define U3V_PFNC_RGB12                          0x0230001A /* Red-Green-Blue 12-bit unpacked */
#define U3V_PFNC_RGB12_Planar                   0x02300023 /* Red-Green-Blue 12-bit unpacked planar */
#define U3V_PFNC_RGB12p                         0x0224005D /* Red-Green-Blue 12-bit packed */
#define U3V_PFNC_RGB14                          0x0230005E /* Red-Green-Blue 14-bit unpacked */
#define U3V_PFNC_RGB16                          0x02300033 /* Red-Green-Blue 16-bit */
#define U3V_PFNC_RGB16_Planar                   0x02300024 /* Red-Green-Blue 16-bit planar */

/* USB3 Vision Acquisition Modes */
#define U3V_ACQUISITION_MODE_CONTINUOUS         0x0
#define U3V_ACQUISITION_MODE_SINGLE_FRAME       0x1
#define U3V_ACQUISITION_MODE_MULTI_FRAME        0x2

/* USB3 Vision Stream Payload types */
#define U3V_STREAM_PLD_TYPE_IMAGE                   0x0001
#define U3V_STREAM_PLD_TYPE_IMAGE_EXTENDED_CHUNK    0x4001
#define U3V_STREAM_PLD_TYPE_CHUNK                   0x4000



#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_U3VCAM_DEVICE_CLASS_SPECS_H
