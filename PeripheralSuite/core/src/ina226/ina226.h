/*
 * ina226.h
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef PERIPHERAL_SUITE_INA226_RRD_H_
#define PERIPHERAL_SUITE_INA226_RRD_H_

/******************************************************************************/
/*----------------------------------Include-----------------------------------*/
/******************************************************************************/
#include "src/common/peripheral_suite_common.h"

/******************************************************************************/
/*---------------------------------Constants----------------------------------*/
/******************************************************************************/
// INA226 REGISTER ADDRESSES
#define RRD_INA226_CONFIG_REG                   0x00      // RW. Configuration Register. All-register reset, shunt voltage and bus Configuration Register voltage ADC conversion times and averaging, operating mode.
#define RRD_INA226_SHUNT_V_REG                  0x01      // R.  Shunt Voltage Register.
#define RRD_INA226_BUS_V_REG                    0x02      // R.  Bus Voltage Register
#define RRD_INA226_POWER_REG                    0x03      // R.  Power Register
#define RRD_INA226_CURRENT_REG                  0x04      // R.  Current Register.
#define RRD_INA226_CALIB_REG                    0x05      // RW. Calibration Register. Sets full-scale range and LSB of current and power measurements. Overall system calibration.
#define RRD_INA226_MASK_EN_REG                  0x06      // RW. Mask/Enable Register. Alert configuration and Conversion Ready flag.
#define RRD_INA226_ALERT_REG                    0x07      // RW. Alert Limit Register. Contains the limit value to compare to the selected Alert function.
#define RRD_INA226_MAN_ID_REG                   0xFE      // R.  Manufacturer ID Register(0x5449). Contains unique manufacturer identification number.
#define RRD_INA226_DIE_ID_REG                   0xFF      // R.  Die ID Register(0x2260). Contains unique die identification number.

// CONFIG REGISTER BIT MASKS
#define RRD_INA226_CONF_RESET_MASK              0x8000
#define RRD_INA226_CONF_AVERAGE_MASK            0x0E00
#define RRD_INA226_CONF_BUSVC_MASK              0x01C0
#define RRD_INA226_CONF_SHUNTVC_MASK            0x0038
#define RRD_INA226_CONF_MODE_MASK               0x0007

// DEFAULT VALUES
#define RRD_INA226_MAN_ID_REG_RET               0x5449
#define RRD_INA226_DIE_ID_REG_RET               0x2260
#define RRD_INA226_CONFIG_REG_DEFAULT           0x4127

// LiMIT VALUES
#define RRD_INA226_MINIMAL_SHUNT_RESISTANCE_R   0.001
#define RRD_INA226_MAX_SHUNT_VOLTAGE_V          0.08192

/******************************************************************************/
/*-------------------------------------Enum-----------------------------------*/
/******************************************************************************/
typedef enum {
    RRD_INA226_1_SAMPLE,
    RRD_INA226_4_SAMPLES,
    RRD_INA226_16_SAMPLES,
    RRD_INA226_64_SAMPLES,
    RRD_INA226_128_SAMPLES,
    RRD_INA226_256_SAMPLES,
    RRD_INA226_512_SAMPLES,
    RRD_INA226_1024_SAMPLES
} rrd_ina226_average_et;

typedef enum {
    RRD_INA226_140_us,
    RRD_INA226_204_us,
    RRD_INA226_332_us,
    RRD_INA226_588_us,
    RRD_INA226_1100_us,
    RRD_INA226_2100_us,
    RRD_INA226_4200_us,
    RRD_INA226_8300_us
} rrd_ina226_timing_et;

typedef enum {
    RRD_INA226_MODE_POWER_DOWN_SHUTDOWN     = 0x00,
    RRD_INA226_MODE_SHUNT_TRIG              = 0x01,
    RRD_INA226_MODE_BUS_TRIG                = 0x02,
    RRD_INA226_MODE_SHUNT_BUS_TRIG          = 0x03,
    RRD_INA226_MODE_SHUNT_CONTINUOUS        = 0x05,
    RRD_INA226_MODE_BUS_CONTINUOUS          = 0x06,
    RRD_INA226_MODE_SHUNT_BUS_CONTINUOUS    = 0x07
} rrd_ina226_mode_et;

