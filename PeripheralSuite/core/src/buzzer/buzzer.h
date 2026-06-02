/*
 * buzzer.h
 *	
 *  Created on: 2025_10_11
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
#ifndef PERIPHERAL_SUITE_BUZZER_RRD_H_
#define PERIPHERAL_SUITE_BUZZER_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*------------------------------------Ops-------------------------------------*/
/******************************************************************************/
/*
 * rrd_buzzer_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * pwm_set_duty         | duty: duty in [0, 1].
 * pwm_set_freq         | frequency: frequency in Hz.
 * ****************************************************************************
 */
typedef struct rrd_buzzer_ops {
    void *pwm_context;
    void (*pwm_set_duty)    (void *pwm_context, float duty);
    void (*pwm_set_freq)    (void *pwm_context, uint32_t frequency);
} rrd_buzzer_ops_st;

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
/*
 * rrd_buzzer_interface_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * notone               | none.
 * tone                 | frequency: frequency in Hz.
 *                      | volume: volume in [0, 1].
 * ****************************************************************************
 */
typedef void (*rrd_buzzer_notone_fn_t)(void *self);
typedef void (*rrd_buzzer_tone_fn_t)  (void *self, uint32_t frequency, float volume);

typedef struct rrd_buzzer_interface {
    rrd_buzzer_tone_fn_t tone;
    rrd_buzzer_notone_fn_t notone;
} rrd_buzzer_interface_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_buzzer {
    rrd_buzzer_interface_st *interface;
    rrd_buzzer_ops_st *ops;

    volatile bool is_toning;
} rrd_buzzer_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
uint8_t rrd_buzzer_init(rrd_buzzer_st *self, rrd_buzzer_ops_st *ops);
/** \} */

/** \addtogroup Operation
 ** \{ */
void rrd_buzzer_notone(rrd_buzzer_st *self);
void rrd_buzzer_tone(rrd_buzzer_st *self, uint32_t frequency, float volume);
/** \} */

/******************************************************************************/
/*-----------------------------------Debug-----------------------------------*/
/******************************************************************************/
#define OPEN_BUZZER_TEST 0
#if OPEN_BUZZER_TEST
void rrd_buzzer_test(rrd_buzzer_ops_st *ops);
#endif
#endif /* PERIPHERAL_SUITE_BUZZER_RRD_H_ */
