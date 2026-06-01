
<div align="center">
  <img src="./Docs/images/icon.webp" height="150">
  <h1>PeripheralSuite</h1>
  <span>一个轻量级、模块化的嵌入式外设驱动库（C）</span>
</div>
<br>
<div align="center">
  <img src="https://img.shields.io/badge/License-GPLv3-green?logoColor=63%2C%20185%2C%2017&label=license&labelColor=63%2C%20185%2C%2017&color=63%2C%20185%2C%2017">
  <img src="https://img.shields.io/badge/Language-C-green?logoColor=63%2C%20185%2C%2017&labelColor=63%2C%20185%2C%2017&color=63%2C%20185%2C%2017">
</div>
<p align="center">
<a href="Docs/README_en.md">English</a> | <a href="README.md">简体中文</a>
</p>


## 一、项目介绍
**PeripheralSuite** 是一个面向嵌入式平台的外设驱动库，采用纯 C 语言编写。库使用 Interface/Ops 的抽象方式将“驱动逻辑”和“底层硬件读写”解耦，便于在不同 MCU/SDK（HAL/LL/裸机）之间迁移与复用。

## 二、特性
- 纯 C 语言实现，适用于资源受限的嵌入式环境
- Interface/Ops 设计：上层驱动与底层 I2C/SPI/PWM/ADC 适配解耦
- 模块化组织，结构清晰，便于裁剪、移植与扩展
- 内置 stdkit 子模块（Git submodule）

## 三、已支持外设
| 模块 | 通信 | 简述 |
| --- | --- | --- |
| AS5600 | I2C | 12-bit 磁编码器，支持多圈计数、转速、定点/浮点输出 |
| Buzzer | PWM | 蜂鸣器驱动，支持频率与音量控制 |
| ICM42688 | SPI | 6 轴 IMU（三轴陀螺仪 + 三轴加速度计），支持校准 |
| INA226 | I2C | 电流/功率监控，支持电压/电流/功率测量与校准 |
| NTC | ADC | NTC 热敏电阻温度换算（查表） |
| WS2812 | PWM | WS2812 灯带驱动，多灯珠 RGB 控制 |

## 四、目录结构
- `PeripheralSuite/`: 外设驱动库源码（相对仓库根目录）
  - `core/`: 核心库
    - `interface/`: 统一入口（`peripheral_suite.c/h`）
    - `src/`: 各外设驱动实现
    - `include/stdkit/`: stdkit 子模块
    - `CMakeLists.txt`: CMake 构建脚本
- `Docs/`: 文档目录（开发规范、README 规范等）

## 五、快速开始
### 1. 克隆仓库
```bash
git clone https://github.com/Rev-RoastedDuck/PeripheralSuite.git
cd PeripheralSuite
git submodule update --init --recursive
```

### 2. 构建
```bash
cd PeripheralSuite/core
cmake -S . -B build
cmake --build build
```

### 3. 集成到项目
- CMake 集成（推荐）：
```cmake
add_subdirectory(path/to/PeripheralSuite/PeripheralSuite/core peripheralsuite_build)
target_link_libraries(your_target PRIVATE peripheralsuite)
```
- 手动集成：拷贝 `PeripheralSuite/core/` 下的源码与头文件，并配置包含路径：
  - `PeripheralSuite/core/interface`
  - `PeripheralSuite/core/src`
  - `PeripheralSuite/core/include/stdkit/stdkit`

## 六、文档
- 模块 API：见各模块头文件（`PeripheralSuite/core/src/**.h`）
- 规范与提示：见 `Docs/` 目录

## 七、许可证
本项目采用 GNU General Public License v3.0 开源。详情参考 https://www.gnu.org/licenses/gpl-3.0.html
