/*
 * ntc.c
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "ntc.h"

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/**
 * @brief 线性插值查找温度
 * @return 温度(unit: ℃)
*/
float rrd_ntc_update_temperature(rrd_ntc_st *self, uint16_t raw_value){
    float resistance_k = (self->config.bot_resistance_k 
                            / (raw_value / self->config.adc_full_scale_count))
                            - self->config.bot_resistance_k;
    if(resistance_k >= self->config.ntc_table[0].resistance_k){
        self->params.temperature = self->config.ntc_table[0].temperature;
        return self->params.temperature;
    }else if(resistance_k <= self->config.ntc_table[self->config.ntc_table_length - 1].resistance_k){
        self->params.temperature = self->config.ntc_table[self->config.ntc_table_length - 1].temperature;
        return self->params.temperature;
    }

    for(uint8_t i = 0; i < self->config.ntc_table_length - 1; ++i){
        if(resistance_k <= self->config.ntc_table[i].resistance_k && 
           resistance_k >= self->config.ntc_table[i + 1].resistance_k){
            float temp_diff = self->config.ntc_table[i + 1].temperature - self->config.ntc_table[i].temperature;
            float res_diff = self->config.ntc_table[i].resistance_k - self->config.ntc_table[i + 1].resistance_k;
            float res_ratio = (self->config.ntc_table[i].resistance_k - resistance_k) / res_diff;
            self->params.temperature = self->config.ntc_table[i].temperature + temp_diff * res_ratio;
            return self->params.temperature;
        }
    }

    return self->params.temperature;
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_ntc_interface_st g_rrd_ntc_interface = {
	.update = (rrd_ntc_update_fn_t)rrd_ntc_update_temperature,
};

static void rrd_ntc_deinit(void *self)
{
	if (self == NULL) return;

	rrd_ntc_st *self_t = self;
	memset(self_t, 0, sizeof(*self_t));
}

static void rrd_ntc_destroy(void **self)
{
	if (NULL == self || NULL == *self) return;

	rrd_ntc_st *self_t = (rrd_ntc_st *)(*self);
	rrd_ntc_deinit(self_t);
	free(self_t);
	*self = NULL;
}

uint8_t rrd_ntc_init(
    rrd_ntc_st *self, 
    uint16_t bot_resistance_k, 
    uint16_t adc_full_scale_count,
    rrd_ntc_table_item_st *ntc_table, uint8_t ntc_table_length)
{
	if (NULL == self || NULL == ntc_table) return 1;
	memset(self, 0, sizeof(*self));

	self->interface = &g_rrd_ntc_interface;
	self->del = (rrd_del_fn_t)rrd_ntc_deinit;

	self->config.ntc_table = ntc_table;
	self->config.ntc_table_length = ntc_table_length;
	self->config.bot_resistance_k = bot_resistance_k;
	self->config.adc_full_scale_count = adc_full_scale_count;

	return 0;
}

rrd_ntc_st *rrd_ntc_new(
    uint16_t bot_resistance_k, 
    uint16_t adc_full_scale_count, 
    rrd_ntc_table_item_st *ntc_table, uint8_t ntc_table_length)
{
	rrd_ntc_st *self;
	self = malloc(sizeof(*self));
	if (NULL == self) return NULL;

	uint8_t ret = rrd_ntc_init(self, 
                                bot_resistance_k, 
                                adc_full_scale_count,
	                            ntc_table, ntc_table_length);
	if (ret) goto init_failed;

	self->del = (rrd_del_fn_t)rrd_ntc_destroy;
	return self;

init_failed:
	free(self);
	return NULL;
}

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#if OPEN_RRD_NTC_TEST

#endif
