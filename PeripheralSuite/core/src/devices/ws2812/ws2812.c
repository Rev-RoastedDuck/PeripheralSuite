/*
 * ws2812.c
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2025 by Rev-RoastedDuck.
 */

#include "ws2812.h"

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
void rrd_ws2812_set_rgb(rrd_ws2812_st *self, size_t led_index, uint8_t r, uint8_t g, uint8_t b)
{
	led_index = led_index * RRD_WS2812_DATA_LEN;

	for (uint8_t index = 0; index < 8; index++) {
		self->rgb_buff[led_index + index] = (g & (0x80 >> index)) ? self->config.pwm_heigh_counter
		                                                          : self->config.pwm_low_counter;

		self->rgb_buff[led_index + index + 8] = (r & (0x80 >> index)) ? self->config.pwm_heigh_counter
		                                                              : self->config.pwm_low_counter;

		self->rgb_buff[led_index + index + 16] = (b & (0x80 >> index)) ? self->config.pwm_heigh_counter
		                                                               : self->config.pwm_low_counter;
	}

	for (size_t index = self->config.led_num * RRD_WS2812_DATA_LEN; index < self->rgb_buff_size; ++index) {
		self->rgb_buff[index] = 0;
	}
}

void rrd_ws2812_update(rrd_ws2812_st *self)
{
	self->ops->set_pwm_duty(self->ops->pwm_context, self->rgb_buff, self->rgb_buff_size);
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_ws2812_interface_st g_rrd_ws2812_interface = {
	.update = (rrd_ws2812_update_fn_t)rrd_ws2812_update,
	.set_rgb = (rrd_ws2812_set_rgb_fn_t)rrd_ws2812_set_rgb,
};

static void rrd_ws2812_deinit(void *self)
{
	if (self == NULL) return;

	rrd_ws2812_st *self_t = self;
	if (self_t->rgb_buff != NULL) {
		free(self_t->rgb_buff);
	}
	memset(self_t, 0, sizeof(*self_t));
}

static void rrd_ws2812_destroy(void **self)
{
	if (NULL == self || NULL == *self) return;

	rrd_ws2812_st *self_t = (rrd_ws2812_st *)(*self);
	rrd_ws2812_deinit(self_t);
	free(self_t);
	*self = NULL;
}

uint8_t rrd_ws2812_init(rrd_ws2812_st *self, size_t led_num, rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period)
{
	if (NULL == self || NULL == ops || led_num == 0) return 1;

	memset(self, 0, sizeof(*self));

	self->ops = ops;
	self->interface = &g_rrd_ws2812_interface;
	self->del = (rrd_del_fn_t)rrd_ws2812_deinit;

	self->config.led_num = led_num;
	self->config.pwm_low_counter = (pwm_counter_period / 3) - 1;
	self->config.pwm_heigh_counter = (pwm_counter_period / 3) * 2;
	self->rgb_buff_size = led_num * RRD_WS2812_DATA_LEN + RRD_WS2812_RESET_PERIOD_NUM;
	self->rgb_buff = (uint16_t *)malloc(self->rgb_buff_size * sizeof(uint16_t));
	if (NULL == self->rgb_buff) return 1;

	return 0;
}

rrd_ws2812_st *rrd_ws2812_new(size_t led_num, rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period)
{
	rrd_ws2812_st *self;
	self = malloc(sizeof(*self));
	if (NULL == self) return NULL;

	uint8_t ret = rrd_ws2812_init(self, led_num, ops, pwm_counter_period);
	if (ret) goto init_failed;

	self->del = (rrd_del_fn_t)rrd_ws2812_destroy;
	return self;

init_failed:
	free(self);
	return NULL;
}

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#if OPEN_RRD_WS2812_TEST
void rrd_ws2812_test(rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period)
{
	rrd_ws2812_st *led = rrd_ws2812_new(1, ops, pwm_counter_period);
	if (NULL == led) return;

	led->interface->set_rgb(led, 0, 0, 0, 0);
	led->interface->update(led);

	led->interface->set_rgb(led, 0, 10, 10, 10);
	led->interface->update(led);

	led->del(&led);
}
#endif
