/*
 * Copyright (c) 2023 Seeed Studio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT qitas_qma6100p

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/__assert.h>
#include "qma6100p.h"

#define LOG_LEVEL CONFIG_SENSOR_LOG_LEVEL
#include <zephyr/log/log.h>
LOG_MODULE_REGISTER(QMA6100P, LOG_LEVEL);

static int qma_write_reg(const struct device *dev, uint8_t reg, uint8_t val)
{
    const struct qma6100p_config *config = dev->config;
    return i2c_reg_write_byte_dt(&config->i2c, reg, val);
}

static int qma_read_reg(const struct device *dev, uint8_t reg, uint8_t *val)
{
    const struct qma6100p_config *config = dev->config;
    return i2c_reg_read_byte_dt(&config->i2c, reg, val);
}

static int qma6100p_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    struct qma6100p_data *data = dev->data;
    const struct qma6100p_config *config = dev->config;
    uint8_t buf[6];

    if (i2c_burst_read_dt(&config->i2c, QMA6100P_REG_XOUTL, buf, 6) < 0) {
        LOG_ERR("Failed to read sample data");
        return -EIO;
    }

    data->ax = sys_le16_to_cpu(buf[0] | (buf[1] << 8)) >> 2;
    data->ay = sys_le16_to_cpu(buf[2] | (buf[3] << 8)) >> 2;
    data->az = sys_le16_to_cpu(buf[4] | (buf[5] << 8)) >> 2;

    return 0;
}

static void qma6100p_accel_convert(struct sensor_value *val, int16_t raw_val)
{
	/* 8G range, 14-bit resolution. 1g = 9.80665 m/s^2 */
    /* Sensitivity is 4096 LSB/g for 8G range */
	int64_t micro_ms2 = (int64_t)raw_val * SENSOR_G / 4096;

	val->val1 = micro_ms2 / 1000000;
	val->val2 = micro_ms2 % 1000000;
}

static int qma6100p_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val)
{
    struct qma6100p_data *data = dev->data;

    if (chan == SENSOR_CHAN_ACCEL_X) {
        qma6100p_accel_convert(val, data->ax);
    } else if (chan == SENSOR_CHAN_ACCEL_Y) {
        qma6100p_accel_convert(val, data->ay);
    } else if (chan == SENSOR_CHAN_ACCEL_Z) {
        qma6100p_accel_convert(val, data->az);
    } else if (chan == SENSOR_CHAN_ACCEL_XYZ) {
        qma6100p_accel_convert(&val[0], data->ax);
        qma6100p_accel_convert(&val[1], data->ay);
        qma6100p_accel_convert(&val[2], data->az);
    } else {
        return -ENOTSUP;
    }

    return 0;
}

static int qma6100p_init(const struct device *dev)
{
    const struct qma6100p_config *config = dev->config;
    uint8_t chip_id = 0;

    if (!device_is_ready(config->i2c.bus)) {
		LOG_ERR("I2C bus device not ready");
		return -ENODEV;
	}

    if (qma_read_reg(dev, QMA6100P_REG_CHIP_ID, &chip_id) < 0) {
        LOG_ERR("Failed to read chip ID");
        return -EIO;
    }

    if (chip_id != QMA6100P_CHIP_ID_VAL) {
        LOG_ERR("Invalid chip ID: 0x%02x", chip_id);
        return -EINVAL;
    }

    LOG_INF("QMA6100P chip ID OK");

    /* Reset sequence from original driver */
    qma_write_reg(dev, QMA6100P_REG_RESET, 0xb6);
    k_msleep(5);
    qma_write_reg(dev, QMA6100P_REG_RESET, 0x00);
    k_msleep(10);
    
    /* Put into active mode */
    qma_write_reg(dev, QMA6100P_REG_POWER_MANAGE, 0x80);
    k_msleep(2);

    /* Set range to 8G and bandwidth to 100Hz */
    qma_write_reg(dev, QMA6100P_REG_RANGE, QMA6100P_RANGE_8G);
    qma_write_reg(dev, QMA6100P_REG_BW_ODR, QMA6100P_BW_100);

    return 0;
}


static const struct sensor_driver_api qma6100p_driver_api = {
    .sample_fetch = qma6100p_sample_fetch,
    .channel_get = qma6100p_channel_get,
};

#define QMA6100P_INIT(inst)                                           \
    static struct qma6100p_data qma6100p_data_##inst;                 \
    static const struct qma6100p_config qma6100p_config_##inst = {   \
        .i2c = I2C_DT_SPEC_INST_GET(inst),                          \
    };                                                              \
                                                                    \
    DEVICE_DT_INST_DEFINE(inst, qma6100p_init, NULL,                 \
                          &qma6100p_data_##inst, &qma6100p_config_##inst, \
                          POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, \
                          &qma6100p_driver_api);

DT_INST_FOREACH_STATUS_OKAY(QMA6100P_INIT) 