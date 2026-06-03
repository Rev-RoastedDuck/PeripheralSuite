/*
 * icm42688_reg.h
 *	
 *  Created on: 2025_10_10
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2025 by Rev-RoastedDuck.
 */

#ifndef PERIPHERAL_SUITE_ICM42688_REG_RRD_H_
#define PERIPHERAL_SUITE_ICM42688_REG_RRD_H_

#define RRD_ICM42688_WHO_AM_I 0x75

// *********************************************************************************
// Register Bank0：核心功能寄存器（默认寄存器组，无需切换Bank即可访问）
// *********************************************************************************

/* 可配置值（bit[7:0]）：
 * 1. SPI模式选择（bit[4]）：
 *    - 0：SPI模式0/3（默认，CPOL=0/1，CPHA=0/1）
 *    - 1：SPI模式1/2（CPOL=0/1，CPHA=1/0）
 * 2. 软复位（bit[0]）：
 *    - 0：正常模式（默认）
 *    - 1：触发软复位（置1后自动清0，复位后需等待1ms再操作）
 * 3. bit[7:5]、bit[3:1]：保留（读写无效）
 * 注：I2C从机地址由AP_AD0引脚决定（0→0x68，1→0x69），非本寄存器配置
 */
#define RRD_ICM42688_DEVICE_CONFIG             0x11  // 设备配置寄存器：SPI模式与软复位（手册14.1节）

/* 可配置值（bit[7:0]）：
 * 1. I2C引脚slew rate（bit[5:3]，仅I2C模式有效）：
 *    - 000：20~60ns  001：12~36ns  010：6~18ns  011：4~12ns
 *    - 100：2~6ns   101：<2ns     110~111：保留
 * 2. SPI/I3C引脚slew rate（bit[2:0]，SPI/I3C模式有效）：
 *    - 配置值同I2C引脚，默认010（6~18ns）
 * 3. bit[7:6]：保留（读写无效）
 */
#define RRD_ICM42688_DRIVE_CONFIG              0x13  // 驱动配置寄存器：I2C/SPI引脚slew rate（手册14.2节）

/* 可配置值（bit[7:0]）：
 * 1. INT2配置（bit[5:3]）：
 *    - bit[5]：INT2模式（0=脉冲模式，1=锁存模式）
 *    - bit[4]：INT2驱动电路（0=开漏输出，1=推挽输出）
 *    - bit[3]：INT2极性（0=低电平有效，1=高电平有效，默认0）
 * 2. INT1配置（bit[2:0]）：
 *    - bit[2]：INT1模式（0=脉冲模式，1=锁存模式）
 *    - bit[1]：INT1驱动电路（0=开漏输出，1=推挽输出）
 *    - bit[0]：INT1极性（0=低电平有效，1=高电平有效，默认0）
 * 3. bit[7:6]：保留（读写无效）
 */
#define RRD_ICM42688_INT_CONFIG                0x14  // 中断配置寄存器：INT1/INT2引脚属性（手册14.3节）

/* 可配置值（bit[7:0]）：
 * 1. FIFO模式（bit[1:0]）：
 *    - 00：循环模式（满后覆盖旧数据，默认）
 *    - 01：停止模式（满后停止写入）
 *    - 10~11：保留
 * 2. FIFO刷新（bit[2]）：
 *    - 0：正常（默认）
 *    - 1：刷新FIFO（置1后自动清0，清空所有数据）
 * 3. bit[7:3]：保留（读写无效）
 * 注：FIFO使能需配置FIFO_CONFIG1（0x5F）的FIFO_GYRO_EN/FIFO_ACCEL_EN
 */
#define RRD_ICM42688_FIFO_CONFIG               0x16  // FIFO配置寄存器：FIFO模式与刷新（手册14.4节）

/* 温度转换公式（手册14.6节）：
 * Temperature(°C) = (TEMP_DATA / 132.48) + 25
 * 例：TEMP_DATA=0 → 25°C；TEMP_DATA=13248 → 125°C（超出量程，实际最大85°C）
 * 量程：-40°C ~ +85°C，精度±0.1°C
 */
