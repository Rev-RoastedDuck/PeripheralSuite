/*
 * ina226.c
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
#include "ina226.h"

/******************************************************************************/
/*-----------------------------------Private----------------------------------*/
/******************************************************************************/
/** \addtogroup iic function
 ** \{ */
static inline uint8_t _rrd_ina226_write_2bytes(rrd_ina226_st *self, const uint8_t reg_add, const uint16_t data)
{
	uint8_t buf[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
	uint8_t ret = self->ops->iic_write_buffer(self->ops->iic_context, reg_add, buf, 2);
	self->ops->delay_us(50);
	return ret;
}

static inline uint16_t _rrd_ina226_read_2bytes(rrd_ina226_st *self, const uint8_t reg_add)
{
	uint8_t buf[2] = {0};
	self->ops->iic_read_buffer(self->ops->iic_context, reg_add, buf, 2);
	self->ops->delay_us(50);
	return (uint16_t)((buf[0] << 8) | buf[1]);
}
/** \} */

/** \addtogroup configuration register function
 ** \{ */
static inline uint8_t _rrd_ina226_reset(rrd_ina226_st *self)
{
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask |= RRD_INA226_CONF_RESET_MASK;
	return _rrd_ina226_write_2bytes(self, RRD_INA226_CONFIG_REG, mask);
}

static inline uint8_t _rrd_ina226_set_average(rrd_ina226_st *self, uint8_t avg)
{
	if (avg > 7) return 1;
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask &= ~RRD_INA226_CONF_AVERAGE_MASK;
	mask |= (avg << 9);
	return _rrd_ina226_write_2bytes(self, RRD_INA226_CONFIG_REG, mask);
}

static inline uint8_t _rrd_ina226_get_average(rrd_ina226_st *self)
{
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask &= RRD_INA226_CONF_AVERAGE_MASK;
	mask >>= 9;
	return mask;
}

static inline uint8_t _rrd_ina226_set_bus_vct(rrd_ina226_st *self, uint8_t bvct)
{
	if (bvct > 7) return 1;
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask &= ~RRD_INA226_CONF_BUSVC_MASK;
	mask |= (bvct << 6);
	return _rrd_ina226_write_2bytes(self, RRD_INA226_CONFIG_REG, mask);
}

static inline uint8_t _rrd_ina226_get_bus_vct(rrd_ina226_st *self)
{
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask &= RRD_INA226_CONF_BUSVC_MASK;
	mask >>= 6;
	return mask;
}

static inline uint8_t _rrd_ina226_set_shunt_vct(rrd_ina226_st *self, uint8_t svct)
{
  if (svct > 7) return 1;
  uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
  mask &= ~RRD_INA226_CONF_SHUNTVC_MASK;
  mask |= (svct << 3);
  _rrd_ina226_write_2bytes(self, RRD_INA226_CONFIG_REG, mask);
  return 0;
}

static inline uint8_t _rrd_ina226_get_shunt_vct(rrd_ina226_st *self)
{
	uint16_t mask = _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG);
	mask &= RRD_INA226_CONF_SHUNTVC_MASK;
	mask >>= 3;
	return mask;
}
/** \} */

/** \addtogroup transfer function
 ** \{ */
static inline void _rrd_ina226_transfer_power(rrd_ina226_st *self)
{
	self->params.power = self->params.power_raw * self->config.current_lsb * 25.0f;	// fixed 25 W
}

static inline void _rrd_ina226_transfer_bus_voltage(rrd_ina226_st *self)
{
	self->params.bus_voltage = self->params.bus_voltage_raw * 1.25e-3f - self->config.bus_voltage_offset; // fixed 1.25 mV
}

static inline void _rrd_ina226_transfer_shunt_voltage(rrd_ina226_st *self)
{
	self->params.shunt_voltage = self->params.shunt_voltage_raw * 2.5e-6f; // fixed 2.50 uV
}

static inline void _rrd_ina226_transfer_current(rrd_ina226_st *self)
{
	self->params.current = self->params.current_raw * self->config.current_lsb - self->config.current_zero_offset;
}
/** \} */

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
/**
 * @brief set INA226 onfiguration register
 * @param self INA226 instrance
 * @param average Average number of samples to be averaged
 * @param bus_voltage_ct Bus voltage conversion time
 * @param shunt_voltage_ct Shunt voltage conversion time
 * @param mode Operating mode
 * @return uint8_t 0 success, other failed
*/
uint8_t rrd_ina226_set_configuration(
	rrd_ina226_st *self,
	rrd_ina226_average_et average,
	rrd_ina226_timing_et bus_voltage_ct,
	rrd_ina226_timing_et shunt_voltage_ct,
	rrd_ina226_mode_et mode)
{
	uint16_t config = 0;
	config |= (average << 9);
	config |= (bus_voltage_ct << 6);
	config |= (shunt_voltage_ct << 3);
	config |= mode;
	return _rrd_ina226_write_2bytes(self, RRD_INA226_CONFIG_REG, config);
}

