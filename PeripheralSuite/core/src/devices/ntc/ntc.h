/*
 * ntc.h
 *	
 *  Note: ntc_resistance = bot_resistance / (adc_raw / adc_full_scale_count) - bot_resistance
 * 
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef PERIPHERAL_SUITE_NTC_RRD_H_
#define PERIPHERAL_SUITE_NTC_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
typedef float (*rrd_ntc_update_fn_t)(void *self, uint16_t raw_value);

typedef struct rrd_ntc_interface {
	rrd_ntc_update_fn_t update;
} rrd_ntc_interface_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_ntc_table_item {
	int16_t temperature;  // unit: ℃
	float resistance_k;  // unit: KΩ
} rrd_ntc_table_item_st;

typedef struct rrd_ntc {
	rrd_ntc_interface_st *interface;
	rrd_del_fn_t del;

	struct {
		uint8_t ntc_table_length;
		rrd_ntc_table_item_st *ntc_table;

		float bot_resistance_k;         // unit: KΩ. The fixed bottom resistor in the voltage divider circuit.
        uint16_t adc_full_scale_count;  // The full scale count of the ADC.
	} config;

	struct {
		float temperature;
	} params;
} rrd_ntc_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_ntc_st *rrd_ntc_new(
    uint16_t bot_resistance_k, 
    uint16_t adc_full_scale_count,
    rrd_ntc_table_item_st *ntc_table, uint8_t ntc_table_length);

uint8_t rrd_ntc_init(
    rrd_ntc_st *self, 
    uint16_t bot_resistance_k, 
    uint16_t adc_full_scale_count,
    rrd_ntc_table_item_st *ntc_table, uint8_t ntc_table_length);
/** \} */

/** \addtogroup Operation
 ** \{ */
float rrd_ntc_update_temperature(rrd_ntc_st *self, uint16_t raw_value);
/** \} */

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#define OPEN_RRD_NTC_TEST (0)
#if OPEN_RRD_NTC_TEST
#endif
#endif /* PERIPHERAL_SUITE_NTC_RRD_H_ */
