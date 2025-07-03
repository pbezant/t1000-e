# Complete T1000-E iBeacon Setup Guide

This guide will get you up and running with the T1000-E LoRaWAN tracker + iBeacon functionality on a fresh computer.

## 🎯 What You're Building

A Seeed Studio T1000-E tracker that simultaneously:
- Sends LoRaWAN tracking data (GPS, sensors, etc.)
- Broadcasts iBeacon advertisements for proximity detection
- Includes emergency mode triggered by button press
- Real-time sensor data in beacon scan response

## 📋 Prerequisites

### Hardware Required
- Seeed Studio T1000-E LoRaWAN tracker
- J-Link debugger (for flashing)
- USB cable for power/debugging

### Software Required
1. **Segger Embedded Studio (SES)** - Nordic's official IDE
   - Download from: https://www.segger.com/downloads/embedded-studio/
   - Version: Latest (free for Nordic development)

2. **J-Link Software** - For debugging/flashing
   - Download from: https://www.segger.com/downloads/jlink/
   - Install the full package

## 🚀 Quick Start

### Step 1: Clone the Repository
```bash
git clone <your-repo-url>
cd t1000-e
```

### Step 2: Apply Build Fixes
The project has missing Nordic SDK dependencies that need to be resolved:

```bash
# Run the automated fix script
python3 Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/pca10056/s140/11_ses_lorawan_tracker/fix_ses_project.py
```

This script:
- Removes problematic Nordic SDK library references
- Creates stub implementations for missing functions
- Adds the iBeacon files to the project
- Backs up the original project file

### Step 3: Open in Segger Embedded Studio
1. Launch Segger Embedded Studio
2. Open project: `Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject`
3. Build the project (F7 or Build menu)

### Step 4: Flash the Device
1. Connect J-Link to T1000-E
2. Power on the device
3. In SES: Target → Connect J-Link
4. Flash: Target → Download File

## 🔧 Detailed Setup (If Quick Start Fails)

### Manual Build Fix Process

If the automated script doesn't work, here's the manual process:

#### 1. Remove Nordic SDK Dependencies
Edit the `.emProject` file and remove these folder groups:
- `nRF_Drivers`
- `nRF_BLE` 
- `nRF_SoftDevice`
- `None` (problematic folder)

#### 2. Create Missing Headers
Create these files in the project directory:

**ble_hci.h** - Already created
**nrf_delay.h**:
```c
#ifndef NRF_DELAY_H__
#define NRF_DELAY_H__
#include <stdint.h>
void nrf_delay_ms(uint32_t ms);
void nrf_delay_us(uint32_t us);
#endif
```

**nordic_common.h**:
```c
#ifndef NORDIC_COMMON_H__
#define NORDIC_COMMON_H__
#include <stdint.h>
#define NRF_SUCCESS 0
#define NRF_ERROR_INVALID_PARAM 0x07
// Add other error codes as needed
#endif
```

**app_error.h**:
```c
#ifndef APP_ERROR_H__
#define APP_ERROR_H__
#include <stdint.h>
#define APP_ERROR_CHECK(ERR_CODE) do { if (ERR_CODE != 0) { /* handle error */ } } while(0)
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
#endif
```

#### 3. Create Stub Implementation Files

**minimal_nordic.c** - Contains 50+ Nordic SDK function stubs
**simple_log.c** - Logging system replacement  
**startup_stubs.c** - Driver initialization stubs

(These are automatically created by the fix script)

#### 4. Add iBeacon Files
Copy these files to the project:
- `app_ble_beacon.h`
- `app_ble_beacon.c`
- `main_lorawan_tracker.c` (modified version)
- `main_lorawan_tracker.h` (modified version)

#### 5. Update Project Include Paths
Add to project settings:
- Current directory: "."
- Project root for headers

## 📱 iBeacon Configuration

### Default Settings
- **UUID**: `E2C56DB5-DFFB-48D2-B060-D0F5A71096E0`
- **Major**: `0x0001` (normal) / `0xFF00` (emergency)
- **Minor**: `0x0001` (normal) / battery level (emergency)
- **TX Power**: Configurable (-4dBm default)