/**
 * @brief Set INA226 Calibration Register
 * @note Shunt_voltage_input_range(mV): –81.9175, 81.92
 * @note 1 LSB = 2.5uV Shunt_voltage = 1.25mV Bus_voltage
 * @note Current_LSB = Maximum_Expected_Current / 2^15. Recommended values: 0.050, 0.100, 0.250, 0.500, 1, 2, 2.5
 * @note CAL = 0.00512 / (Current_LSB * R_SHUNT
*/
uint8_t rrd_ina226_set_calibration(
	rrd_ina226_st *self,
	float shunt_resistance_R,
	float current_max_A, float current_zero_offset_A,
	float bus_voltage_offset_V)
{
	if(current_max_A < 0.001f) return 1;
	if(shunt_resistance_R < RRD_INA226_MINIMAL_SHUNT_RESISTANCE_R) return 2;
	if(shunt_resistance_R * current_max_A > RRD_INA226_MAX_SHUNT_VOLTAGE_V) return 3;

	self->config.shunt_resistance = shunt_resistance_R;

	self->config.current_max = current_max_A;
	self->config.current_lsb = current_max_A / 32768.0f;
	self->config.current_zero_offset = current_zero_offset_A;

	self->config.bus_voltage_offset = bus_voltage_offset_V;

	// Calculate and set Calibration Register
	// Calibration Register is int16_t max 32767
	uint16_t calib = (uint16_t)(0.00512f / (self->config.current_lsb * shunt_resistance_R));
	while(calib > 32767){
		calib >>= 1;
		self->config.current_lsb *= 2;
	}

	return _rrd_ina226_write_2bytes(self, RRD_INA226_CALIB_REG, calib);
}

float rrd_ina226_get(rrd_ina226_st *self, rrd_ina226_measurement_et measurement)
{
	switch (measurement) {
	case RRD_INA226_MEASUREMENT_BUS_VOLTAGE:
		self->params.bus_voltage_raw = (int16_t)_rrd_ina226_read_2bytes(self, RRD_INA226_BUS_V_REG);
		_rrd_ina226_transfer_bus_voltage(self);
		return self->params.bus_voltage;
	case RRD_INA226_MEASUREMENT_SHUNT_VOLTAGE:
		self->params.shunt_voltage_raw = (int16_t)_rrd_ina226_read_2bytes(self, RRD_INA226_SHUNT_V_REG);
		_rrd_ina226_transfer_shunt_voltage(self);
		return self->params.shunt_voltage;
	case RRD_INA226_MEASUREMENT_CURRENT:
		self->params.current_raw = (int16_t)_rrd_ina226_read_2bytes(self, RRD_INA226_CURRENT_REG);
		_rrd_ina226_transfer_current(self);
		return self->params.current;
	case RRD_INA226_MEASUREMENT_POWER:
		self->params.power_raw = (int16_t)_rrd_ina226_read_2bytes(self, RRD_INA226_POWER_REG);
		_rrd_ina226_transfer_power(self);
		return self->params.power;
	default:
		return 0.0f;
	}
}
/** \} */

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_ina226_interface_st g_rrd_ina226_interface = {
	.set_calibration = (rrd_ina226_set_calibration_fn_t)rrd_ina226_set_calibration,
	.set_configuration = (rrd_ina226_set_configuration_fn_t)rrd_ina226_set_configuration,
	.get = (rrd_ina226_get_fn_t)rrd_ina226_get
};

static void rrd_ina226_deinit(void *self)
{
	if (self == NULL) return;

	rrd_ina226_st *self_t = self;
	memset(self_t, 0, sizeof(*self_t));
}

static void rrd_ina226_destroy(void **self)
{
	if (NULL == self || NULL == *self) return;

	rrd_ina226_st *self_t = (rrd_ina226_st *)(*self);
	rrd_ina226_deinit(self_t);
	free(self_t);
	*self = NULL;
}

uint8_t rrd_ina226_init(rrd_ina226_st *self, rrd_ina226_ops_st *ops)
{
	if (NULL == self || NULL == ops) return 1;

	memset(self, 0, sizeof(*self));

	self->ops = ops;
	self->interface = &g_rrd_ina226_interface;
	self->del = (rrd_del_fn_t)rrd_ina226_deinit;

	{// reset and self-check
		_rrd_ina226_reset(self);
		if (RRD_INA226_MAN_ID_REG_RET != _rrd_ina226_read_2bytes(self, RRD_INA226_MAN_ID_REG)) return 3;
		if (RRD_INA226_DIE_ID_REG_RET != _rrd_ina226_read_2bytes(self, RRD_INA226_DIE_ID_REG)) return 4;
		if (RRD_INA226_CONFIG_REG_DEFAULT != _rrd_ina226_read_2bytes(self, RRD_INA226_CONFIG_REG)) return 5;
	}

	self->config.current_lsb = 0.002;
	self->config.current_zero_offset = 0;

	rrd_ina226_set_configuration(self, 
								 RRD_INA226_4_SAMPLES,
	                             RRD_INA226_204_us, RRD_INA226_204_us,
	                             RRD_INA226_MODE_SHUNT_BUS_CONTINUOUS);
	return 0;
}

rrd_ina226_st *rrd_ina226_new(rrd_ina226_ops_st *ops)
{
	rrd_ina226_st *self;
	self = malloc(sizeof(*self));
	if (NULL == self) return NULL;

	uint8_t ret = rrd_ina226_init(self, ops);
	if (ret) goto init_failed;

	self->del = (rrd_del_fn_t)rrd_ina226_destroy;
	return self;

init_failed:
	free(self);
	return NULL;
}

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#if OPEN_RRD_INA226_TEST
void rrd_ina226_test(rrd_ina226_ops_st *ops)
{
	rrd_ina226_st *ina226_dev = rrd_ina226_new(ops);
	if (NULL == ina226_dev) return;

	ina226_dev->interface->set_calibration(ina226_dev, 0.1f, 0.001f, 0, 0);

	ina226_dev->del(&ina226_dev);
}
#endif
