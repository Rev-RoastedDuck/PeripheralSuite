
## 一、Git Commit 规范
### 1. 提交信息结构
```
<类型>(<模块>): <简要描述>

[可选的详细描述]

[可选的参考信息，例如 Issue 编号]
```

### 2. 常见提交类型
| 类型       | 说明        |
| -------- | --------- |
| feat     | 新功能       |
| fix      | 修复 bug    |
| docs     | 文档更新      |
| style    | 代码格式修改    |
| refactor | 代码重构      |
| perf     | 性能优化      |
| test     | 测试相关      |
| chore    | 构建或辅助工具修改、删除旧代码、修改ignore、清理冗余文件、无效能产出、无 BUG 修复的维护性操作(杂活) |
| revert   | 回滚提交      |
| build    | 构建系统修改    |
| ci       | CI/CD 配置  |
| deps     | 依赖更新      |

###  3. 常见模块类型
**全项目通用模块**
| 模块 | 说明 |
| ---- | ---- |
| core | 核心逻辑、核心功能、核心流程 |
| common | 公共组件、通用工具、共享代码 |
| utils | 工具函数、辅助方法、通用工具类 |
| config | 配置文件、参数配置、环境配置 |
| test | 测试用例、单元测试、集成测试 |
| demo | 示例代码、演示程序、使用案例 |
| script | 脚本文件、构建脚本、部署脚本 |
| asset | 资源文件、模型、图片、素材 |
| lib | 第三方库、依赖库、SDK |

---

**网页项目通用模块**
| 模块 | 说明 |
| ---- | ---- |
| web | 前端网页、网站整体 |
| api | 接口请求、后端接口、接口逻辑 |
| server | 服务端、后端服务 |
| client | 客户端、桌面端 |
| ui | 界面、样式、组件 |
| router | 路由、页面跳转 |
| store | 状态管理、数据存储 |

---

**嵌入式**
| 模块 | 说明 |
| ---- | ---- |
| bsp | 板级支持包、底层驱动 |
| driver | 通用驱动（电机、传感器等） |
| mcu | 单片机、主控相关 |
| sensor | 传感器、采样、信号采集 |
| power | 电源电路、供电模块 |
| pcb | PCB设计、布线 |
| schematic | 原理图设计 |

---

**工程/构建/CI通用模块**
| 模块 | 说明 |
| ---- | ---- |
| build | 编译配置、构建流程 |
| ci | 自动化部署、CI/CD配置 |
| dep | 依赖更新、第三方库升级 |
| lint | 代码格式化、语法检查 |

---

**项目专属模块**
| 模块 | 说明 |
| ---- | ---- |
| hw | 硬件电路、PCB、原理图、元器件、电源 |
| sw-main | 软件-主控板程序、主控逻辑、通信、调度 |
| sw-driver | 软件-电驱板、FOC控制、电机驱动、电流采样 |
| mech | 机械结构、3D模型、安装件、机械设计 |
| webots | Webots仿真、仿真场景、模型、控制 |
| arch | 项目架构、目录结构、.gitignore、工程配置 |

### 4. 关联issue
#### 4.1 仅关联不关闭issue
```bash
git commit -m "fix: 修复电流采样异常 (#123)"
```
#### 4.2 自动关闭 issue
```bash
git commit -m "fix: 修复ADC采样抖动 closes #123"
```
等价关键字：
- close / closes / closed
- fix / fixes / fixed
- resolve / resolves / resolved

#### 4.3 多 issue 关联
```bash
git commit -m "fix: 修复多个问题 closes #123 #124 #125"
```
或：
```bash
git commit -m "fix: xxx

closes #123
fixes #124
"
```

### 5. 提交示例
```text
feat(user): add login functionality
```

```text
fix(auth): resolve token expiration issue

The token expiration logic was incorrectly calculating timezones.
This update adjusts the logic to handle UTC offsets properly.

Fixes #123
```

```text
revert: feat(user): add login functionality

This reverts commit abc123456789.
```