/*
 * icm42688.h
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2025 by Rev-RoastedDuck.
 */

#ifndef PERIPHERAL_SUITE_ICM42688_RRD_H_
#define PERIPHERAL_SUITE_ICM42688_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"
#include "icm42688_reg.h"

/******************************************************************************/
/*------------------------------------Ops-------------------------------------*/
/******************************************************************************/
/* rrd_icm42688_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * spi_read_write     	| spi_context: SPI context pointer.
 *                     	| reg: register address.
 *                     	| tx_data: transmit buffer (NULL for read-only).
 *                     	| rx_data: receive buffer (NULL for write-only).
 *                     	| len: data length.
 * get_time_stamp_us     | context: base context pointer.
 * ****************************************************************************
 */
typedef struct rrd_icm42688_ops {
	void *spi_context;
	void (*spi_read_write)(void *spi_context, uint8_t reg, uint8_t *tx_data, uint8_t *rx_data, uint8_t len);

	void *base_context;
	rrd_get_timestamp_size_hook_t get_time_stamp_us;
} rrd_icm42688_ops_st;

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
typedef void (*rrd_icm42688_update_fn_t)(void *self);
typedef void (*rrd_icm42688_calibration_fn_t)(void *self);
typedef void (*rrd_icm42688_get_int_status_fn_t)(void *self, uint8_t *buff);

typedef struct rrd_icm42688_interface {
	rrd_icm42688_update_fn_t update;
	rrd_icm42688_calibration_fn_t calibration;
    rrd_icm42688_get_int_status_fn_t get_int_status;
} rrd_icm42688_interface_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_icm42688 {
	rrd_icm42688_interface_st *interface;
	rrd_icm42688_ops_st *ops;

	rrd_del_fn_t del;

	struct {
		float lsb_accel;
		float lsb_gyro;
		uint16_t gyro_offset_yaw;
		uint16_t calibration_count;
	} config;

	struct {
		float gyro_sum[3];
		float accel_sum[3];
		int32_t cycles_remaining;
	} calibration;

	float gyro[3];
	float accel[3];
	float gyro_adc[3];
	float accel_adc[3];
	float gyro_adc_zero[3];
	float accel_adc_zero[3];

	float temperature;
} rrd_icm42688_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_icm42688_st *rrd_icm42688_new(rrd_icm42688_ops_st *ops);
uint8_t rrd_icm42688_init(rrd_icm42688_st *self, rrd_icm42688_ops_st *ops);
/** \} */

/** \addtogroup Operation
 ** \{ */
void rrd_icm42688_update(rrd_icm42688_st *self);
void rrd_icm42688_calibration(rrd_icm42688_st *self);
/** \} */

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#define OPEN_RRD_ICM42688_TEST (0)
#if OPEN_RRD_ICM42688_TEST
void rrd_icm42688_test(rrd_icm42688_ops_st *ops);
#endif
#endif /* PERIPHERAL_SUITE_ICM42688_RRD_H_ */
