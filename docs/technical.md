# Technical Decisions & Patterns

## iBeacon Implementation

*   **Beacon Type**: Standard Apple iBeacon.
*   **Advertising Mode**: The device will use scannable, non-connectable advertising (`BT_LE_ADV_SCAN_IND`). This allows other devices to request a scan response to get more data without establishing a connection.
*   **Advertising Packet (31 bytes max)**: Contains the essential iBeacon data to ensure compatibility with standard iBeacon scanners.
    *   Flags: `BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR`
    *   Manufacturer Data:
        *   Apple Company ID: `0x004C`
        *   iBeacon Type: `0x02`
        *   Data Length: `0x15` (21 bytes)
        *   **UUID**: `E2C56DB5-DFFB-48D2-B060-D0F5A71096E0` (Default)
        *   **Major**: `1` (Default)
        *   **Minor**: `1` (Default)
        *   **Measured Power (RSSI at 1m)**: Calibrated value, TBD. Default: `-59` dBm.
*   **Scan Response Packet (31 bytes max)**: Contains the custom data and device name.
    *   **Device Name**: `t1000-[last 6 of MAC]`. Will be dynamically set at startup.
    *   **Custom Manufacturer Data**: A second manufacturer data field with a custom (non-Apple) company ID will be used to broadcast sensor, location, and status data. The format will be a TLV (Type-Length-Value) structure to remain extensible.
*   **Power Management**: BLE advertising will be temporarily paused during LoRaWAN transmissions and intensive scanning operations (Wi-Fi/GNSS) to prevent radio interference and reduce peak power consumption.

## LoRaWAN Implementation

*   **Library Integration**: The Semtech LoRa Basics Modem library will be integrated into the PlatformIO project as a Zephyr module. This involves adding it to `CMakeLists.txt` and ensuring its source files are compiled and linked.
*   **Hardware Abstraction Layer (HAL)**: The biggest technical task is creating a new `smtc_modem_hal.c` that implements the required HAL functions using Zephyr APIs. This includes:
    *   `smtc_modem_hal_gpio_init`, `smtc_modem_hal_gpio_set_value`, etc. -> Zephyr GPIO API
    *   `smtc_modem_hal_spi_write`, `smtc_modem_hal_spi_read` -> Zephyr SPI API
    *   `smtc_modem_hal_rtc_get_time_s`, `smtc_modem_hal_start_timer` -> Zephyr Kernel Timers
    *   `smtc_modem_hal_enter_critical_section`, `smtc_modem_hal_exit_critical_section` -> `k_sched_lock()`, `k_sched_unlock()`
*   **Payload Formatting**: The LoRaWAN uplink payloads will follow the same data structure as the original example to maintain compatibility, using Cayenne LPP-like data IDs.

## Emergency Mode

*   **Trigger**: A long press on the user button.
*   **Beacon Behavior**:
    *   A status byte in the custom manufacturer data (in the scan response) will be set to an "SOS" state.
    *   Advertising interval may be decreased for faster propagation.
*   **LoRaWAN Behavior**:
    *   An immediate, confirmed, high-priority uplink will be triggered using `smtc_modem_request_emergency_uplink()`.
    *   The payload will contain an SOS status byte along with the latest sensor and location data. 