#define RRD_ICM42688_TEMP_DATA1                0x1D  // 温度数据高8位：只读，16位补码（手册14.5节）
#define RRD_ICM42688_TEMP_DATA0                0x1E  // 温度数据低8位：只读（手册14.6节）

/* 加速度计算：实际值 = (ACCEL_DATA) / 灵敏度（灵敏度由ACCEL_CONFIG0量程决定）
 * 例：量程±2g（灵敏度16384 LSB/g），ACCEL_DATA=16384 → 1g
 */
#define RRD_ICM42688_ACCEL_DATA_X1             0x1F  // 加速度计X轴数据高8位：只读，16位补码（手册14.7节）
#define RRD_ICM42688_ACCEL_DATA_X0             0x20  // 加速度计X轴数据低8位：只读（手册14.8节）
#define RRD_ICM42688_ACCEL_DATA_Y1             0x21  // 加速度计Y轴数据高8位：只读（手册14.9节）
#define RRD_ICM42688_ACCEL_DATA_Y0             0x22  // 加速度计Y轴数据低8位：只读（手册14.10节）
#define RRD_ICM42688_ACCEL_DATA_Z1             0x23  // 加速度计Z轴数据高8位：只读（手册14.11节）
#define RRD_ICM42688_ACCEL_DATA_Z0             0x24  // 加速度计Z轴数据低8位：只读（手册14.12节）

/* 角速度计算：实际值 = (GYRO_DATA) / 灵敏度（灵敏度由GYRO_CONFIG0量程决定）
 * 例：量程±250dps（灵敏度131 LSB/dps），GYRO_DATA=131 → 1dps
 */
#define RRD_ICM42688_GYRO_DATA_X1              0x25  // 陀螺仪X轴数据高8位：只读，16位补码（手册14.13节）
#define RRD_ICM42688_GYRO_DATA_X0              0x26  // 陀螺仪X轴数据低8位：只读（手册14.14节）
#define RRD_ICM42688_GYRO_DATA_Y1              0x27  // 陀螺仪Y轴数据高8位：只读（手册14.15节）
#define RRD_ICM42688_GYRO_DATA_Y0              0x28  // 陀螺仪Y轴数据低8位：只读（手册14.16节）
#define RRD_ICM42688_GYRO_DATA_Z1              0x29  // 陀螺仪Z轴数据高8位：只读（手册14.17节）
#define RRD_ICM42688_GYRO_DATA_Z0              0x2A  // 陀螺仪Z轴数据低8位：只读（手册14.18节）

/* 关键状态位（bit[7:0]）：
 * - bit0：AGC就绪中断（AGC_RDY_INT）
 * - bit1：FIFO满中断（FIFO_FULL_INT）
 * - bit2：FIFO水印中断（FIFO_THS_INT）
 * - bit3：数据就绪中断（DATA_RDY_INT）
 * - bit4：复位完成中断（RESET_DONE_INT）
 * - bit5：PLL就绪中断（PLL_RDY_INT）
 * - bit6：UI_FSYNC中断（UI_FSYNC_INT）
 * - bit7：保留
 * 注：读此寄存器后，所有状态位自动清0（锁存模式下）
 */
#define RRD_ICM42688_INT_STATUS                0x2D  // 中断状态寄存器：只读，中断触发标识（手册14.21节）

/* 可配置值（bit[7:0]）：
 * 1. 温度传感器使能（bit[5]）：
 *    - 0：禁用温度传感器
 *    - 1：使能温度传感器（默认）
 * 2. 陀螺仪模式（bit[3:2]）：
 *    - 00：陀螺仪关闭（默认）
 *    - 01：陀螺仪待机模式
 *    - 10：保留
 *    - 11：陀螺仪低噪声（LN）模式（全性能）
 * 3. 加速度计模式（bit[1:0]）：
 *    - 00：加速度计关闭（默认）
 *    - 01：加速度计关闭
 *    - 10：加速度计低功耗（LP）模式
 *    - 11：加速度计低噪声（LN）模式（全性能）
 * 4. IDLE模式（bit[4]）：0=RC oscillator随传感器关闭，1=RC oscillator始终使能（默认0）
 * 5. bit[7:6]：保留（读写无效）
 * 注：传感器从关闭→使能后，需等待200μs再写入其他寄存器
 */
