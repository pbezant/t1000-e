# System Architecture

## Overview

The firmware is designed as a multi-threaded Zephyr application to handle concurrent operations (LoRaWAN, BLE, sensors) efficiently and robustly. The architecture is modular, separating concerns for easier development and maintenance.

## Threads

1.  **Main Thread**: Responsible for initializing the system, including all device drivers, kernel services, and application-specific modules. After setup, it may enter a low-power state or handle system-wide monitoring.
2.  **LoRaWAN Thread**: A dedicated thread to manage the Semtech LoRa Basics Modem engine. It will handle the LoRaWAN join process, schedule periodic data uplinks, and process downlink commands. It will run the `smtc_modem_run_engine()` loop.
3.  **Sensor & Beacon Thread**: This thread periodically:
    *   Reads data from all onboard sensors (temperature, light, battery, accelerometer).
    *   Triggers location scans (GNSS and/or Wi-Fi via the LR1110).
    *   Updates a shared data structure with the latest information.
    *   Constructs and updates the BLE iBeacon advertising and scan response packets with the new data.
4.  **Button Event Handler**: An interrupt-driven handler (or a thread that waits on a semaphore) to detect presses of the user button. When pressed, it will set an emergency flag that is read by the LoRaWAN and Beacon threads to trigger immediate alerts.

## Data Flow

*   A central, thread-safe data structure (using a `k_mutex` for protection) will hold the state of all sensor and location data.
*   **Sensors/Location Scans** write to the shared data structure.
*   **Sensor & Beacon Thread** reads from the shared structure to build the BLE advertising payload.
*   **LoRaWAN Thread** reads from the shared structure to build the LoRaWAN uplink payload.
*   **Button Handler** sets an emergency flag in the shared structure, signaling the other threads to modify their behavior (send emergency packets).

## Key Components

*   **Zephyr HAL for LoRa Basics Modem**: A critical abstraction layer that maps the hardware-agnostic function calls required by the `smtc_hal` (like GPIO, SPI, timers) to the specific Zephyr RTOS device driver APIs. This is necessary to make the Semtech library work within Zephyr.
*   **iBeacon Manager**: A module responsible for all BLE-related tasks: initialization, constructing iBeacon and scan response packets, and managing the advertising state (e.g., pausing during LoRa Tx).
*   **LoRaWAN Manager**: A module that encapsulates the interaction with the LoRa Basics Modem library, managing state (joining, joined), data transmission, and event handling.
*   **Device Drivers**: Zephyr-compliant drivers for peripherals not covered by the standard Zephyr tree, specifically the QMA6100P accelerometer and an interface to the LR1110 for GNSS/Wi-Fi scanning. 