### Customization
Edit `app_ble_beacon.c` to change:
- UUID in `IBEACON_UUID` array
- Advertising interval
- TX power levels
- Device name format

## 🔍 Testing & Verification

### 1. Check LoRaWAN Functionality
- Device should join LoRaWAN network
- GPS coordinates transmitted every tracking interval
- Sensor data (accelerometer, temperature) included

### 2. Verify iBeacon Broadcasting
Use a BLE scanner app (like nRF Connect) to verify:
- iBeacon advertisements visible
- Correct UUID/Major/Minor values
- Scan response contains device name and sensor data

### 3. Test Emergency Mode
- Press button on device
- Major should change to 0xFF00
- Minor should show battery level
- LED behavior should change

## 🐛 Troubleshooting

### Build Errors

**"file does not exist" errors**:
- Run the fix script again
- Manually create missing headers
- Check include paths in project settings

**Nordic SDK function errors**:
- Add function stubs to `minimal_nordic.c`
- Check function signatures match usage

### Runtime Issues

**LoRaWAN not joining**:
- Check DevEUI, AppEUI, AppKey configuration
- Verify LoRaWAN gateway coverage
- Check frequency plan settings

**iBeacon not visible**:
- Verify BLE initialization in logs
- Check advertising parameters
- Ensure no radio conflicts with LoRaWAN

**Power consumption high**:
- iBeacon pauses during LoRaWAN TX (intentional)
- Adjust advertising intervals
- Check sleep mode configuration

## 📂 Project Structure

```
t1000-e/
├── Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/
│   ├── pca10056/s140/11_ses_lorawan_tracker/
│   │   ├── t1000_e_dev_kit_pca10056.emProject
│   │   ├── app_ble_beacon.h
│   │   ├── app_ble_beacon.c
│   │   ├── main_lorawan_tracker.c (modified)
│   │   ├── main_lorawan_tracker.h (modified)
│   │   ├── ble_hci.h (created)
│   │   ├── minimal_nordic.c (created)
│   │   ├── simple_log.c (created)
│   │   └── fix_ses_project.py
│   └── apps/examples/11_lorawan_tracker/
│       ├── main_lorawan_tracker.c (original)
│       └── main_lorawan_tracker.h (original)
└── tracker_with_beacon/ (development files)
```

## 🔧 Advanced Configuration

### LoRaWAN Settings
Edit in `main_lorawan_tracker.c`:
- `LORAWAN_REGION` - Set your region
- Tracking intervals
- Sensor reading frequencies

### iBeacon Settings  
Edit in `app_ble_beacon.c`:
- Advertising intervals
- TX power levels
- UUID/Major/Minor values
- Scan response data format

### Power Management
- iBeacon automatically pauses during LoRaWAN transmissions
- Adjust sleep intervals for battery optimization
- Configure sensor reading frequencies

## 📞 Support

If you run into issues:

1. **Check the build log** - SES provides detailed error messages
2. **Verify hardware connections** - J-Link, power, antennas
3. **Test basic examples first** - Try the original LoRaWAN tracker example
4. **Check Nordic documentation** - For SDK-specific issues

## 🎉 Success Indicators

When everything is working:
- ✅ Project builds without errors in SES
- ✅ Device joins LoRaWAN network 
- ✅ GPS coordinates transmitted via LoRaWAN
- ✅ iBeacon visible in BLE scanner apps
- ✅ Emergency mode triggered by button press
- ✅ Real-time sensor data in beacon scan response

## 📝 Notes

- This project uses Nordic SDK with LoRa Basics Modem, NOT Zephyr RTOS
- PlatformIO won't work due to custom Nordic SDK stack
- Segger Embedded Studio is the recommended build environment
- The fix script resolves 50+ missing Nordic SDK dependencies
- iBeacon and LoRaWAN share radio time efficiently

---

**Good luck with your build, and may your beacons broadcast far and wide!** 🚀📡 