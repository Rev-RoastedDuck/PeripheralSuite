/*
 * as5600.c
 *	
 *  Created on: 2026_5_16
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "as5600.h"

/******************************************************************************/
/*-----------------------------------Private----------------------------------*/
/******************************************************************************/
static inline uint16_t _rrd_as5600_get_raw_count(rrd_as5600_st *self)
{
	uint8_t data_list[2];
    self->ops->iic_read_buffer(self->ops->iic_context, RRD_AS5600_REG_RAW_ANGLE, data_list, 2);
    return data_list[0]<<8 | data_list[1];
}

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
void rrd_as5600_update(rrd_as5600_st *self, uint8_t opt)
{
	uint16_t single_turn_count = _rrd_as5600_get_raw_count(self);
    int16_t raw_theta = (int16_t)(single_turn_count - self->raw.single_turn_count);

    if(raw_theta > RRD_AS5600_WRAP_THRESHOLD){
        self->raw.revolutions -= 1;
    }else if(raw_theta < -RRD_AS5600_WRAP_THRESHOLD){
        self->raw.revolutions += 1;
    }

    int32_t multi_turn_count = self->raw.revolutions * RRD_AS5600_CPR + single_turn_count;

    if(opt & RRD_AS5600_UPDATE_OPT_SPEED){
        size_t now_us = self->ops->get_time_stamp_us(self->ops->base_context);
        size_t dt = now_us - self->raw._time_stamp_us;
        int32_t delta_count = multi_turn_count - self->raw.multi_turn_count;
        self->raw.speed = ((int64_t)delta_count * 1000000) / dt;
        self->raw._time_stamp_us = now_us;
    }

    self->raw.multi_turn_count = multi_turn_count;
    self->raw.single_turn_count = single_turn_count;
}

void rrd_as5600_update_fx(rrd_as5600_st *self, uint8_t opt)
{
    self->fx.single_turn_q16 = self->raw.single_turn_count << 4;
    self->fx.multi_turn_q16  = (self->raw.revolutions << 16) + self->fx.single_turn_q16;
}

void rrd_as5600_update_fp(rrd_as5600_st *self, uint8_t opt)
{
    #define RRD_AS5600_2PI_F             (6.28318530718f)
    #define RRD_AS5600_COUNT_TO_RAD_F    (RRD_AS5600_2PI_F / RRD_AS5600_CPR)

    self->fp.multi_turn_radian  = (float)self->raw.multi_turn_count  * RRD_AS5600_COUNT_TO_RAD_F;
    self->fp.single_turn_radian = (float)self->raw.single_turn_count * RRD_AS5600_COUNT_TO_RAD_F;
    if(opt & RRD_AS5600_UPDATE_OPT_SPEED) self->fp.speed = (float)self->raw.speed * RRD_AS5600_COUNT_TO_RAD_F;

    #undef RRD_AS5600_COUNT_TO_RAD_F
    #undef RRD_AS5600_2PI_F
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_as5600_interface_st g_rrd_as5600_interface = {
	.update = (rrd_as5600_update_fn_t)rrd_as5600_update,
	.update_fx = (rrd_as5600_update_fn_t)rrd_as5600_update_fx,
	.update_fp = (rrd_as5600_update_fn_t)rrd_as5600_update_fp,
};

void rrd_as5600_deinit_rrd(void *self)
{
    if(self == NULL) return;

    rrd_as5600_st *self_t = self;
    memset(self_t, 0, sizeof(*self_t));
}

void rrd_as5600_destroy_rrd(void **self)
{
    if(NULL == self || NULL == *self) return;

    rrd_as5600_st *self_t = (rrd_as5600_st *)(*self);
    rrd_as5600_deinit_rrd(self_t);
    free(self_t);
    *self = NULL;
}

uint8_t rrd_as5600_init(rrd_as5600_st *self, rrd_as5600_ops_st *ops)
{
	if(NULL == self || NULL == ops) return 1;

    memset(self, 0, sizeof(*self));

	self->ops = ops;
	self->interface = &g_rrd_as5600_interface;
	self->del = (rrd_del_fn_t)rrd_as5600_deinit_rrd;

	self->raw.revolutions = 0;
	self->raw._time_stamp_us = self->ops->get_time_stamp_us(self->ops->base_context);
	while (self->ops->get_time_stamp_us(self->ops->base_context) < self->raw._time_stamp_us + 10){
		_rrd_as5600_get_raw_count(self);
	}

    self->raw._time_stamp_us = self->ops->get_time_stamp_us(self->ops->base_context);
	self->raw.single_turn_count = _rrd_as5600_get_raw_count(self);
    self->raw.multi_turn_count = (int32_t)self->raw.single_turn_count;
    self->raw.speed = 0;

    return 0;
}

rrd_as5600_st *rrd_as5600_new(rrd_as5600_ops_st *ops)
{
	rrd_as5600_st *self;
	self = malloc(sizeof(*self));
	if(NULL == self) return NULL;

    uint8_t ret = rrd_as5600_init(self, ops);
    if(ret) goto init_failed;

    self->del = (rrd_del_fn_t)rrd_as5600_destroy_rrd;
	return self;

init_failed:
	free(self);
	return NULL;
}

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
