#include "app_ble_beacon.h"
#include "nordic_common.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "nrf_sdh_ble.h"
#include "nrf_log.h"
#include "smtc_hal.h"

// iBeacon UUID: E2C56DB5-DFFB-48D2-B060-D0F5A71096E0
static const uint8_t ibeacon_uuid[16] = {
    0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2,
    0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0
};

// iBeacon advertising data
static uint8_t ibeacon_adv_data[] = {
    0x02, 0x01, 0x06,                           // Flags: LE General Discoverable, BR/EDR not supported
    0x1A, 0xFF,                                 // Manufacturer Specific Data
    0x4C, 0x00,                                 // Apple Company ID (0x004C)
    0x02, 0x15,                                 // iBeacon Type and Length
    // UUID (16 bytes) - will be filled in
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01,                                 // Major (will be updated)
    0x00, 0x01,                                 // Minor (will be updated)
    0xC5                                        // Measured Power (-59 dBm)
};

// Scan response data with device name and sensor data
static uint8_t scan_response_data[31];
static uint8_t scan_response_len = 0;
static bool emergency_mode = false;

// Current sensor values
static uint8_t current_battery = 0;
static int16_t current_temp = 0;
static uint16_t current_light = 0;
static int16_t current_ax = 0, current_ay = 0, current_az = 0;

static void build_scan_response_data(void);
static void update_ibeacon_advertising(void);

void app_ble_beacon_init(void)
{
    // Copy UUID into advertising data
    memcpy(&ibeacon_adv_data[9], ibeacon_uuid, 16);
    
    // Build initial scan response
    build_scan_response_data();
    
    HAL_DBG_TRACE_INFO("iBeacon module initialized\n");
}

void app_ble_beacon_start(void)
{
    ret_code_t err_code;
    ble_gap_adv_params_t adv_params;
    ble_gap_adv_data_t adv_data;
    
    // Set advertising parameters for iBeacon
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = 0;  // Advertise indefinitely
    adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = 160; // 100ms (160 * 0.625ms)
    
    // Set advertising data
    memset(&adv_data, 0, sizeof(adv_data));
    adv_data.adv_data.p_data = ibeacon_adv_data;
    adv_data.adv_data.len    = sizeof(ibeacon_adv_data);
    adv_data.scan_rsp_data.p_data = scan_response_data;
    adv_data.scan_rsp_data.len    = scan_response_len;
    
    err_code = sd_ble_gap_adv_set_configure(&adv_params, &adv_data, NULL);
    if (err_code != NRF_SUCCESS) {
        HAL_DBG_TRACE_ERROR("Failed to configure iBeacon advertising: %d\n", err_code);
        return;
    }
    
    err_code = sd_ble_gap_adv_start(BLE_GAP_ADV_SET_HANDLE_DEFAULT, BLE_CONN_CFG_TAG_DEFAULT);
    if (err_code != NRF_SUCCESS) {
        HAL_DBG_TRACE_ERROR("Failed to start iBeacon advertising: %d\n", err_code);
        return;
    }
    
    HAL_DBG_TRACE_INFO("iBeacon advertising started\n");
}

void app_ble_beacon_stop(void)
{
    ret_code_t err_code = sd_ble_gap_adv_stop(BLE_GAP_ADV_SET_HANDLE_DEFAULT);
    if (err_code != NRF_SUCCESS) {
        HAL_DBG_TRACE_WARNING("Failed to stop iBeacon advertising: %d\n", err_code);
    } else {
        HAL_DBG_TRACE_INFO("iBeacon advertising stopped\n");
    }
}

