/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/__assert.h>
#include "qma6100p.h"

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL

/* Timer for periodic sensor reading */
static void sensor_read_timer_handler(struct k_timer *dummy);
K_TIMER_DEFINE(sensor_read_timer, sensor_read_timer_handler, NULL);

/* ADC device */
#define ADC_NODE DT_ALIAS(adc)
#if !DT_NODE_HAS_STATUS(ADC_NODE, okay)
#error "No ADC device found"
#endif

static const struct adc_dt_spec adc_channels[] = {
	DT_IO_CHANNELS_GET(ADC_NODE)
};

/* Accelerometer device */
static const struct device *accel_dev = DEVICE_DT_GET(DT_ALIAS(accel0));

/*
 * Set up iBeacon advertisement data.
 *
 * This data contains the Apple-specific manufacturer data format for iBeacons.
 * UUID: E2C56DB5-DFFB-48D2-B060-D0F5A71096E0
 * Major: 1
 * Minor: 1
 * Measured Power (RSSI at 1m): -59 dBm
 */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
		      0x4c, 0x00, /* Apple */
		      0x02, 0x15, /* iBeacon */
		      0xe2, 0xc5, 0x6d, 0xb5, 0xdf, 0xfb, 0x48, 0xd2, /* UUID[15..8] */
		      0xb0, 0x60, 0xd0, 0xf5, 0xa7, 0x10, 0x96, 0xe0, /* UUID[7..0] */
		      0x00, 0x01, /* Major */
		      0x00, 0x01, /* Minor */
		      0xc5)      /* Measured Power */
};

/* Set up scan response data */
static uint8_t dev_name[sizeof("t1000-XXXXXX")] = "t1000-";
static struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, dev_name, sizeof(dev_name) -1),
};

static void bt_ready(void)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;
	int err;

	/* Get the device address */
	bt_id_get(&addr, &count);

	/* Format the device name to t1000-[last 6 of mac] */
	snprintf(dev_name + 6, 7, "%02X%02X%02X", addr.a.val[2], addr.a.val[1], addr.a.val[0]);

	err = bt_le_adv_start(BT_LE_ADV_SCAN_IND, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

    bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));
	printk("Beacon started, advertising as %s with address %s\n", dev_name, addr_s);
}

static void sensor_read_timer_handler(struct k_timer *dummy)
{
    uint16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};
    int32_t val_mv;
    struct sensor_value accel[3];

    printk("\n--- Sensor Readings ---\n");

    /* Read battery, light, and temp ADC channels */
    for (int i = 0; i < ARRAY_SIZE(adc_channels); i++) {
        (void)adc_sequence_init_dt(&adc_channels[i], &sequence);

		int err = adc_read(adc_channels[i].dev, &sequence);
		if (err < 0) {
			printk("Could not read ADC channel %d (%d)\n", i, err);
			continue;
		}

        val_mv = (int32_t)buf;
        adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);

        /* Note: a real application would convert these mV to useful units */
		printk("ADC %d (%s): %d mV\n", i, adc_channels[i].dev->name, val_mv);
	}

    /* Read accelerometer */
    if (sensor_sample_fetch(accel_dev) < 0) {
        printk("Failed to fetch sample from accelerometer\n");
    } else {
        sensor_channel_get(accel_dev, SENSOR_CHAN_ACCEL_XYZ, accel);
        printk("Accel X: %d.%06d m/s^2\n", accel[0].val1, accel[0].val2);
        printk("Accel Y: %d.%06d m/s^2\n", accel[1].val1, accel[1].val2);
        printk("Accel Z: %d.%06d m/s^2\n", accel[2].val1, accel[2].val2);
    }

    printk("-----------------------\n");
}

void main(void)
{
	int err;

	printk("Starting T1000-E Tracker\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
	bt_ready();
    printk("Bluetooth initialized\n");

    /* Verify ADC channels are ready */
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!device_is_ready(adc_channels[i].dev)) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return;
		}
	}

    /* Verify accelerometer is ready */
    if (!device_is_ready(accel_dev)) {
        printk("Accelerometer device not ready\n");
        return;
    }

    /* Start periodic timer for sensor reads */
    k_timer_start(&sensor_read_timer, K_SECONDS(5), K_SECONDS(10));
    printk("Started sensor read timer.\n");
}
