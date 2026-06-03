/*
 * buzzer.c
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#include "buzzer.h"

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
void rrd_buzzer_tone(rrd_buzzer_st *self, uint32_t frequency, float volume)
{
    self->is_toning = true;
    self->ops->pwm_set_freq(self->ops->pwm_context, frequency);
    self->ops->pwm_set_duty(self->ops->pwm_context, volume);
}

void rrd_buzzer_notone(rrd_buzzer_st *self)
{
    self->is_toning = false;
    self->ops->pwm_set_duty(self->ops->pwm_context, 0);
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_buzzer_interface_st g_rrd_buzzer_interface = {
    .tone = (rrd_buzzer_tone_fn_t)rrd_buzzer_tone,
    .notone = (rrd_buzzer_notone_fn_t)rrd_buzzer_notone,
};

uint8_t rrd_buzzer_init(rrd_buzzer_st *self, rrd_buzzer_ops_st *ops)
{
    if(self == NULL || ops == NULL) return 1;

    self->is_toning = false;
    self->ops = ops;
    self->interface = &g_rrd_buzzer_interface;

    return 0;
}

/******************************************************************************/
/*-----------------------------------Debug-----------------------------------*/
/******************************************************************************/
#if OPEN_BUZZER_TEST
void rrd_buzzer_test(rrd_buzzer_ops_st *ops)
{
    rrd_buzzer_st buzzer;
    rrd_buzzer_init(&buzzer, ops);

    buzzer.interface->tone(&buzzer, 2500, 0.01);
    HAL_Delay(300);
    buzzer.interface->notone(&buzzer);
    HAL_Delay(300);
    buzzer.interface->tone(&buzzer, 2500, 0.01);
    HAL_Delay(300);
    buzzer.interface->notone(&buzzer);
    HAL_Delay(300);
    buzzer.interface->tone(&buzzer, 2500, 0.01);
    HAL_Delay(300);
    buzzer.interface->notone(&buzzer);
}
#endif
