/*
 * spi_rrd.h
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2025 by Rev-RoastedDuck.
 */
#ifndef PERIPHERAL_SUITE_WS2812_RRD_H_
#define PERIPHERAL_SUITE_WS2812_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"

/******************************************************************************/
/*---------------------------------Constants----------------------------------*/
/******************************************************************************/
#define RRD_WS2812_DATA_LEN         (24)
#define RRD_WS2812_RESET_PERIOD_NUM (50)

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
typedef void (*rrd_ws2812_update_fn_t)(void *self);
typedef void (*rrd_ws2812_set_rgb_fn_t)(void *self, size_t led_index, uint8_t r, uint8_t g, uint8_t b);

typedef struct rrd_ws2812_interface {
	rrd_ws2812_update_fn_t update;
	rrd_ws2812_set_rgb_fn_t set_rgb;
} rrd_ws2812_interface_st;

/******************************************************************************/
/*------------------------------------Ops-------------------------------------*/
/******************************************************************************/
/* rrd_ws2812_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * set_pwm_duty       	| pwm_context: PWM context pointer.
 *                     	| data: duty cycle buffer.
 *                     	| len: buffer length.
 * ****************************************************************************
 */
typedef struct rrd_ws2812_ops {
	void *pwm_context;
	void (*set_pwm_duty)(void *pwm_context, uint16_t *data, size_t len);
} rrd_ws2812_ops_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_ws2812 {
	rrd_ws2812_interface_st *interface;
	rrd_ws2812_ops_st *ops;

	rrd_del_fn_t del;

	struct {
		size_t led_num;
		uint16_t pwm_low_counter;
		uint16_t pwm_heigh_counter;
	} config;

	uint16_t *rgb_buff;
	size_t rgb_buff_size;
} rrd_ws2812_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_ws2812_st *rrd_ws2812_new(size_t led_num, rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period);
uint8_t rrd_ws2812_init(rrd_ws2812_st *self, size_t led_num, rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period);
/** \} */

/** \addtogroup Operation
 ** \{ */
void rrd_ws2812_update(rrd_ws2812_st *self);
void rrd_ws2812_set_rgb(rrd_ws2812_st *self, size_t led_index, uint8_t r, uint8_t g, uint8_t b);
/** \} */

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#define OPEN_RRD_WS2812_TEST (0)
#if OPEN_RRD_WS2812_TEST
void rrd_ws2812_test(rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period);
#endif
#endif /* PERIPHERAL_SUITE_WS2812_RRD_H_ */
