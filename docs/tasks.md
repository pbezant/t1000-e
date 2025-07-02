# Development Tasks

## Phase 1: Project Setup & BLE Beacon

*   [x] **Task 1.1**: Set up the basic Zephyr project structure in `src/main.c`.
*   [x] **Task 1.2**: Implement iBeacon advertising.
    *   [x] Initialize the Bluetooth subsystem.
    *   [x] Get the device MAC address and format the device name (`t1000-[mac]`).
    *   [x] Construct the static iBeacon advertising packet.
    *   [x] Construct an initial scan response packet with the device name.
    *   [x] Start advertising.
*   [ ] **Task 1.3**: Verify basic beacon functionality with a scanner app (e.g., nRF Connect). (Awaiting user verification)

## Phase 2: Sensor & Data Integration

*   [ ] **Task 2.1**: Implement drivers/handlers for onboard sensors.
    *   [ ] Add driver for QMA6100P accelerometer.
    *   [ ] Add handlers for reading temperature, light, and battery level.
*   [ ] **Task 2.2**: Create the shared, thread-safe data structure for all dynamic data.
*   [ ] **Task 2.3**: Create the `Sensor & Beacon Thread`.
    *   [ ] Periodically read all sensor values and update the shared data structure.
    *   [ ] Dynamically update the custom manufacturer data in the BLE scan response packet.

## Phase 3: LoRaWAN Integration

*   [ ] **Task 3.1**: Integrate the LoRa Basics Modem library into the Zephyr build.
*   [ ] **Task 3.2**: **Implement the Zephyr-based HAL for the modem.**
    *   [ ] GPIO interface
    *   [ ] SPI interface
    *   [ ] RTC/Timer interface
    *   [ ] Critical section interface
*   [ ] **Task 3.3**: Create the `LoRaWAN Thread`.
    *   [ ] Initialize the modem.
    *   [ ] Handle the `join network` process.
    *   [ ] Implement the main `smtc_modem_run_engine()` loop.
*   [ ] **Task 3.4**: Implement periodic LoRaWAN uplinks.
    *   [ ] Read data from the shared structure.
    *   [ ] Format the uplink payload.
    *   [ ] Request the uplink.

## Phase 4: Location & Emergency Features

*   [ ] **Task 4.1**: Implement LR1110 scanning.
    *   [ ] Add interface for GNSS scanning.
    *   [ ] Add interface for Wi-Fi scanning.
    *   [ ] Integrate scanning results into the shared data structure.
*   [ ] **Task 4.2**: Implement the button handler for emergency mode.
    *   [ ] Configure a GPIO interrupt for the user button.
    *   [ ] On trigger, set the emergency flag in the shared data structure.
*   [ ] **Task 4.3**: Implement emergency logic in the LoRaWAN and Beacon threads.
    *   [ ] Beacon: Update advertising data to reflect SOS.
    *   [ ] LoRaWAN: Send an emergency uplink.

## Phase 5: Power Management & Finalization

*   [ ] **Task 5.1**: Implement power management strategies.
    *   [ ] Pause BLE advertising during LoRa Tx.
    *   [ ] Utilize CPU low-power states.
*   [ ] **Task 5.2**: Full integration testing and debugging.
*   [ ] **Task 5.3**: Code cleanup and final review. 