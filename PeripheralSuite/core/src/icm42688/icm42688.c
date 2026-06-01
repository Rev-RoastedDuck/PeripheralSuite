/*
 * icm42688.c
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2025 by Rev-RoastedDuck.
 */
#include "icm42688.h"

/******************************************************************************/
/*---------------------------------Constants----------------------------------*/
/******************************************************************************/
#define RRD_ICM42688_G              9.8011f

#define RRD_ICM42688_LSB_ACC_16G	0.0047856934f
#define RRD_ICM42688_LSB_ACC_8G		0.0023928467f
#define RRD_ICM42688_LSB_ACC_4G		0.0011964233f
#define RRD_ICM42688_LSB_ACC_2G		0.00059821167f

#define RRD_ICM42688_LSB_GYRO_2000_R	0.0010652644f
#define RRD_ICM42688_LSB_GYRO_1000_R	0.00053263222f
#define RRD_ICM42688_LSB_GYRO_500_R	    0.00026631611f
#define RRD_ICM42688_LSB_GYRO_250_R	    0.00013315805f
#define RRD_ICM42688_LSB_GYRO_125D_R	0.000066579027f

/******************************************************************************/
/*-----------------------------------Private----------------------------------*/
/******************************************************************************/
static inline void _rrd_icm42688_write_reg(rrd_icm42688_st *self, uint8_t reg, uint8_t data)
{
	self->ops->spi_read_write(self->ops->spi_context, reg, &data, NULL, 1);
}

static inline void _rrd_icm42688_read_reg(rrd_icm42688_st *self, uint8_t reg, uint8_t *buff)
{
	self->ops->spi_read_write(self->ops->spi_context, reg, NULL, buff, 1);
}

static void _rrd_icm42688_calibration_init(rrd_icm42688_st *self)
{
	self->config.calibration_count = 2000;
	self->calibration.cycles_remaining = self->config.calibration_count;
	memset(self->calibration.accel_sum, 0, sizeof(self->calibration.accel_sum));
	memset(self->calibration.gyro_sum, 0, sizeof(self->calibration.gyro_sum));
}

static void _rrd_icm42688_calibration(rrd_icm42688_st *self)
{
	for (uint8_t axis = 0; axis < 3; ++axis) {
		self->calibration.accel_sum[axis] += self->accel_adc[axis];
		if (self->calibration.cycles_remaining == 1) {
			self->accel_adc_zero[axis] = self->calibration.accel_sum[axis] / self->config.calibration_count;
			if (axis == 2) {
				self->accel_adc_zero[axis] -= RRD_ICM42688_G / self->config.lsb_accel;
			}
		}
	}

	for (uint8_t axis = 0; axis < 3; ++axis) {
		self->calibration.gyro_sum[axis] += self->gyro_adc[axis];
		if (self->calibration.cycles_remaining == 1) {
			self->gyro_adc_zero[axis] = self->calibration.gyro_sum[axis] / self->config.calibration_count;
			if (axis == 2) {
				self->gyro_adc_zero[axis] -= (float)self->config.gyro_offset_yaw;
			}
		}
	}
	--self->calibration.cycles_remaining;
}

/******************************************************************************/
/*-----------------------------------Public-----------------------------------*/
/******************************************************************************/
void rrd_icm42688_update(rrd_icm42688_st *self)
{
	uint8_t buff[14];
	self->ops->spi_read_write(
		self->ops->spi_context,
		RRD_ICM42688_TEMP_DATA1,
		NULL,
		buff,
		14);

	int16_t raw_tmp    =         (int16_t)((buff[0] << 8)  | buff[1]);
	self->accel_adc[0] = (float)((int16_t)((buff[2] << 8)  | buff[3])  - self->accel_adc_zero[0]);
	self->accel_adc[1] = (float)((int16_t)((buff[4] << 8)  | buff[5])  - self->accel_adc_zero[1]);
	self->accel_adc[2] = (float)((int16_t)((buff[6] << 8)  | buff[7])  - self->accel_adc_zero[2]);
	self->gyro_adc[0]  = (float)((int16_t)((buff[8] << 8)  | buff[9])  - self->gyro_adc_zero[0]);
	self->gyro_adc[1]  = (float)((int16_t)((buff[10] << 8) | buff[11]) - self->gyro_adc_zero[1]);
	self->gyro_adc[2]  = (float)((int16_t)((buff[12] << 8) | buff[13]) - self->gyro_adc_zero[2]);

	self->temperature = ((float)raw_tmp / 132.48f) + 25;

	self->accel[0] = self->accel_adc[0] * self->config.lsb_accel;
	self->accel[1] = self->accel_adc[1] * self->config.lsb_accel;
	self->accel[2] = self->accel_adc[2] * self->config.lsb_accel;

	self->gyro[0] = self->gyro_adc[0] * self->config.lsb_gyro;
	self->gyro[1] = self->gyro_adc[1] * self->config.lsb_gyro;
	self->gyro[2] = self->gyro_adc[2] * self->config.lsb_gyro;
}

