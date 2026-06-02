/*
 * mt6816.h
 *	
 *  Created on: 2026_6_2
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2026 by Rev-RoastedDuck.
 */
#ifndef PERIPHERAL_SUITE_MT6816_RRD_H_
#define PERIPHERAL_SUITE_MT6816_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"

/******************************************************************************/
/*---------------------------------Constants----------------------------------*/
/******************************************************************************/
#define RRD_MT6816_CPR               (16384)             // count-per-revolution
#define RRD_MT6816_REG_03            (0x03)              // Angle[13:8] + status
#define RRD_MT6816_REG_04            (0x04)              // Angle[7:0]
#define RRD_MT6816_READ_CMD          (0x80)              // SPI read command prefix
#define RRD_MT6816_WRAP_THRESHOLD    ((int16_t)(11469))  // RRD_MT6816_CPR * 0.7f
#define RRD_MT6816_NO_MAGNET_MASK    (0x0002)            // No magnet warning flag

typedef enum {
    RRD_MT6816_UPDATE_OPT_NONE  = 0x00,
    RRD_MT6816_UPDATE_OPT_SPEED = 0x01 << 0,
} rrd_mt6816_update_opt_et;

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
/* rrd_mt6816_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * spi_transfer16      	| spi_context: SPI context pointer.
 *                     	| tx_data: 16-bit command to send.
 *                     	| returns: 16-bit received data.
 * ****************************************************************************
 */
typedef struct rrd_mt6816_ops {
    void *spi_context;
    uint16_t (*spi_transfer16)(void *spi_context, uint16_t tx_data);

    void *base_context;
    rrd_get_timestamp_size_hook_t get_time_stamp_us;
} rrd_mt6816_ops_st;

/* rrd_mt6816_interface_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * update_x    	         | opt: update option.
 * ****************************************************************************
 */
typedef void (*rrd_mt6816_update_fn_t)(void *self, uint8_t opt);
typedef struct rrd_mt6816_interface {
    rrd_mt6816_update_fn_t update;
    rrd_mt6816_update_fn_t update_fx;
    rrd_mt6816_update_fn_t update_fp;
} rrd_mt6816_interface_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_mt6816 {
    rrd_mt6816_interface_st *interface;
    rrd_mt6816_ops_st *ops;

    rrd_del_fn_t del;

    struct {
        bool no_magnet;             // no magnet warning flag
        bool parity_error;          // parity error flag
        int32_t speed;              // unit: count/sec
        int32_t revolutions;        // unit: rev
        int32_t multi_turn_count;   // unit: count
        uint16_t single_turn_count; // unit: count
        size_t _time_stamp_us;
    } raw;

    struct {
        int32_t multi_turn_q16;   // Q16.16
        uint16_t single_turn_q16; // Q16. [0, 16383] -> [0, 2PI]
    } fx;

    struct {
        float speed;              // rad/s
        float multi_turn_radian;  // rad
        float single_turn_radian; // rad
    } fp;
} rrd_mt6816_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_mt6816_st *rrd_mt6816_new(rrd_mt6816_ops_st *ops);
uint8_t rrd_mt6816_init(rrd_mt6816_st *self, rrd_mt6816_ops_st *ops);
/** \} */

/** \addtogroup Operation
 ** \{ */
void rrd_mt6816_update(rrd_mt6816_st *self, uint8_t opt);
void rrd_mt6816_update_fx(rrd_mt6816_st *self, uint8_t opt);
void rrd_mt6816_update_fp(rrd_mt6816_st *self, uint8_t opt);
/** \} */

#define OPEN_MT6816_TEST_RRD (0)
#if OPEN_MT6816_TEST_RRD
void rrd_mt6816_test(rrd_mt6816_ops_st *ops);
#endif
#endif /* PERIPHERAL_SUITE_MT6816_RRD_H_ */
