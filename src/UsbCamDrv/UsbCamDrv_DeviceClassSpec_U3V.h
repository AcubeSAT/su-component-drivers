//
// Created by mojo on 29/01/22.
//

#ifndef COMPONENT_DRIVERS_USBCAMDRV_DEVCLASSU3V_H
#define COMPONENT_DRIVERS_USBCAMDRV_DEVCLASSU3V_H

#include "UsbCamDrv_App.h"

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************
* Macro definitions
*********************************************************/

/*** USB3Vision - USB IF Device class ***/
#define U3V_USB_DEVICE_CLASS                           0xEF     /* Miscellaneous class of USB-IF */
#define U3V_USB_DEVICE_SUBCLASS                        0x05     /* USB3 Vision subclass of Misc */
#define U3V_USB_DEVICE_CONTROL_IF_PRCOL                0x00     /* Control IF protocol for USB3 Vision */
#define U3V_USB_DEVICE_EVENT_IF_PRCOL                  0x01     /* Event IF protocol for USB3 Vision */
#define U3V_USB_DEVICE_STREAMING_IF_PRCOL              0x02     /* Streaming IF protocol for USB3 Vision */

/*** USB3Vision - GenCP Registers Size ***/
#define U3V_REG_GENCP_VERSION_SIZE                     4u
#define U3V_REG_MANUFACTURER_NAME_SIZE                 64u
#define U3V_REG_MODEL_NAME_SIZE                        64u
#define U3V_REG_FAMILY_NAME_SIZE                       64u
#define U3V_REG_DEVICE_VERSION_SIZE                    64u
#define U3V_REG_MANUFACTURER_INFO_SIZE                 64u
#define U3V_REG_SERIAL_NUMBER_SIZE                     64u
#define U3V_REG_USER_DEFINED_NAME_SIZE                 64u
#define U3V_REG_DEVICE_CAPABILITY_SIZE                 8u
#define U3V_REG_MAX_DEVICE_RESPONSE_TIME_MS_SIZE       4u
#define U3V_REG_MANIFEST_TABLE_ADDRESS_SIZE            8u
#define U3V_REG_SBRM_ADDRESS_SIZE                      8u
#define U3V_REG_DEVICE_CONFIGURATION_SIZE              8u
#define U3V_REG_HEARTBEAT_TIMEOUT_SIZE                 4u
#define U3V_REG_MESSAGE_CHANNEL_ID_SIZE                4u
#define U3V_REG_TIMESTAMP_SIZE                         8u
#define U3V_REG_TIMESTAMP_LATCH_SIZE                   4u
#define U3V_REG_TIMESTAMP_INCREMENT_SIZE               8u
#define U3V_REG_ACCESS_PRIVILEGE_SIZE                  4u
#define U3V_REG_RESERVED_DPRCTD_AREA_SIZE              4u
#define U3V_REG_IMPLEMENTATION_ENDIANESS_SIZE          4u
#define U3V_REG_RESERVED_SPACE_SIZE                    65008u


/********************************************************
* Type definitions
*********************************************************/

/*** USB3 Vision - GenCP Registers Offset Mapping ***/
typedef enum
{
    U3V_REG_GENCP_VERSION_OFS                        = 0x00000,
    U3V_REG_MANUFACTURER_NAME_OFS                    = 0x00004,
    U3V_REG_MODEL_NAME_OFS                           = 0x00044,
    U3V_REG_FAMILY_NAME_OFS                          = 0x00084,
    U3V_REG_DEVICE_VERSION_OFS                       = 0x000C4,
    U3V_REG_MANUFACTURER_INFO_OFS                    = 0x00104,
    U3V_REG_SERIAL_NUMBER_OFS                        = 0x00144,
    U3V_REG_USER_DEFINED_NAME_OFS                    = 0x00184,
    U3V_REG_DEVICE_CAPABILITY_OFS                    = 0x001C4,
    U3V_REG_MAX_DEVICE_RESPONSE_TIME_MS_OFS          = 0x001CC,
    U3V_REG_MANIFEST_TABLE_ADDRESS_OFS               = 0x001D0,
    U3V_REG_SBRM_ADDRESS_OFS                         = 0x001D8,
    U3V_REG_DEVICE_CONFIGURATION_OFS                 = 0x001E0,
    U3V_REG_HEARTBEAT_TIMEOUT_OFS                    = 0x001E8,
    U3V_REG_MESSAGE_CHANNEL_ID_OFS                   = 0x001EC,
    U3V_REG_TIMESTAMP_OFS                            = 0x001F0,
    U3V_REG_TIMESTAMP_LATCH_OFS                      = 0x001F8,
    U3V_REG_TIMESTAMP_INCREMENT_OFS                  = 0x001FC,
    U3V_REG_ACCESS_PRIVILEGE_OFS                     = 0x00204,
    U3V_REG_RESERVED_DPRCTD_AREA_OFS                 = 0x00208,
    U3V_REG_IMPLEMENTATION_ENDIANESS_OFS             = 0x0020C,
    U3V_REG_RESERVED_SPACE_OFS                       = 0x00210,
} T_U3VGenCpRegisterOffsetMap;


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //COMPONENT_DRIVERS_USBCAMDRV_DEVCLASSU3V_H
