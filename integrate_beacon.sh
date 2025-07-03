#!/bin/bash

echo "🚀 Integrating iBeacon code into T1000-E project..."

# Define paths
TRACKER_SRC="Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/t1000_e/tracker/src"
TRACKER_INC="Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/t1000_e/tracker/inc"
EXAMPLE_DIR="Seeed-Tracker-T1000-E-for-LoRaWAN-dev-board/apps/examples/11_lorawan_tracker"

# Copy beacon files to tracker source
echo "📁 Copying beacon files..."
cp tracker_with_beacon/app_ble_beacon.c "$TRACKER_SRC/"
cp tracker_with_beacon/app_ble_beacon.h "$TRACKER_INC/"

# Replace main file
echo "🔄 Updating main tracker file..."
cp tracker_with_beacon/main_lorawan_tracker.c "$EXAMPLE_DIR/"

# Copy documentation
cp tracker_with_beacon/README_BEACON_INTEGRATION.md "$EXAMPLE_DIR/"

echo "✅ Integration complete!"
echo ""
echo "📋 Next steps:"
echo "1. Install Segger Embedded Studio (free): https://www.segger.com/downloads/embedded-studio/"
echo "2. Open: $EXAMPLE_DIR/../../../pca10056/s140/11_ses_lorawan_tracker/t1000_e_dev_kit_pca10056.emProject"
echo "3. Add app_ble_beacon.c and app_ble_beacon.h to the project"
echo "4. Build with F7, Flash with F5"
echo ""
echo "🎯 Your T1000-E now has iBeacon functionality!" 