#define RRD_ICM42688_PWR_MGMT0                 0x4E  // 电源管理0：陀螺仪/加速度计/温度传感器使能（手册14.36节）

/* 可配置值（bit[7:0]）：
 * 1. 量程选择（GYRO_UI_FS_SEL，bit[7:5]）：
 *    - 000：±2000 dps，灵敏度=16.4  LSB/dps
 *    - 001：±1000 dps，灵敏度=32.8  LSB/dps
 *    - 010：±500  dps，灵敏度=65.5  LSB/dps
 *    - 011：±250  dps，灵敏度=131   LSB/dps（常用）
 *    - 100：±125  dps，灵敏度=262   LSB/dps
 *    - 101：±62.5 dps，灵敏度=524.3 LSB/dps
 *    - 110：±31.25dps，灵敏度=1048.6 LSB/dps
 *    - 111：±15.625dps，灵敏度=2097.2 LSB/dps
 * 2. 采样率选择（GYRO_ODR，bit[3:0]）：
 *    - 0000：保留    0001：32kHz    0010：16kHz    0011：8kHz
 *    - 0100：4kHz    0101：2kHz    0110：1kHz（默认） 0111：200Hz
 *    - 1000：100Hz   1001：50Hz    1010：25Hz     1011：12.5Hz
 *    - 1100~1101：保留  1111：500Hz
 * 3. bit[4]：保留（读写无效）
 */
#define RRD_ICM42688_GYRO_CONFIG0              0x4F  // 陀螺仪配置0：量程（FS）与采样率（ODR）（手册14.37节）

/* 可配置值（bit[7:0]）：
 * 1. 量程选择（ACCEL_UI_FS_SEL，bit[7:5]）：
 *    - 000：±16 g，灵敏度=2048  LSB/g
 *    - 001：±8  g，灵敏度=4096  LSB/g
 *    - 010：±4  g，灵敏度=8192  LSB/g
 *    - 011：±2  g，灵敏度=16384 LSB/g（常用，默认）
 *    - 100~111：保留
 * 2. 采样率选择（ACCEL_ODR，bit[3:0]）：
 *    - LN模式（bit[1:0]=11）：支持12.5Hz~32kHz（同GYRO_ODR，默认1kHz）
 *    - LP模式（bit[1:0]=10）：支持1.5625Hz~500Hz（无800/3200Hz）
 *    - 具体档位：0000=保留，0001=32kHz（LN），...，1111=500Hz（LP/LN）
 * 3. bit[4]：保留（读写无效）
 */
#define RRD_ICM42688_ACCEL_CONFIG0             0x50  // 加速度计配置0：量程（FS）与采样率（ODR）（手册14.38节）

/* 可配置值（bit[7:0]）：
 * 1. 温度滤波器带宽（TEMP_FILT_BW，bit[7:5]）：
 *    - 000：4000Hz（延迟0.125ms，默认）  001：170Hz（延迟1ms）
 *    - 010：82Hz（延迟2ms）              011：40Hz（延迟4ms）
 *    - 100：20Hz（延迟8ms）              101：10Hz（延迟16ms）
 *    - 110：5Hz（延迟32ms）              111：5Hz（延迟32ms）
 * 2. 陀螺仪UI滤波器阶数（GYRO_UI_FILT_ORD，bit[3:2]）：
 *    - 00：1阶滤波器  01：2阶滤波器  10：3阶滤波器  11：保留
 * 3. 陀螺仪DEC2_M2滤波器阶数（GYRO_DEC2_M2_ORD，bit[1:0]）：
 *    - 00~01：保留  10：3阶滤波器  11：保留
 * 4. bit[4]：保留（读写无效）
 */
#define RRD_ICM42688_GYRO_CONFIG1              0x51  // 陀螺仪配置1：温度滤波器与UI滤波器（手册14.39节）

/* 可配置值（bit[7:0]）：
 * 1. 加速度计UI滤波器带宽（ACCEL_UI_FILT_BW，bit[7:4]）：
 *    配置加速度计UI滤波器的3dB带宽，值的选择与采样率关联，支持"低噪声"和"低延迟"模式，需结合应用需求选择
 * 2. 陀螺仪UI滤波器带宽（GYRO_UI_FILT_BW，bit[3:0]）：
 *    配置陀螺仪UI滤波器的3dB带宽，值的选择与采样率关联，支持"低噪声"和"低延迟"模式，需结合应用需求选择
 * 注：具体带宽数值及对应噪声带宽（NBW）、群延迟（Group Delay）需参考手册详细表格
 */
