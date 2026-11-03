# Zero-Branch-Math

[English](README.md)

> **极速无分支数学库 —— 专为游戏物理引擎、实时渲染与 AI 推理设计。**
>
> 该项目使用的协议为：`Apache-2.0`

通过极值逼近与 Estrin 树求值结构，将复杂的超越函数（如 `sin`, `cos`, `tanh`）坍缩为纯 FMA（乘加融合）指令。极大规避了标准库中冗余的分支判断与异常处理，榨干硅片的每一滴算力。

## ⚡ 跑分对比 (Benchmarks)

> 测试环境：Windows 11 / GCC 13.2 (`-O3 -march=native -fopenmp`) / 多核并发

### 🚀 极端案例分析：1900+帧/秒的实时 SDF 渲染效果（纯 CPU 实现）

*   **任务**：渲染一个复杂的 1000x1000 尺寸的 SDF 图形，具有平滑的 `erf` 抗锯齿效果以及谐波失真效果。
*   **技术架构**：原生 Win32 API、无锁线程池（支持 16 核处理器）、Zero-Branch-Math 计算引擎。

| 引擎 | 计算时间（毫秒） | 帧率（FPS） | 加速比 |
| :--- | :--- | :--- | :--- |
| `std::`系统 | 2.57 ms | 325.8 | 1.00x |
| **Zero-Branch-Math** | **0.75 ms** | **1903.1** | **3.42x** |

<details>
<summary><b>👉 点击展开查看详细基准与物理模拟数据 (Click to expand)</b></summary>

#### 核心算子微基准测试
| 算子 | 耗时 (s) | 加速比 | 精度表现 |
| :--- | :--- | :--- | :--- |
| `sin / cos / tanh` (混合 1 亿次) | 0.0136 (std: 0.0362) | **2.65x** | 误差 < 5.6e-03 |
| `atan2(y,x)` (四象限查询) | 0.0180 (std: 0.0367) | **2.03x** | 误差 < 4.7e-07 rad |
| `erf(x)` | 0.0069 (std: 0.0093) | **1.33x** | 误差 < 1e-04 |
| `log(x)` | 0.0052 (std: 0.0064) | **1.22x** | 误差 < 1e-05 |
| `exp(x)` | 0.0072 (std: 0.0050) | 0.69x | 误差 < 1e-05 (注1) |

#### 2. 10 万粒子复杂物理模拟 (带非线性阻尼与高频叠加)
| 算子库 | 物理耗时 (ms) | 加速比 | 最大坐标漂移误差 |
| :--- | :--- | :--- | :--- |
| System `std::` | 7.75 | 1.00x | 0.0 (基准) |
| **Zero-Branch-Math** | **3.77** | **2.05x** | **4.14e-02 (像素级完全不可见)** |

> **注 1 (`exp` 的特别说明)**：在现代桌面级 x86 CPU 上，`std::exp` 具有底层硬件指令（如 `MUFU.EX2`）加速，纯软件多项式很难在速度上超越硬件。`fast_exp` 主要面向**无硬件 FPU 的嵌入式设备（ARM MCU, RISC-V）**，作为软件后备方案。
> 
> **💡 关于精度说明**：由于单精度（float32）在极小值域拟合的物理极限，`fast_tanh` 的最大绝对误差为 `5.6e-03`。然而在真实的物理引擎累加中，最终坐标漂移仅有 `0.04` 个像素，视觉效果绝对完美。如果您需要全区间 `1e-06` 的极限精度，请联系作者获取定制版本。

</details>

## Demo 演示

> **提示:**若图片加载有问题，可进入assets文件夹查看

### 极光
![aurora](assets/aurora-atan2/aurora.jpg)

<details>
<summary><b>更多图片展示</b></summary>

### atan2
![atan2](assets/atan2/atan2.jpg)

### 10万粒子曼陀罗渲染
![particle](assets/particle/particle.jpg)

### 四叶草
![FourLeafClover](assets/four-leaf-clover/four-leaf-clover.jpg)

</details>

## 📦 支持的算子

目前开源的纯 C++ 版本包含以下高频核心算子（内置单周期周期规约与饱和截断，绝对安全）：

*   `fast_sin(x)`：拟合区间 **[-π, π]**
*   `fast_cos(x)`：拟合区间 **[-π, π]**
*   `fast_tanh(x)`：拟合区间 \([-5, 5]\)
*   `fast_erf(x)`：1.33x speedup
*   `fast_log(x)`：1.22x speedup
*   `fast_exp(x)`：专为无 FPU 嵌入式设备设计的软件后备方案
*   `fast_atan2(x)`

*(误差控制在 `1e-06` 级别，完美满足图形学与 AI 推理需求)*

下一步待优化的算子：

* ` None `

---

## 🛠️ 快速上手

### 运行 demo
可直接运行 demo 内的 bat 脚本

因为 `Zero-Branch-Math` 内部生成了极其精简的 FMA（乘加融合）指令，**请务必使用以下优化参数进行编译**，否则编译器将无法发挥其真实性能（尤其是 `-march=native`，它决定了能否使用您 CPU 最新的 AVX 指令集）：

**Visual Studio (MSVC):**
请在项目属性中设置：
*   C/C++ -> 优化 -> 优化 -> **最大优化 (/O2)** (或 /O3)
*   C/C++ -> 代码生成 -> 启用增强指令集 -> **高级矢量扩展 (AVX2)**
*   C/C++ -> 代码生成 -> 浮点模型 -> **快速 (/fp:fast)**
```
只需将 `include/cpu/fast_math.h` 拷入你的工程，直接 `#include` 即可，无需链接任何第三方动态库：

```cpp
#include "fast_math.h"

int main() {
    float angle = 1.234f;
    float s = fast_sin(angle); // 极致丝滑
    float t = fast_tanh(angle);
    return 0;
}
```

---

## 📚 文档
*   [支持的算子](docs/supported_ops.md)
*   [跑分对比](docs/benchmarks.md)
*   [架构与原理](docs/architecture.md)

---

## 仓库架构

```text
Zero-Branch-Math/
├── README.md
├── LICENSE
├── .gitignore
│
├── include/                   # 【核心】即插即用的头文件
│   ├── cpu/
│   └── gpu/
│
├── examples/                  # Demo 演示区
│
├── docs/
│   ├── benchmarks.md          # 详细跑分说明（软硬件环境、编译参数）
│   ├── supported_ops.md       # 所有算子列表与精度误差表
│   └── architecture.md        # 深度解析
│
├── benchmarks/                # 复现跑分的测试脚本
│
└── assets/                    # 资产：demo出图
```

## 联系方式
如果你需要为特有的数学公式定制极速 FMA 内核？底层母机引擎可提供企业级定制，联系：`邮箱：azqwerrty@163.com`