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
void rrd_ws2812_set_rgb(rrd_ws2812_st *self, size_t led_index, rrd_ws2812_rgb_st rgb_st)
{
	led_index = led_index * RRD_WS2812_DATA_LEN;

	for (uint8_t index = 0; index < 8; index++) {
		self->rgb_buff[led_index + index] = (rgb_st.g & (0x80 >> index)) 
												? self->config.pwm_heigh_counter
		                                       	: self->config.pwm_low_counter;

		self->rgb_buff[led_index + index + 8] = (rgb_st.r & (0x80 >> index)) 
													? self->config.pwm_heigh_counter
		                                           	: self->config.pwm_low_counter;

		self->rgb_buff[led_index + index + 16] = (rgb_st.b & (0x80 >> index)) 
													? self->config.pwm_heigh_counter
		                                        	: self->config.pwm_low_counter;
	}

	for (size_t index = self->config.led_num * RRD_WS2812_DATA_LEN; index < self->rgb_buff_size; ++index) {
		self->rgb_buff[index] = 0;
	}
}

void rrd_ws2812_update(rrd_ws2812_st *self)
{
	self->ops->set_pwm_cv(self->ops->pwm_context, self->rgb_buff, self->rgb_buff_size);
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
static void _rrd_ws2812_test_rainbow(rrd_ws2812_rgb_st *led, uint8_t *hue, uint8_t speed, uint8_t brightness)
{
    *hue += speed;

    uint8_t r, g, b;
    uint8_t region = *hue / 43;

    switch (region) {
        case 0:
            r = 255;
            g = (*hue * 6) % 256;
            b = 0;
            break;
        case 1:
            r = (255 - ((*hue - 43) * 6) % 256);
            g = 255;
            b = 0;
            break;
        case 2:
            r = 0;
            g = 255;
            b = ((*hue - 86) * 6) % 256;
            break;
        case 3:
            r = 0;
            g = (255 - ((*hue - 129) * 6) % 256);
            b = 255;
            break;
        case 4:
            r = ((*hue - 172) * 6) % 256;
            g = 0;
            b = 255;
            break;
        default:
            r = 255;
            g = 0;
            b = (255 - ((*hue - 215) * 6) % 256);
            break;
    }

    led->r = (r * brightness) >> 8;
    led->g = (g * brightness) >> 8;
    led->b = (b * brightness) >> 8;
}

void delay_1ms(uint32_t count);
void rrd_ws2812_test(rrd_ws2812_ops_st *ops, uint16_t pwm_counter_period)
{
    rrd_ws2812_st *led = rrd_ws2812_new(1, ops, pwm_counter_period);
    if (NULL == led) return;

    // 全灭
    led->interface->set_rgb(led, 0, (rrd_ws2812_rgb_st){0, 0, 0});
    led->interface->update(led);
    delay_1ms(200);

    // 各色闪一次
    led->interface->set_rgb(led, 0, (rrd_ws2812_rgb_st){50, 0, 0});
    led->interface->update(led);
    delay_1ms(200);
    led->interface->set_rgb(led, 0, (rrd_ws2812_rgb_st){0, 50, 0});
    led->interface->update(led);
    delay_1ms(200);
    led->interface->set_rgb(led, 0, (rrd_ws2812_rgb_st){0, 0, 50});
    led->interface->update(led);
    delay_1ms(200);
    led->interface->set_rgb(led, 0, (rrd_ws2812_rgb_st){0, 0, 0});
    led->interface->update(led);
    delay_1ms(200);

    // 彩虹循环效果
    uint8_t hue = 0;
    rrd_ws2812_rgb_st led_rgb;
    while (1) {
        _rrd_ws2812_test_rainbow(&led_rgb, &hue, 1, 50);
        led->interface->set_rgb(led, 0, led_rgb);
        led->interface->update(led);
        delay_1ms(20);
    }
}
#endif