#define RRD_ICM42688_GYRO_ACCEL_CONFIG0        0x52  // 陀螺仪与加速度计UI滤波器带宽配置（手册对应章节）

/* 可配置值（bit[7:0]）：
 * 1. 加速度计UI滤波器阶数（ACCEL_UI_FILT_ORD，bit[4:3]）：
 *    - 00：1阶滤波器
 *    - 01：2阶滤波器
 *    - 10：3阶滤波器
 *    - 11：保留
 * 2. 加速度计DEC2_M2滤波器阶数（ACCEL_DEC2_M2_ORD，bit[2:1]）：
 *    - 00：保留
 *    - 01：保留
 *    - 10：3阶滤波器
 *    - 11：保留
 * 3. bit[7:5]、bit[0]：保留（读写无效）
 */
#define RRD_ICM42688_ACCEL_CONFIG1             0x53  // 加速度计配置1：UI滤波器与DEC2_M2滤波器阶数（手册14.41节）

/* 可配置值（bit[7:0]）：
 * 1. 时间戳使能（bit[0]）：0=禁用，1=使能（默认）
 * 2. 时间戳FSYNC使能（bit[1]）：0=禁用，1=使能FSYNC时间戳（需配合FIFO_TMST_FSYNC_EN）
 * 3. 时间戳分辨率（bit[3]）：0=1μs（默认），1=16μs
 * 4. bit[7:5]、bit[2]：保留（读写无效）
 */
#define RRD_ICM42688_TMST_CONFIG               0x54  // 时间戳配置寄存器（手册14.42节）

/* 可配置值（bit[7:0]）：
 * 1. DMP省电模式（bit[7]）：0=禁用，1=使能（默认，需配合WOM）
 * 2. 运动功能使能（bit[6:3]）：
 *    - bit[6]：TAP检测使能  bit[5]：计步器使能  bit[4]：倾斜检测使能
 *    - bit[3]：Raise to Wake/Sleep使能
 * 3. DMP采样率（DMP_ODR，bit[1:0]）：
 *    - 00：25Hz  01：保留  10：50Hz  11：保留
 * 4. bit[2]：保留（读写无效）
 */
#define RRD_ICM42688_APEX_CONFIG0              0x56  // APEX配置0：运动功能使能（手册14.43节）

/* 可配置值（bit[7:0]）：
 * 1. 高精度模式使能（bit[4]）：0=16位数据，1=20位扩展数据（FIFO增加3字节扩展位）
 * 2. FIFO数据选择（bit[3:0]）：
 *    - bit[3]：FIFO_TMST_FSYNC_EN（时间戳/FSYNC使能）
 *    - bit[2]：FIFO_TEMP_EN（温度数据写入使能）
 *    - bit[1]：FIFO_GYRO_EN（陀螺仪数据写入使能）
 *    - bit[0]：FIFO_ACCEL_EN（加速度计数据写入使能）
 * 3. FIFO部分读取使能（bit[6]）：0=禁用，1=支持断点续读
 * 4. FIFO水印触发方式（bit[5]）：0=≥阈值触发，1=每次ODR触发
 * 5. bit[7]：保留（读写无效）
 * 默认：0x00（所有数据禁用写入）
 */
#define RRD_ICM42688_FIFO_CONFIG1              0x5F  // FIFO配置1：数据写入使能（手册14.45节）

/* 说明：FIFO水印阈值 = (FIFO_CONFIG2 << 4) | (FIFO_CONFIG3 & 0x0F)
 * 范围：0 ~ 4095（FIFO最大深度4096字节）
 * 例：阈值=256 → FIFO_CONFIG2=0x10，FIFO_CONFIG3=0x00
 * 注：FIFO_WM_EN需先置0，再修改阈值；阈值不可设为0
 */
