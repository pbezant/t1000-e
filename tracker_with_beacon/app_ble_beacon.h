#ifndef __APP_BLE_BEACON_H__
#define __APP_BLE_BEACON_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize iBeacon advertising
 */
void app_ble_beacon_init(void);

/**
 * @brief Start iBeacon advertising
 */
void app_ble_beacon_start(void);

/**
 * @brief Stop iBeacon advertising
 */
void app_ble_beacon_stop(void);

/**
 * @brief Update iBeacon scan response with sensor data
 * 
 * @param battery Battery level (0-100)
 * @param temp Temperature in 0.1°C
 * @param light Light level in lux
 * @param ax Accelerometer X axis
 * @param ay Accelerometer Y axis  
 * @param az Accelerometer Z axis
 * @param emergency Emergency status flag
 */
void app_ble_beacon_update_sensor_data(uint8_t battery, int16_t temp, uint16_t light, 
                                       int16_t ax, int16_t ay, int16_t az, bool emergency);

/**
 * @brief Set emergency mode (changes beacon data)
 * 
 * @param emergency Emergency status
 */
void app_ble_beacon_set_emergency(bool emergency);

#ifdef __cplusplus
}
#endif

#endif /* __APP_BLE_BEACON_H__ */ 