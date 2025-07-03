# T1000-E Tracker with iBeacon Integration

This is a modified version of the official Seeed Studio T1000-E LoRaWAN tracker example that adds **simultaneous iBeacon functionality**.

## 🎯 What This Does

**Original T1000-E Tracker Features:**
- ✅ Full LoRaWAN connectivity (OTAA/ABP)
- ✅ GNSS/GPS positioning
- ✅ Wi-Fi scanning for positioning
- ✅ BLE scanning for positioning  
- ✅ Sensor readings (battery, temperature, light, accelerometer)
- ✅ Configuration via BLE UART
- ✅ Power management and sleep modes

**NEW: Added iBeacon Features:**
- 🚨 **Simultaneous iBeacon advertising** alongside LoRaWAN
- 📱 **Dynamic device name** (`t1000-XXXXXX` from MAC address)
- 📊 **Sensor data in scan response** (battery, temp, light, accelerometer)
- 🆘 **Emergency mode** - button press changes beacon Major/Minor values
- ⚡ **Power management** - beacon pauses during LoRaWAN transmissions
- 🔄 **Real-time updates** - beacon data updates with sensor readings

## 📡 iBeacon Specification

### Standard iBeacon Advertisement
- **UUID:** `E2C56DB5-DFFB-48D2-B060-D0F5A71096E0`
- **Major:** `0x0001` (normal) / `0xFF00` (emergency)
- **Minor:** `0x0001` (normal) / `0x00XX` (emergency, XX = battery level)
- **TX Power:** `-59 dBm`
- **Advertising Interval:** 100ms

### Scan Response Data
- **Device Name:** `t1000-XXXXXX` (last 6 hex digits of MAC)
- **Manufacturer Data:** Custom format with sensor readings:
  ```
  Company ID: 0xFFEE (custom)
  Payload (12 bytes):
    - Battery level (1 byte, 0-100%)
    - Temperature (2 bytes, signed, 0.1°C resolution)
    - Light level (2 bytes, unsigned, lux)
    - Accelerometer X (2 bytes, signed)
    - Accelerometer Y (2 bytes, signed) 
    - Accelerometer Z (2 bytes, signed)
    - Emergency flag (1 byte, 0x00/0xFF)
  ```

## 🔧 Files Modified/Added

### New Files:
- `app_ble_beacon.h` - iBeacon API declarations
- `app_ble_beacon.c` - iBeacon implementation

### Modified Files:
- `main_lorawan_tracker.c` - Integrated iBeacon calls
- `main_lorawan_tracker.h` - No changes needed

## 🚀 How It Works

1. **Initialization:**
   - iBeacon module initializes alongside existing BLE
   - UUID and default values are set

2. **Network Join:**
   - After LoRaWAN network join, iBeacon advertising starts
   - Continues advertising indefinitely

3. **Sensor Updates:**
   - Every tracking cycle, sensor data is read
   - iBeacon scan response is updated with fresh data
   - Emergency state is checked and reflected in Major/Minor

4. **Power Management:**
   - iBeacon advertising pauses during LoRaWAN transmissions
   - Resumes immediately after TX completion
   - Reduces power consumption during radio conflicts

5. **Emergency Mode:**
   - Triggered by button press (existing functionality)
   - Changes iBeacon Major to `0xFF00`
   - Sets Minor to current battery level
   - Sends emergency LoRaWAN uplink (existing)

## 📱 Mobile App Integration

Your mobile app can scan for:

1. **iBeacon UUID** `E2C56DB5-DFFB-48D2-B060-D0F5A71096E0`
2. **Check Major/Minor** for emergency status
3. **Scan for device name** `t1000-*` to identify trackers
4. **Parse manufacturer data** for real-time sensor readings

Example iOS Core Location beacon monitoring:
```swift
let uuid = UUID(uuidString: "E2C56DB5-DFFB-48D2-B060-D0F5A71096E0")!
let beaconRegion = CLBeaconRegion(uuid: uuid, identifier: "T1000Tracker")
locationManager.startMonitoring(for: beaconRegion)
```

## 🔨 Building

This uses the **existing Nordic SDK + LoRa Basics Modem** build system (NOT Zephyr).

### Prerequisites:
- Nordic nRF5 SDK v17.1.0
- Segger Embedded Studio (SES)
- LoRa Basics Modem SDK (included)

### Build Steps:
1. Open the SES project file for your board:
   ```
   pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject
   ```

2. Add the new files to the project:
   - Right-click project → Add Existing File
   - Add `app_ble_beacon.h` and `app_ble_beacon.c`

3. Build and flash as normal

### Alternative: Command Line Build
```bash
# Navigate to the example directory
cd pca10056/s140/11_ses_lorawan_tracker/

# Build (if you have SES command line tools)
emBuild -project t1000_e_dev_kit_pca10056.emProject -config "Release"
```

## 🔧 Configuration

### LoRaWAN Settings
Configure in `lorawan_key_config.h` as usual:
- Device EUI, Application EUI, Application Key
- Region settings
- Join method (OTAA/ABP)

### iBeacon Settings
Modify in `app_ble_beacon.c`:
- `ibeacon_uuid[]` - Change the UUID
- `APP_ADV_FAST_INTERVAL` - Advertising interval
- Manufacturer data format

### Tracker Behavior
Configure in `main_lorawan_tracker.c`:
- `tracker_scan_type` - What sensors to use
- `tracker_periodic_interval` - How often to send data
- `gnss_scan_duration`, `wifi_scan_duration`, etc.

## 🐛 Troubleshooting

### "Bluetooth functions undefined"
- Make sure you're using the Nordic SDK build system, not Zephyr
- Verify SoftDevice S140 is included in the build

### "iBeacon not advertising"
- Check that LoRaWAN network join was successful
- iBeacon only starts after network join
- Check debug logs for error messages

### "Can't see scan response data"
- Make sure your scanner requests scan response packets
- Some BLE scanners only show advertisement data by default

### "Emergency mode not working"
- Verify button press triggers `TRACKER_STATE_BIT8_USER` event
- Check that `event_state` is being set correctly

## 📚 Integration Notes

- **Non-intrusive:** All existing T1000-E functionality preserved
- **Power efficient:** iBeacon pauses during LoRaWAN TX
- **Real-time:** Sensor data updates every tracking cycle
- **Emergency aware:** Button press immediately reflected in beacon
- **Mobile friendly:** Standard iBeacon + custom manufacturer data

## 🎉 Success!

You now have a **dual-mode tracker** that:
- ✅ Sends position/sensor data via LoRaWAN
- ✅ Broadcasts real-time data via iBeacon
- ✅ Supports emergency alerting on both protocols
- ✅ Maintains power efficiency
- ✅ Works with standard mobile apps

Perfect for applications requiring both long-range IoT connectivity and local mobile interaction! 