#define RRD_ICM42688_FIFO_CONFIG2              0x60  // FIFO配置2：水印阈值高8位（手册14.46节）
#define RRD_ICM42688_FIFO_CONFIG3              0x61  // FIFO配置3：水印阈值低4位（手册14.47节）

/* 可配置值（bit[7:0]）：
 * 1. 数据就绪中断清除选项（UI_DRDY_INT_CLEAR，bit[5:4]）：
 *    - 00：读状态位清除（默认）
 *    - 01：读状态位清除
 *    - 10：读传感器寄存器清除
 *    - 11：读状态位 AND 读传感器寄存器清除
 * 2. FIFO阈值中断清除选项（FIFO_THS_INT_CLEAR，bit[3:2]）：
 *    - 00：读状态位清除（默认）
 *    - 01：读状态位清除
 *    - 10：读FIFO数据1字节清除
 *    - 11：读状态位 AND 读FIFO数据1字节清除
 * 3. FIFO满中断清除选项（FIFO_FULL_INT_CLEAR，bit[1:0]）：
 *    - 00：读状态位清除（默认）
 *    - 01：读状态位清除
 *    - 10：读FIFO数据1字节清除
 *    - 11：读状态位 AND 读FIFO数据1字节清除
 * 4. bit[7:6]：保留（读写无效）
 */
#define RRD_ICM42688_INT_CONFIG0               0x63  // 中断配置0：中断清除模式（手册14.49节）

/* 可配置值（bit[7:0]）：
 * 1. 中断脉冲时长（INT_TPULSE_DURATION，bit[6]）：
 *    - 0：脉冲时长100μs（仅ODR < 4kHz时可用，默认）
 *    - 1：脉冲时长8μs（ODR ≥ 4kHz时必须使用，ODR < 4kHz时可选）
 * 2. 中断去断言时长禁用（INT_TDEASSERT_DISABLE，bit[5]）：
 *    - 0：去断言时长最小100μs（仅ODR < 4kHz时可用，默认）
 *    - 1：禁用去断言时长（ODR ≥ 4kHz时必须使用，ODR < 4kHz时可选）
 * 3. 异步复位（INT_ASYNC_RESET，bit[4]）：
 *    - 默认值为1，用户需改为0以保证INT1和INT2引脚正常工作
 * 4. bit[7]、bit[3:0]：保留（读写无效）
 */
#define RRD_ICM42688_INT_CONFIG1               0x64  // 中断配置1：中断脉冲与去断言时长（手册14.50节）

/* 可配置值（bit[7:0]）：
 * 1. UI_FSYNC_INT1_EN（bit[6]）：
 *    - 0：UI FSYNC中断不路由到INT1
 *    - 1：UI FSYNC中断路由到INT1
 * 2. PLL_RDY_INT1_EN（bit[5]）：
 *    - 0：PLL就绪中断不路由到INT1
 *    - 1：PLL就绪中断路由到INT1
 * 3. RESET_DONE_INT1_EN（bit[4]）：
 *    - 0：复位完成中断不路由到INT1
 *    - 1：复位完成中断路由到INT1
 * 4. UI_DRDY_INT1_EN（bit[3]）：
 *    - 0：UI数据就绪中断不路由到INT1
 *    - 1：UI数据就绪中断路由到INT1
 * 5. FIFO_THS_INT1_EN（bit[2]）：
 *    - 0：FIFO阈值中断不路由到INT1
 *    - 1：FIFO阈值中断路由到INT1
 * 6. FIFO_FULL_INT1_EN（bit[1]）：
 *    - 0：FIFO满中断不路由到INT1
 *    - 1：FIFO满中断路由到INT1
 * 7. UI_AGC_RDY_INT1_EN（bit[0]）：
 *    - 0：UI AGC就绪中断不路由到INT1
 *    - 1：UI AGC就绪中断路由到INT1
 * 8. bit[7]：保留（读写无效）
 */
#define RRD_ICM42688_INT_SOURCE0               0x65  // 中断源0：INT1基础中断路由使能（手册14.51节）

