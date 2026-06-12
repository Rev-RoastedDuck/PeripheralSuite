/*
 * mt6816.c
 *	
 *  Created on: 2026_6_2
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2026 by Rev-RoastedDuck.
 */

#include "mt6816.h"

/******************************************************************************/
/*-----------------------------------Private----------------------------------*/
/******************************************************************************/
static inline bool _rrd_mt6816_parity_even(uint16_t data)
{
    data ^= data >> 8;
    data ^= data >> 4;
    data ^= data >> 2;
    data ^= data >> 1;
    return (~data) & 0x01;
}

static inline uint16_t _rrd_mt6816_spi_read_reg(rrd_mt6816_st *self, uint8_t reg)
{
    uint8_t tx_buf[2] = {(uint8_t)(RRD_MT6816_READ_CMD | reg), 0x00};
    uint8_t rx_buf[2] = {0};
    self->ops->write_read(self->ops->spi_context, tx_buf, rx_buf, sizeof(tx_buf));
    return ((uint16_t)rx_buf[0] << 8) | rx_buf[1];
}

static inline uint16_t _rrd_mt6816_get_raw_count(rrd_mt6816_st *self)
{
    uint16_t angle_data;

    angle_data  = (_rrd_mt6816_spi_read_reg(self, RRD_MT6816_REG_03) & 0x00FF) << 8;
    angle_data |= (_rrd_mt6816_spi_read_reg(self, RRD_MT6816_REG_04) & 0x00FF);

    self->raw.parity_error = !_rrd_mt6816_parity_even(angle_data);
    self->raw.no_magnet = (angle_data & RRD_MT6816_NO_MAGNET_MASK) != 0;
    if (self->raw.parity_error) return self->raw.single_turn_count;

    return angle_data >> 2;
}

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
void rrd_mt6816_update(rrd_mt6816_st *self, uint8_t opt)
{
    uint16_t single_turn_count = _rrd_mt6816_get_raw_count(self);
    int16_t raw_theta = (int16_t)(single_turn_count - self->raw.single_turn_count);

    if (raw_theta > RRD_MT6816_WRAP_THRESHOLD) {
        self->raw.revolutions -= 1;
    } else if (raw_theta < -RRD_MT6816_WRAP_THRESHOLD) {
        self->raw.revolutions += 1;
    }

    int32_t multi_turn_count = self->raw.revolutions * RRD_MT6816_CPR + single_turn_count;

    if (opt & RRD_MT6816_UPDATE_OPT_SPEED) {
        size_t now_us = self->ops->get_time_stamp_us(self->ops->base_context);
        size_t dt = now_us - self->raw._time_stamp_us;
        int32_t delta_count = multi_turn_count - self->raw.multi_turn_count;
        self->raw.speed = ((int64_t)delta_count * 1000000) / dt;
        self->raw._time_stamp_us = now_us;
    }

    self->raw.multi_turn_count = multi_turn_count;
    self->raw.single_turn_count = single_turn_count;
}

void rrd_mt6816_update_fx(rrd_mt6816_st *self, uint8_t opt)
{
    self->fx.single_turn_q16 = self->raw.single_turn_count << 2;
    self->fx.multi_turn_q16  = (self->raw.revolutions << 16) + self->fx.single_turn_q16;
}

void rrd_mt6816_update_fp(rrd_mt6816_st *self, uint8_t opt)
{
    #define RRD_MT6816_2PI_F             (6.28318530718f)
    #define RRD_MT6816_COUNT_TO_RAD_F    (RRD_MT6816_2PI_F / RRD_MT6816_CPR)

    self->fp.multi_turn_radian  = (float)self->raw.multi_turn_count  * RRD_MT6816_COUNT_TO_RAD_F;
    self->fp.single_turn_radian = (float)self->raw.single_turn_count * RRD_MT6816_COUNT_TO_RAD_F;
    if (opt & RRD_MT6816_UPDATE_OPT_SPEED) self->fp.speed = (float)self->raw.speed * RRD_MT6816_COUNT_TO_RAD_F;

    #undef RRD_MT6816_COUNT_TO_RAD_F
    #undef RRD_MT6816_2PI_F
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_mt6816_interface_st g_rrd_mt6816_interface = {
    .update    = (rrd_mt6816_update_fn_t)rrd_mt6816_update,
    .update_fx = (rrd_mt6816_update_fn_t)rrd_mt6816_update_fx,
    .update_fp = (rrd_mt6816_update_fn_t)rrd_mt6816_update_fp,
};

void rrd_mt6816_deinit_rrd(void *self)
{
    if (self == NULL) return;

    rrd_mt6816_st *self_t = self;
    memset(self_t, 0, sizeof(*self_t));
}

void rrd_mt6816_destroy_rrd(void **self)
{
    if (NULL == self || NULL == *self) return;

    rrd_mt6816_st *self_t = (rrd_mt6816_st *)(*self);
    rrd_mt6816_deinit_rrd(self_t);
    free(self_t);
    *self = NULL;
}

uint8_t rrd_mt6816_init(rrd_mt6816_st *self, rrd_mt6816_ops_st *ops)
{
    if (NULL == self || NULL == ops) return 1;

    memset(self, 0, sizeof(*self));

    self->ops = ops;
    self->interface = &g_rrd_mt6816_interface;
    self->del = (rrd_del_fn_t)rrd_mt6816_deinit_rrd;

    self->raw.revolutions = 0;
    self->raw._time_stamp_us = self->ops->get_time_stamp_us(self->ops->base_context);
    while (self->ops->get_time_stamp_us(self->ops->base_context) < self->raw._time_stamp_us + 10) {
        _rrd_mt6816_get_raw_count(self);
    }

    self->raw._time_stamp_us = self->ops->get_time_stamp_us(self->ops->base_context);
    self->raw.single_turn_count = _rrd_mt6816_get_raw_count(self);
    self->raw.multi_turn_count = (int32_t)self->raw.single_turn_count;
    self->raw.speed = 0;

    return 0;
}

rrd_mt6816_st *rrd_mt6816_new(rrd_mt6816_ops_st *ops)
{
    rrd_mt6816_st *self;
    self = malloc(sizeof(*self));
    if (NULL == self) return NULL;

    uint8_t ret = rrd_mt6816_init(self, ops);
    if (ret) goto init_failed;

    self->del = (rrd_del_fn_t)rrd_mt6816_destroy_rrd;
    return self;

init_failed:
    free(self);
    return NULL;
}


/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#if OPEN_MT6816_TEST_RRD
void rrd_mt6816_test(rrd_mt6816_ops_st *ops)
{
    rrd_mt6816_st mt6816_dev;
    rrd_mt6816_init(&mt6816_dev, ops);
    while (1) {
        rrd_mt6816_update(&mt6816_dev, RRD_MT6816_UPDATE_OPT_SPEED);
        rrd_mt6816_update_fp(&mt6816_dev, RRD_MT6816_UPDATE_OPT_SPEED);
        printf("%0.3f, %0.3f\r\n", mt6816_dev.fp.multi_turn_radian, mt6816_dev.fp.single_turn_radian);
        delay_1ms(2);
    }
}
#endif
