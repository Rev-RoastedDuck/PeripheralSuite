/*
 * as5600.h
 *	
 *  Created on: 2026_5_16
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
#ifndef PERIPHERAL_SUITE_AS5600_RRD_H_
#define PERIPHERAL_SUITE_AS5600_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"

/******************************************************************************/
/*---------------------------------Constants----------------------------------*/
/******************************************************************************/
#define RRD_AS5600_CPR      	    (4096) 				// 每圈计数
#define RRD_AS5600_REG_RAW_ANGLE 	(0x0C) 				// 原始角度寄存器地址
#define RRD_AS5600_ADDRESS  	    (0x6c) 				// 8位
#define RRD_AS5600_WRAP_THRESHOLD 	((int16_t)(2867))	//RRD_AS5600_CPR * 0.7f

typedef enum {
    RRD_AS5600_UPDATE_OPT_NONE  = 0x00,
    RRD_AS5600_UPDATE_OPT_SPEED = 0x01 << 0,
} rrd_as5600_update_opt_et;

/******************************************************************************/
/*------------------------------------Ops-------------------------------------*/
/******************************************************************************/
/* rrd_as5600_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * iic_read_buffer    	| reg: register address.
 *                      | data: data buffer.
 *                      | len: data length.
 * iic_write_buffer    	| reg: register address.
 *                      | data: data buffer.
 *                      | len: data length.
 * ****************************************************************************
 */
typedef struct rrd_as5600_ops {
    void *iic_context;
    void (*iic_read_buffer)   (void *iic_context, uint8_t reg, uint8_t *data, uint8_t len);
    void (*iic_write_buffer)  (void *iic_context, uint8_t reg, uint8_t *data, uint8_t len);

    void *base_context;
	rrd_get_timestamp_size_hook_t get_time_stamp_us;
} rrd_as5600_ops_st;

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
/* rrd_as5600_interface_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * update_x    	        | opt: update option.
 * ****************************************************************************
 */
typedef void (*rrd_as5600_update_fn_t)	(void *self, uint8_t opt);
typedef struct rrd_as5600_interface {
	rrd_as5600_update_fn_t update;
	rrd_as5600_update_fn_t update_fx;
	rrd_as5600_update_fn_t update_fp;
} rrd_as5600_interface_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_as5600 {
	rrd_as5600_interface_st *interface;
	rrd_as5600_ops_st *ops;

	rrd_del_fn_t del;

    struct {
        int32_t speed;             // unit: count/sec
        int32_t revolutions;       // unit: rev
        int32_t multi_turn_count;  // unit: count
        uint16_t single_turn_count;// unit: count
        size_t _time_stamp_us;
    } raw;

    struct {
        int32_t multi_turn_q16;   // Q16.16
        uint16_t single_turn_q16; // Q16. [0, 4095] -> [0, 2PI]
    } fx;

    struct {
        float speed;              // rad/s
        float multi_turn_radian;  // rad
        float single_turn_radian; // rad
    } fp;
} rrd_as5600_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_as5600_st *rrd_as5600_new(rrd_as5600_ops_st *ops);
uint8_t rrd_as5600_init(rrd_as5600_st *self, rrd_as5600_ops_st *ops);
/** \} */

/** \addtogroup Operation
 ** \{ */
void rrd_as5600_update(rrd_as5600_st *self, uint8_t opt);
void rrd_as5600_update_fx(rrd_as5600_st *self, uint8_t opt);
void rrd_as5600_update_fp(rrd_as5600_st *self, uint8_t opt);
/** \} */

#define OPEN_AS5600_TEST_RRD (0)
#if OPEN_AS5600_TEST_RRD
void rrd_as5600_test(rrd_as5600_ops_st *ops);
#endif
#endif /* PERIPHERAL_SUITE_AS5600_RRD_H_ */