/* 可配置值（bit[7:0]）：
 * 1. I3C_PROTOCOL_ERROR_INT1_EN（bit[6]）：
 *    - 0：I3C协议错误中断不路由到INT1
 *    - 1：I3C协议错误中断路由到INT1
 * 2. SMD_INT1_EN（bit[3]）：
 *    - 0：显著运动检测（SMD）中断不路由到INT1
 *    - 1：显著运动检测（SMD）中断路由到INT1
 * 3. WOM_Z_INT1_EN（bit[2]）：
 *    - 0：Z轴唤醒（WOM）中断不路由到INT1
 *    - 1：Z轴唤醒（WOM）中断路由到INT1
 * 4. WOM_Y_INT1_EN（bit[1]）：
 *    - 0：Y轴唤醒（WOM）中断不路由到INT1
 *    - 1：Y轴唤醒（WOM）中断路由到INT1
 * 5. WOM_X_INT1_EN（bit[0]）：
 *    - 0：X轴唤醒（WOM）中断不路由到INT1
 *    - 1：X轴唤醒（WOM）中断路由到INT1
 * 6. bit[7]、bit[5:4]：保留（读写无效）
 */
#define RRD_ICM42688_INT_SOURCE1               0x66  // 中断源1：INT1运动类中断路由使能（手册14.52节）

/* 可配置值（bit[7:0]）：
 * 1. UI_FSYNC_INT2_EN（bit[6]）：
 *    - 0：UI FSYNC中断不路由到INT2
 *    - 1：UI FSYNC中断路由到INT2
 * 2. PLL_RDY_INT2_EN（bit[5]）：
 *    - 0：PLL就绪中断不路由到INT2
 *    - 1：PLL就绪中断路由到INT2
 * 3. RESET_DONE_INT2_EN（bit[4]）：
 *    - 0：复位完成中断不路由到INT2
 *    - 1：复位完成中断路由到INT2
 * 4. UI_DRDY_INT2_EN（bit[3]）：
 *    - 0：UI数据就绪中断不路由到INT2
 *    - 1：UI数据就绪中断路由到INT2
 * 5. FIFO_THS_INT2_EN（bit[2]）：
 *    - 0：FIFO阈值中断不路由到INT2
 *    - 1：FIFO阈值中断路由到INT2
 * 6. FIFO_FULL_INT2_EN（bit[1]）：
 *    - 0：FIFO满中断不路由到INT2
 *    - 1：FIFO满中断路由到INT2
 * 7. UI_AGC_RDY_INT2_EN（bit[0]）：
 *    - 0：UI AGC就绪中断不路由到INT2
 *    - 1：UI AGC就绪中断路由到INT2
 * 8. bit[7]：保留（读写无效）
 */
#define RRD_ICM42688_INT_SOURCE3               0x68  // 中断源3：INT2基础中断路由使能（手册14.53节）

/* 可配置值（bit[7:0]）：
 * 1. I3C_PROTOCOL_ERROR_INT2_EN（bit[6]）：
 *    - 0：I3C协议错误中断不路由到INT2
 *    - 1：I3C协议错误中断路由到INT2
 * 2. SMD_INT2_EN（bit[3]）：
 *    - 0：显著运动检测（SMD）中断不路由到INT2
 *    - 1：显著运动检测（SMD）中断路由到INT2
 * 3. WOM_Z_INT2_EN（bit[2]）：
 *    - 0：Z轴唤醒（WOM）中断不路由到INT2
 *    - 1：Z轴唤醒（WOM）中断路由到INT2
 * 4. WOM_Y_INT2_EN（bit[1]）：
 *    - 0：Y轴唤醒（WOM）中断不路由到INT2
 *    - 1：Y轴唤醒（WOM）中断路由到INT2
 * 5. WOM_X_INT2_EN（bit[0]）：
 *    - 0：X轴唤醒（WOM）中断不路由到INT2
 *    - 1：X轴唤醒（WOM）中断路由到INT2
 * 6. bit[7]、bit[5:4]：保留（读写无效）
 */
#define RRD_ICM42688_INT_SOURCE4               0x69  // 中断源4：INT2运动类中断路由使能（手册14.54节）

// *********************************************************************************
// Register Bank1：陀螺仪静态校准与FIFO辅助（需先写REG_BANK_SEL=0x01切换）
// *********************************************************************************

