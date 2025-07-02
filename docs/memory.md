# Project Memory & Decisions Log

This document records key decisions, problems solved, and approaches rejected during the development of the T1000-E tracker firmware.

## 2023-10-27: Initial Project Scoping

*   **Decision**: Re-implement the T1000-E tracker logic within the Zephyr RTOS instead of directly porting the bare-metal example code.
*   **Reasoning**: The user's `platformio.ini` specifies the Zephyr framework. A native Zephyr application is more robust, scalable, and better suited for managing concurrent BLE and LoRaWAN operations than a direct port of the original single-threaded, bare-metal example. This approach allows us to leverage Zephyr's mature Bluetooth stack, kernel features (threads, mutexes), and device driver model.
*   **Rejected Approach**: Simply copying the code from `main_lorawan_tracker.c` into the `src` directory. This would fail to compile and would require a complete rewrite of the underlying hardware abstraction layer (HAL) without the benefits of the RTOS.

*   **Decision**: Use a combination of a standard iBeacon advertising packet and a custom scan response packet.
*   **Reasoning**: The user requires both standard iBeacon compatibility and the ability to broadcast rich, dynamic data (sensors, location, status). The standard iBeacon format has no room for this custom data.
    *   The **advertising packet** will contain only the standard iBeacon payload, ensuring it can be discovered by any iBeacon-compliant scanner.
    *   The **scan response packet** will contain the device name and a custom manufacturer data field with our structured sensor data. This is a best practice for extending BLE advertisements without breaking standards.
*   **Rejected Approach**: Stuffing all data into one advertising packet. This would either violate the iBeacon standard or require a fully custom (non-iBeacon) format, which goes against the user's request.

*   **Decision**: The Semtech LoRa Basics Modem library will be used for the LoRaWAN stack.
*   **Reasoning**: This is the library used in the original, functional example, and it handles the complexity of the LoRaWAN protocol and the LR1110 modem. The main challenge will be to write a new HAL for it that is compatible with Zephyr's APIs.
*   **Rejected Approach**: Using Zephyr's built-in LoRaWAN stack. While one exists, it might not have the same level of direct support and examples for the LR1110 modem as the official Semtech library. Sticking with the LoRa Basics Modem library reduces the risk associated with the LoRaWAN part of the project. 