typedef enum {
    RRD_INA226_MEASUREMENT_POWER,
    RRD_INA226_MEASUREMENT_CURRENT,
    RRD_INA226_MEASUREMENT_BUS_VOLTAGE,
    RRD_INA226_MEASUREMENT_SHUNT_VOLTAGE
} rrd_ina226_measurement_et;

/******************************************************************************/
/*----------------------------------Interface---------------------------------*/
/******************************************************************************/
typedef float   (*rrd_ina226_get_fn_t)(void *self, rrd_ina226_measurement_et measurement);
typedef uint8_t (*rrd_ina226_set_calibration_fn_t)(void *self, float shunt_resistance_R, float current_max_A, float current_zero_offset_A, float bus_voltage_offset_V);
typedef uint8_t (*rrd_ina226_set_configuration_fn_t)(void *self, rrd_ina226_average_et average, rrd_ina226_timing_et bus_voltage_ct, rrd_ina226_timing_et shunt_voltage_ct, rrd_ina226_mode_et mode);

typedef struct rrd_ina226_interface {
    rrd_ina226_get_fn_t get;
    rrd_ina226_set_calibration_fn_t set_calibration;
    rrd_ina226_set_configuration_fn_t set_configuration;
} rrd_ina226_interface_st;

/******************************************************************************/
/*------------------------------------Ops-------------------------------------*/
/******************************************************************************/
/* rrd_ina226_ops_st
 * ****************************************************************************
 * name                 | args
 * ****************************************************************************
 * iic_read_buffer    	| iic_context: IIC context pointer.
 *                     	| reg: register address.
 *                     	| data: data buffer.
 *                     	| len: data length.
 * iic_write_buffer    	| iic_context: IIC context pointer.
 *                     	| reg: register address.
 *                     	| data: data buffer.
 *                     	| len: data length.
 * delay_us            	| us: delay in microseconds.
 * ****************************************************************************
 */
typedef struct rrd_ina226_ops {
    void *iic_context;
    uint8_t (*iic_read_buffer)(void *iic_context, uint8_t reg, uint8_t *data, uint8_t len);
    uint8_t (*iic_write_buffer)(void *iic_context, uint8_t reg, uint8_t *data, uint8_t len);
    
    void (*delay_us)(uint32_t us);
} rrd_ina226_ops_st;

/******************************************************************************/
/*--------------------------------Implementation------------------------------*/
/******************************************************************************/
typedef struct rrd_ina226 {
    rrd_ina226_interface_st *interface;
    rrd_ina226_ops_st *ops;

    rrd_del_fn_t del;

    struct{
        float shunt_resistance;         // unit: 0hm

        float current_max;              // unit: A
        float current_lsb;              // unit: A
        float current_zero_offset;      // unit: A

        float bus_voltage_offset;       // unit: V
    } config;

    struct{
        int16_t shunt_voltage_raw;      // unit: V
        int16_t bus_voltage_raw;        // unit: V
        int16_t power_raw;              // unit: W
        int16_t current_raw;            // unit: A

        float shunt_voltage;            // unit: V
        float bus_voltage;              // unit: V
        float power;                    // unit: W
        float current;                  // unit: A
    } params;
} rrd_ina226_st;

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/** \addtogroup Construction/Destruction
 ** \{ */
rrd_ina226_st *rrd_ina226_new(rrd_ina226_ops_st *ops);
uint8_t rrd_ina226_init(rrd_ina226_st *self, rrd_ina226_ops_st *ops);
/** \} */

/** \addtogroup Operation
 ** \{ */
float   rrd_ina226_get(rrd_ina226_st *self, rrd_ina226_measurement_et measurement);
uint8_t rrd_ina226_set_calibration(rrd_ina226_st *self, float shunt_resistance_R, float current_max_A, float current_zero_offset_A, float bus_voltage_offset_V);
uint8_t rrd_ina226_set_configuration(rrd_ina226_st *self, rrd_ina226_average_et average, rrd_ina226_timing_et bus_voltage_ct, rrd_ina226_timing_et shunt_voltage_ct, rrd_ina226_mode_et mode);
/** \} */

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#define OPEN_RRD_INA226_TEST (0)
#if OPEN_RRD_INA226_TEST
void rrd_ina226_test(rrd_ina226_ops_st *ops);
#endif
#endif /* PERIPHERAL_SUITE_INA226_RRD_H_ */