/* 可配置值（bit[7:0]）：
 * 1. 陀螺仪抗混叠滤波器（AAF）使能（bit[1]）：
 *    - 0：使能AAF（默认）  1：禁用AAF
 * 2. 陀螺仪陷波滤波器使能（bit[0]）：
 *    - 0：使能陷波滤波器（默认，抑制MEMS谐振噪声）  1：禁用陷波滤波器
 * 3. bit[7:2]：保留（读写无效）
 */
#define RRD_ICM42688_GYRO_CONFIG_STATIC2       0x0B  // 陀螺仪静态校准2：AAF/陷波滤波器使能（手册15.2节）

/* 说明：三者配合配置陀螺仪AAF带宽，需按手册5.3节公式计算（如带宽42Hz→GYRO_AAF_DELT=1）
 * GYRO_AAF_DELTSQR = (GYRO_AAF_DELT)²，分高4位（0x0E[3:0]）和低8位（0x0D[7:0]）
 * GYRO_AAF_BITSHIFT：由带宽决定（如42Hz→15）
 */
#define RRD_ICM42688_GYRO_CONFIG_STATIC3       0x0C  // 陀螺仪静态校准3：AAF带宽配置（手册15.3节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC4       0x0D  // 陀螺仪静态校准4：AAF_DELTSQR低8位（手册15.4节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC5       0x0E  // 陀螺仪静态校准5：AAF_DELTSQR高4位+BITSHIFT（手册15.5节）

/* 说明：陷波滤波器参数为工厂校准值，用户不建议修改；带宽由GYRO_NF_BW_SEL（bit6:4）配置
 * 带宽档位：0=1449Hz，1=680Hz，...，7=10Hz（数值越小带宽越窄，滤波越强）
 */
#define RRD_ICM42688_GYRO_CONFIG_STATIC6       0x0F  // 陀螺仪X轴陷波滤波器参数（手册15.6节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC7       0x10  // 陀螺仪Y轴陷波滤波器参数（手册15.7节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC8       0x11  // 陀螺仪Z轴陷波滤波器参数（手册15.8节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC9       0x12  // 陀螺仪陷波滤波器选择位（手册15.9节）
#define RRD_ICM42688_GYRO_CONFIG_STATIC10      0x13  // 陀螺仪陷波滤波器带宽（手册15.10节）

/* 说明：自测试响应 = 使能自测试后的值 - 禁用自测试后的值，需在手册规定范围内（如±250dps档→±100~±500 LSB）
 */
#define RRD_ICM42688_XG_ST_DATA                0x5F  // X轴陀螺仪自测试结果：只读（手册15.11节）
#define RRD_ICM42688_YG_ST_DATA                0x60  // Y轴陀螺仪自测试结果：只读（手册15.12节）
#define RRD_ICM42688_ZG_ST_DATA                0x61  // Z轴陀螺仪自测试结果：只读（手册15.13节）

// *********************************************************************************
// Register Bank2：加速度计静态校准（需先写REG_BANK_SEL=0x02切换）
// *********************************************************************************

/* 可配置值（bit[7:0]）：
 * 1. 加速度计AAF使能（bit[0]）：0=使能（默认），1=禁用
 * 2. 加速度计AAF带宽（ACCEL_AAF_DELT，bit[6:1]）：按手册5.3节配置（如42Hz→1）
 * 3. bit[7]：保留（读写无效）
 */
#define RRD_ICM42688_ACCEL_CONFIG_STATIC2      0x03  // 加速度计静态校准2：AAF使能与带宽（手册16.1节）

/* 说明：同陀螺仪AAF配置，ACCEL_AAF_DELTSQR = (ACCEL_AAF_DELT)²
 */
#define RRD_ICM42688_ACCEL_CONFIG_STATIC3      0x04  // 加速度计静态校准3：AAF_DELTSQR低8位（手册16.2节）
#define RRD_ICM42688_ACCEL_CONFIG_STATIC4      0x05  // 加速度计静态校准4：AAF_DELTSQR高4位+BITSHIFT（手册16.3节）

/* 说明：自测试响应 = 使能自测试后的值 - 禁用自测试后的值，需在手册规定范围内（如±2g档→±200~±1000 LSB）
 */