void rrd_icm42688_calibration(rrd_icm42688_st *self)
{
    uint8_t buff = 0;
	_rrd_icm42688_calibration_init(self);
	while (self->calibration.cycles_remaining) {
		self->ops->delay_ms(1);
        _rrd_icm42688_read_reg(self, RRD_ICM42688_INT_STATUS, &buff);
		if(buff & 0x08){
            rrd_icm42688_update(self);
            _rrd_icm42688_calibration(self);
        }
	}
}

void rrd_icm42688_get_int_status(rrd_icm42688_st *self, uint8_t *buff)
{
    _rrd_icm42688_read_reg(self, RRD_ICM42688_INT_STATUS, buff);
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
static rrd_icm42688_interface_st g_rrd_icm42688_interface = {
	.update = (rrd_icm42688_update_fn_t)rrd_icm42688_update,
	.calibration = (rrd_icm42688_calibration_fn_t)rrd_icm42688_calibration,
    .get_int_status = (rrd_icm42688_get_int_status_fn_t)rrd_icm42688_get_int_status,
};

static void rrd_icm42688_deinit(void *self)
{
	if (self == NULL) return;

	rrd_icm42688_st *self_t = self;
	memset(self_t, 0, sizeof(*self_t));
}

static void rrd_icm42688_destroy(void **self)
{
	if (NULL == self || NULL == *self) return;

	rrd_icm42688_st *self_t = (rrd_icm42688_st *)(*self);
	rrd_icm42688_deinit(self_t);
	free(self_t);
	*self = NULL;
}

uint8_t rrd_icm42688_init(rrd_icm42688_st *self, rrd_icm42688_ops_st *ops)
{
	if (NULL == self || NULL == ops) return 1;

	memset(self, 0, sizeof(*self));

	self->ops = ops;
	self->interface = &g_rrd_icm42688_interface;
	self->del = (rrd_del_fn_t)rrd_icm42688_deinit;

	self->config.lsb_accel = RRD_ICM42688_LSB_ACC_16G;
	self->config.lsb_gyro = RRD_ICM42688_LSB_GYRO_2000_R;

	{// 初始化
		uint8_t buff = 0;

		// 关闭传感器
		_rrd_icm42688_write_reg(self, RRD_ICM42688_PWR_MGMT0, 0x00);
		self->ops->delay_ms(10);

		// 软复位
		_rrd_icm42688_write_reg(self, RRD_ICM42688_DEVICE_CONFIG, 0x01);
		self->ops->delay_ms(10);

		// 自检
		_rrd_icm42688_write_reg(self, RRD_ICM42688_REG_BANK_SEL, 0x00);
		self->ops->spi_read_write(self->ops->spi_context, RRD_ICM42688_WHO_AM_I, NULL, &buff, 1);
		if (buff != 0x47) return 1;

		// 清空中断标志位
		_rrd_icm42688_read_reg(self, RRD_ICM42688_INT_STATUS, &buff);

		// Gyro设置 2000dps 1KHz
		_rrd_icm42688_write_reg(self, RRD_ICM42688_GYRO_CONFIG0, 0x06);

		// Accel设置 16G 1KHz
		_rrd_icm42688_write_reg(self, RRD_ICM42688_ACCEL_CONFIG0, 0x06);
	}

	{// 中断
		// 中断输出设置: INT1 INT2 脉冲模式，高电平有效，推挽输出
		_rrd_icm42688_write_reg(self, RRD_ICM42688_INT_CONFIG, 0x1B);

		// 映射中断源0(UI_DRDY_INT1) 到 INT1
		_rrd_icm42688_write_reg(self, RRD_ICM42688_INT_SOURCE0, 0x08);
	}

	{// GYRO抗混叠滤波器配置
		// 切换到Bank1
		_rrd_icm42688_write_reg(self, RRD_ICM42688_REG_BANK_SEL, 0x01);

		// GYRO_AAF_DELT 12 (default 13)
		_rrd_icm42688_write_reg(self, RRD_ICM42688_GYRO_CONFIG_STATIC3, 0x0C);

		// GYRO_AAF_DELTSQR 144 (default 170)
		_rrd_icm42688_write_reg(self, RRD_ICM42688_GYRO_CONFIG_STATIC4, 0x90);
	}

	{// ACCEL抗混叠滤波器配置
		// 切换到Bank2
		_rrd_icm42688_write_reg(self, RRD_ICM42688_REG_BANK_SEL, 0x02);

		// 开启滤波器 ACCEL_AFF_DELT 12 (default 24)
		_rrd_icm42688_write_reg(self, RRD_ICM42688_ACCEL_CONFIG_STATIC2, 0x18);

		// ACCEL_AFF_DELTSQR 144 (default 64)
		_rrd_icm42688_write_reg(self, RRD_ICM42688_ACCEL_CONFIG_STATIC3, 0x90);

		// ACCEL_AAF_BITSHIFT 8 (default 6)
		_rrd_icm42688_write_reg(self, RRD_ICM42688_ACCEL_CONFIG_STATIC4, 0x80);
	}

	{// 自定义滤波器 111Hz
		// 指定Bank0
		_rrd_icm42688_write_reg(self, RRD_ICM42688_REG_BANK_SEL, 0x00);

		// 配置陀螺仪和温度计滤波器: 温度滤波器带宽为 4000Hz，自定义滤波器阶数为 1阶，固定滤波器阶数为 3阶
		_rrd_icm42688_write_reg(self, RRD_ICM42688_GYRO_CONFIG1, 0x12);

		// 配置加速度计滤波器: 自定义滤波器阶数为 1阶，固定滤波器阶数为 3阶
		_rrd_icm42688_write_reg(self, RRD_ICM42688_ACCEL_CONFIG1, 0x05);

		// 3dB
		_rrd_icm42688_write_reg(self, RRD_ICM42688_GYRO_ACCEL_CONFIG0, 0x33);
	}

	{// 开启所有传感器
		// 指定Bank0
		_rrd_icm42688_write_reg(self, RRD_ICM42688_REG_BANK_SEL, 0x00);

		// 使用低噪声模式
		_rrd_icm42688_write_reg(self, RRD_ICM42688_PWR_MGMT0, 0x0F);
	}

	return 0;
}

rrd_icm42688_st *rrd_icm42688_new(rrd_icm42688_ops_st *ops)
{
	rrd_icm42688_st *self;
	self = malloc(sizeof(*self));
	if (NULL == self) return NULL;

	uint8_t ret = rrd_icm42688_init(self, ops);
	if (ret) goto init_failed;

	self->del = (rrd_del_fn_t)rrd_icm42688_destroy;
	return self;

init_failed:
	free(self);
	return NULL;
}

/******************************************************************************/
/*-----------------------------------Debug------------------------------------*/
/******************************************************************************/
#if OPEN_RRD_ICM42688_TEST
void rrd_icm42688_test(rrd_icm42688_ops_st *ops)
{
	uint8_t whoami = 0;
	ops->spi_read_write(ops->spi_context, RRD_ICM42688_WHO_AM_I, NULL, &whoami, 1);

	rrd_icm42688_st *icm42688 = rrd_icm42688_new(ops);
	if (NULL == icm42688) return;

	icm42688->interface->update(icm42688);
	icm42688->interface->calibration_init(icm42688);
	icm42688->interface->calibration(icm42688);

	icm42688->del(&icm42688);
}
#endif
