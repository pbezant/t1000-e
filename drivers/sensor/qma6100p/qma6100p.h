/*
 * Copyright (c) 2023 Seeed Studio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_QMA6100P_H_
#define ZEPHYR_DRIVERS_SENSOR_QMA6100P_H_

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

/* QMA6100P I2C address */
#define QMA6100P_ADDRESS                0x12

/* QMA6100P Chip ID */
#define QMA6100P_REG_CHIP_ID		    0x00
#define QMA6100P_CHIP_ID_VAL			0x90

/* QMA6100P Register Definitions */
#define QMA6100P_REG_XOUTL				0x01
#define QMA6100P_REG_XOUTH				0x02
#define QMA6100P_REG_YOUTL				0x03
#define QMA6100P_REG_YOUTH				0x04
#define QMA6100P_REG_ZOUTL				0x05
#define QMA6100P_REG_ZOUTH				0x06
#define QMA6100P_REG_STEP_CNT_L			0x07
#define QMA6100P_REG_STEP_CNT_M			0x08
#define QMA6100P_REG_INT_STATUS_0		0x09
#define QMA6100P_REG_INT_STATUS_1		0x0a
#define QMA6100P_REG_INT_STATUS_2		0x0b
#define QMA6100P_REG_INT_STATUS_3		0x0c
#define QMA6100P_REG_STEP_CNT_H			0x0d
#define QMA6100P_REG_FIFO_STATE			0x0e
#define QMA6100P_REG_RANGE				0x0f
#define QMA6100P_REG_BW_ODR				0x10
#define QMA6100P_REG_POWER_MANAGE		0x11
#define QMA6100P_REG_STEP_SAMPLE_CNT	0x12
#define QMA6100P_REG_STEP_PRECISION		0x13
#define QMA6100P_REG_STEP_TIME_LOW		0x14
#define QMA6100P_REG_STEP_TIME_UP		0x15
#define QMA6100P_REG_INT_EN_0			0x16
#define QMA6100P_REG_INT_EN_1			0x17
#define QMA6100P_REG_INT_EN_2			0x18
#define QMA6100P_REG_INT1_MAP_0			0x19
#define QMA6100P_REG_INT1_MAP_1			0x1a
#define QMA6100P_REG_INT2_MAP_0			0x1b
#define QMA6100P_REG_INT2_MAP_1			0x1c
#define QMA6100P_REG_INTPIN_CFG			0x20
#define QMA6100P_REG_INT_CFG			0x21
#define QMA6100P_REG_OS_CUST_X		    0x27
#define QMA6100P_REG_OS_CUST_Y			0x28
#define QMA6100P_REG_OS_CUST_Z			0x29
#define QMA6100P_REG_NVM				0x33
#define QMA6100P_REG_RESET				0x36


/* Range selection */
enum qma6100p_range {
	QMA6100P_RANGE_2G = 0x01,
	QMA6100P_RANGE_4G = 0x02,
	QMA6100P_RANGE_8G = 0x04,
	QMA6100P_RANGE_16G = 0x08,
	QMA6100P_RANGE_32G = 0x0f
};

/* Bandwidth selection */
enum qma6100p_bw {
	QMA6100P_BW_100 = 0,
	QMA6100P_BW_200 = 1,
	QMA6100P_BW_400 = 2,
	QMA6100P_BW_800 = 3,
	QMA6100P_BW_1600 = 4,
	QMA6100P_BW_50 = 5,
	QMA6100P_BW_25 = 6,
	QMA6100P_BW_12_5 = 7,
};

/* Power management modes */
enum qma6100p_mode {
	QMA6100P_MODE_STANDBY = 0,
	QMA6100P_MODE_ACTIVE = 1,
};

struct qma6100p_config {
    struct i2c_dt_spec i2c;
};

struct qma6100p_data {
    /* TBD: Store sensor readings here */
    int16_t ax, ay, az;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_QMA6100P_H_ */ 