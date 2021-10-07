#ifndef MCP9808DRIVER_MCP9808_CONSTANTS_HPP
#define MCP9808DRIVER_MCP9808_CONSTANTS_HPP

/**
 * User constants - FOR USE IN FUNCTION CALLS AND CONFIGURATION
 */

// User defined I2C address (bits A3-A1, see datasheet for acceptable values)
#define MCP9808_I2C_USER_ADDR                           (0x00)

// Register addresses
#define MCP9808_REG_RFU                                 (0x00u)
#define MCP9808_REG_CONFIG                              (0x01u)
#define MCP9808_REG_TUPPER                              (0x02u)
#define MCP9808_REG_TLOWER                              (0x03u)
#define MCP9808_REG_TCRIT                               (0x04u)
#define MCP9808_REG_TEMP                                (0x05u)
#define MCP9808_REG_MFGID                               (0x06u)
#define MCP9808_REG_DEVID                               (0x07u)
#define MCP9808_REG_RESOLUTION                          (0x08u)

// Configuration constants (use these only for any configuration
// operations so that you don't overwrite critical data)

// Hysteresis temperature options
#define MCP9808_CONFIG_THYST_0C                         (0x000u)
#define MCP9808_CONFIG_THYST_1_5C                       (0x200u)
#define MCP9808_CONFIG_THYST_3C                         (0x400u)
#define MCP9808_CONFIG_THYST_6C                         (0x600u)

// Low-power mode (SHDN)
#define MCP9808_CONFIG_LOWPWR_ENABLE                    (0x100)
#define MCP9808_CONFIG_LOWPWR_DISABLE                   (0x000)

// TCRIT (critical temperature) register locking
#define MCP9808_CONFIG_TCRIT_LOCK_ENABLE                (0x80)
#define MCP9808_CONFIG_TCRIT_LOCK_DISABLE               (0x00)

// WINLOCK (T_upper and T_lower temperature window) locking
#define MCP9808_CONFIG_WINLOCK_ENABLE                   (0x40)
#define MCP9808_CONFIG_WINLOCK_DISABLE                  (0x00)

// Set interrupts to be cleared on next read of CONFIG register
#define MCP9808_CONFIG_IRQ_CLEAR                        (0x20)

// Set alert output status (see datasheet p. 19)
#define MCP9808_CONFIG_ALERT_ENABLE                     (0x10)
#define MCP9808_CONFIG_ALERT_DISABLE                    (0x00)

// Set alert output control
#define MCP9808_CONFIG_ALERT_CONTROL_DISABLE            (0x08)
#define MCP9808_CONFIG_ALERT_CONTROL_ENABLE             (0x00)

// Set alert output selection
#define MCP9808_CONFIG_ALERT_SELECT_CRITONLY            (0x04)
#define MCP9808_CONFIG_ALERT_SELECT_ALL                 (0x00)

// Set polarity of alerts
#define MCP9808_CONFIG_ALERT_POLARITY_ACTIVE_HIGH       (0x02)
#define MCP9808_CONFIG_ALERT_POLARITY_ACTIVE_LOW        (0x00)

// Set alert output mode
#define MCP9808_CONFIG_ALERT_MODE_IRQ                   (0x01)
#define MCP9808_CONFIG_ALERT_MODE_COMPARATOR            (0x00)

// Set measurement resolution
#define MCP9808_RES_0_50C                               (0x00)
#define MCP9808_RES_0_25C                               (0x01)
#define MCP9808_RES_0_125C                              (0x02)
#define MCP9808_RES_0_0625C                             (0x03)

/**
 * System constants - INTERNAL USE ONLY!
 */

// Base slave bus address (manufacturer-defined, used on the I2C bus)
#define MCP9808_I2C_BASE_ADDR                           (0x30u)

// Mask to enable changes only to address bits 2-4 (which are user-settable)
#define MCP9808_I2C_USER_ADDR_MASK                      (0xF1u)

// Custom bus address (for usage in read-write requests)
#define MCP9808_I2C_BUS_ADDR (static_cast<uint16_t>((MCP9808_I2C_BASE_ADDR & MCP9808_I2C_USER_ADDR_MASK) | MCP9808_I2C_USER_ADDR))

// Hysteresis temperature mask
#define MCP9808_CONFIG_THYST_MASK                       (0xF9FFu)

// Low-power (SHDN) mode mask
#define MCP9808_CONFIG_SHDN_MASK                        (0xFEFFu)

#define MCP9808_CONFIG_TCRIT_LOCK_MASK                  (0xFF7Fu)
#define MCP9808_CONFIG_WINLOCK_MASK                     (0xFFBFu)
#define MCP9808_CONFIG_IRQ_CLEAR_MASK                   (0xFFDFu)
#define MCP9808_CONFIG_ALERT_STATUS_MASK                (0xFFEFu)
#define MCP9808_CONFIG_ALERT_CONTROL_MASK               (0xFFF7u)
#define MCP9808_CONFIG_ALERT_SELECT_MASK                (0xFFFBu)
#define MCP9808_CONFIG_ALERT_POLARITY_MASK              (0xFFFDu)
#define MCP9808_CONFIG_ALERT_MODE_MASK                  (0xFFFEu)

#define MCP9808_RES_MASK                                (0x00FC)

#endif //MCP9808DRIVER_MCP9808_CONSTANTS_HPP