#define RRD_ICM42688_XA_ST_DATA                0x3B  // X轴加速度计自测试结果：只读（手册16.4节）
#define RRD_ICM42688_YA_ST_DATA                0x3C  // Y轴加速度计自测试结果：只读（手册16.5节）
#define RRD_ICM42688_ZA_ST_DATA                0x3D  // Z轴加速度计自测试结果：只读（手册16.6节）

// *********************************************************************************
// Register Bank4：APEX运动参数与用户偏移（需先写REG_BANK_SEL=0x04切换）
// *********************************************************************************
#define RRD_ICM42688_APEX_CONFIG1              0x40  // APEX配置1：计步器低功耗阈值（手册17.1节）
#define RRD_ICM42688_APEX_CONFIG2              0x41  // APEX配置2：计步器幅度阈值（手册17.2节）
#define RRD_ICM42688_APEX_CONFIG3              0x42  // APEX配置3：计步器检测窗口（手册17.3节）
#define RRD_ICM42688_APEX_CONFIG4              0x43  // APEX配置4：倾斜检测延迟（手册17.4节）
#define RRD_ICM42688_APEX_CONFIG5              0x44  // APEX配置5：安装矩阵（手册17.5节）
#define RRD_ICM42688_APEX_CONFIG6              0x45  // APEX配置6：睡眠手势延迟（手册17.6节）
#define RRD_ICM42688_APEX_CONFIG7              0x46  // APEX配置7：Tap检测阈值（手册17.7节）
#define RRD_ICM42688_APEX_CONFIG8              0x47  // APEX配置8：Tap检测窗口（手册17.8节）
#define RRD_ICM42688_APEX_CONFIG9              0x48  // APEX配置9：灵敏度模式（手册17.9节）

#define RRD_ICM42688_ACCEL_WOM_X_THR           0x4A  // X轴唤醒阈值：0~255，单位~3.9mg（手册17.10节）
#define RRD_ICM42688_ACCEL_WOM_Y_THR           0x4B  // Y轴唤醒阈值：同X轴（手册17.11节）
#define RRD_ICM42688_ACCEL_WOM_Z_THR           0x4C  // Z轴唤醒阈值：同X轴（手册17.12节）

/* 说明：用户偏移为12位补码，陀螺仪范围±64dps（分辨率1/32 dps），加速度计范围±1g（分辨率0.5mg）
 * 例：X轴陀螺仪偏移=0x100 → 0x100 / 32 = 8 dps
 */
#define RRD_ICM42688_OFFSET_USER0              0x77  // X轴陀螺仪偏移低8位（手册17.18节）
#define RRD_ICM42688_OFFSET_USER1              0x78  // X轴陀螺仪偏移高4位+Y轴偏移高4位（手册17.19节）
#define RRD_ICM42688_OFFSET_USER2              0x79  // Y轴陀螺仪偏移低8位（手册17.20节）
#define RRD_ICM42688_OFFSET_USER3              0x7A  // Z轴陀螺仪偏移低8位（手册17.21节）
#define RRD_ICM42688_OFFSET_USER4              0x7B  // Z轴陀螺仪偏移高4位+X轴加速度计偏移高4位（手册17.22节）
#define RRD_ICM42688_OFFSET_USER5              0x7C  // X轴加速度计偏移低8位（手册17.23节）
#define RRD_ICM42688_OFFSET_USER6              0x7D  // Y轴加速度计偏移低8位（手册17.24节）
#define RRD_ICM42688_OFFSET_USER7              0x7E  // Y轴加速度计偏移高4位+Z轴加速度计偏移高4位（手册17.25节）
#define RRD_ICM42688_OFFSET_USER8              0x7F  // Z轴加速度计偏移低8位（手册17.26节）

/* 可配置值（bit[2:0]，bit[7:3]为保留位）：
 * - 000：选择 Bank 0（默认）
 * - 001：选择 Bank 1
 * - 010：选择 Bank 2
 * - 011：选择 Bank 3
 * - 100：选择 Bank 4
 * - 101~111：保留（无功能）
 * 注：切换寄存器组后，后续寄存器访问将针对所选 Bank；复位后默认回到 Bank 0
 */
#define RRD_ICM42688_REG_BANK_SEL              0x76  // 寄存器组选择寄存器（所有寄存器组均可访问，手册14.59节）

#endif
