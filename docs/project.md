# Project: T1000-E LoRaWAN + iBeacon Tracker

## Core Objective

Develop firmware for the Seeed Studio T1000-E tracking device that combines LoRaWAN-based location tracking with simultaneous iBeacon advertising. The beacon will broadcast real-time sensor data, location information, and emergency alerts.

## Technology Stack

*   **Hardware**: Seeed Studio T1000-E (based on nRF52840 MCU and Semtech LR1110 LoRa transceiver)
*   **Operating System**: Zephyr RTOS
*   **Build System**: PlatformIO
*   **Key Libraries & APIs**:
    *   **Zephyr Bluetooth Stack**: For implementing the iBeacon functionality.
    *   **Semtech LoRa Basics Modem**: The core library for managing the LoRaWAN stack and modem operations.
    *   **Zephyr Sensor API**: For interfacing with onboard sensors.
    *   **Custom Drivers**: Zephyr-compliant drivers and interfaces for the LR1110's GNSS and Wi-Fi scanning capabilities, and for the QMA6100P accelerometer.