void app_ble_beacon_update_sensor_data(uint8_t battery, int16_t temp, uint16_t light, 
                                       int16_t ax, int16_t ay, int16_t az, bool emergency)
{
    current_battery = battery;
    current_temp = temp;
    current_light = light;
    current_ax = ax;
    current_ay = ay;
    current_az = az;
    emergency_mode = emergency;
    
    // Update major/minor values based on emergency state
    if (emergency) {
        ibeacon_adv_data[25] = 0xFF; // Major = 0xFF00 (emergency)
        ibeacon_adv_data[26] = 0x00;
        ibeacon_adv_data[27] = 0x00; // Minor = battery level
        ibeacon_adv_data[28] = battery;
    } else {
        ibeacon_adv_data[25] = 0x00; // Major = 0x0001 (normal)
        ibeacon_adv_data[26] = 0x01;
        ibeacon_adv_data[27] = 0x00; // Minor = 0x0001 (normal)
        ibeacon_adv_data[28] = 0x01;
    }
    
    // Rebuild scan response data
    build_scan_response_data();
    
    // Update advertising if currently running
    update_ibeacon_advertising();
}

void app_ble_beacon_set_emergency(bool emergency)
{
    app_ble_beacon_update_sensor_data(current_battery, current_temp, current_light,
                                     current_ax, current_ay, current_az, emergency);
}

static void build_scan_response_data(void)
{
    uint8_t pos = 0;
    uint32_t mac_lsb = NRF_FICR->DEVICEADDR[0];
    
    // Device name: "t1000-XXXXXX"
    char device_name[16];
    sprintf(device_name, "t1000-%02X%02X%02X", 
            (mac_lsb >> 16) & 0xFF, (mac_lsb >> 8) & 0xFF, mac_lsb & 0xFF);
    
    uint8_t name_len = strlen(device_name);
    scan_response_data[pos++] = name_len + 1;  // Length
    scan_response_data[pos++] = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME;  // Type
    memcpy(&scan_response_data[pos], device_name, name_len);
    pos += name_len;
    
    // Custom manufacturer data with sensor readings
    scan_response_data[pos++] = 15;  // Length (1 + 2 + 12 sensor bytes)
    scan_response_data[pos++] = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;  // Type
    scan_response_data[pos++] = 0xEE;  // Custom company ID LSB
    scan_response_data[pos++] = 0xFF;  // Custom company ID MSB
    
    // Sensor data payload (12 bytes)
    scan_response_data[pos++] = current_battery;  // Battery (1 byte)
    scan_response_data[pos++] = current_temp & 0xFF;      // Temp LSB
    scan_response_data[pos++] = (current_temp >> 8) & 0xFF;  // Temp MSB
    scan_response_data[pos++] = current_light & 0xFF;     // Light LSB
    scan_response_data[pos++] = (current_light >> 8) & 0xFF; // Light MSB
    scan_response_data[pos++] = current_ax & 0xFF;        // Accel X LSB
    scan_response_data[pos++] = (current_ax >> 8) & 0xFF;    // Accel X MSB
    scan_response_data[pos++] = current_ay & 0xFF;        // Accel Y LSB
    scan_response_data[pos++] = (current_ay >> 8) & 0xFF;    // Accel Y MSB
    scan_response_data[pos++] = current_az & 0xFF;        // Accel Z LSB
    scan_response_data[pos++] = (current_az >> 8) & 0xFF;    // Accel Z MSB
    scan_response_data[pos++] = emergency_mode ? 0xFF : 0x00; // Emergency flag
    
    scan_response_len = pos;
}

static void update_ibeacon_advertising(void)
{
    ret_code_t err_code;
    ble_gap_adv_data_t adv_data;
    
    // Update advertising data
    memset(&adv_data, 0, sizeof(adv_data));
    adv_data.adv_data.p_data = ibeacon_adv_data;
    adv_data.adv_data.len    = sizeof(ibeacon_adv_data);
    adv_data.scan_rsp_data.p_data = scan_response_data;
    adv_data.scan_rsp_data.len    = scan_response_len;
    
    err_code = sd_ble_gap_adv_set_configure(NULL, &adv_data, NULL);
    if (err_code != NRF_SUCCESS) {
        HAL_DBG_TRACE_WARNING("Failed to update iBeacon advertising data: %d\n", err_code);
    }
} 