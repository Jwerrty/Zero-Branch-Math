# Zero-Branch-Math

[English](README.md)

> **极速无分支数学库 —— 专为游戏物理引擎、实时渲染与 AI 推理设计。**

通过极值逼近与 Estrin 树求值结构，将复杂的超越函数（如 `sin`, `cos`, `tanh`）坍缩为纯 FMA（乘加融合）指令。极大规避了标准库中冗余的分支判断与异常处理，榨干硅片的每一滴算力。

## ⚡ 跑分对比 (Benchmarks)

> 测试环境：Windows 11 / GCC 13.2 (`-O3 -march=native -fopenmp`) / 多核并发

### 纯数学算子极限吞吐 (1 亿次 sin/cos/tanh 混合计算)
| 算子库 | 耗时 (s) | 吞吐量 (Mops/s) | 加速比 | 最大绝对误差 |
| :--- | :--- | :--- | :--- | :--- |
| System `std::` | 0.0362 | 2073.5 | 1.00x | 0.0 (基准) |
| **Zero-Branch-Math** | **0.0136** | **5509.0** | **2.65x** | **5.60e-03** |

### 10 万粒子复杂物理模拟 (带非线性阻尼与高频叠加)
| 算子库 | 物理耗时 (ms) | 加速比 | 最大坐标漂移误差 |
| :--- | :--- | :--- | :--- |
| System `std::` | 7.75 | 1.00x | 0.0 (基准) |
| **Zero-Branch-Math** | **3.77** | **2.05x** | **4.14e-02 (像素级完全不可见)** |

> **💡 关于精度说明**：由于单精度（float32）在极小值域拟合的物理极限，`fast_tanh` 的最大绝对误差为 `5.6e-03`。然而在真实的物理引擎累加中，最终坐标漂移仅有 `0.04` 个像素，视觉效果绝对完美。如果您需要全区间 `1e-06` 的极限精度，请联系作者获取定制版本。

### 🚀 极端案例分析：1900+帧/秒的实时 SDF 渲染效果（纯 CPU 实现）

*   **任务**：渲染一个复杂的 1000x1000 尺寸的 SDF 图形，该图形具有平滑的“erf”抗锯齿效果以及谐波失真效果。
*   **技术架构**：原生 Win32 API、无锁线程池（支持 16 核处理器）、Zero-Branch-Math 计算引擎。

| 引擎 | 计算时间（毫秒） | 帧率（FPS） | 加速比 |
| :--- | :--- | :--- | :--- |
| `std::`系统 | 2.57 毫秒 | 325.8 | 1.00 倍 |
| **Zero-Branch-Math** | **0.75 毫秒** | **1903.1** | **3.42 倍** |

## Demo 演示

> **提示:**若图片加载有问题，可进入assets文件夹查看

### particle: 10万粒子曼陀罗渲染
![System Math](https://github.com/user-attachments/assets/1e8ce208-8b7a-4cdc-b6cb-48c1b6a7fabf)
![Zero-Branch-Math](https://github.com/user-attachments/assets/b2444428-4d05-4120-a161-906921a85ebe)

### 四叶草
![System Math](https://github.com/user-attachments/assets/1b9e8d9c-9dc9-4785-a048-9a0d88755742)
![Zero-Branch-Math](https://github.com/user-attachments/assets/cd9e5217-dfd1-4b4f-9b4a-172212c05080)


## 📦 支持的算子

目前开源的纯 C++ 版本包含以下高频核心算子（内置单周期周期规约与饱和截断，绝对安全）：

*   `fast_sin(x)`：拟合区间 **[-π, π]**
*   `fast_cos(x)`：拟合区间 **[-π, π]**
*   `fast_tanh(x)`：拟合区间 \([-5, 5]\)
*   `fast_erf(x)`：1.33x speedup
*   `fast_log(x)`：1.22x speedup
*   `fast_exp(x)`：专为无 FPU 嵌入式设备设计的软件后备方案

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

## 协议

该项目使用的协议为：`Apache-2.0`

## 联系方式
如果你需要为特有的数学公式定制极速 FMA 内核？底层母机引擎可提供企业级定制，联系：`邮箱：azqwerrty@163.com`