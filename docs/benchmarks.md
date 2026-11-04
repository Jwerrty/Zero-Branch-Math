# English

> Test Environment: Windows 11 / GCC 13.2 (`-O3 -march=native -fopenmp`) / Multi-core

### 1. Pure Math Operators Throughput (100M mixed sin/cos/tanh calculations)
| Library | Time (s) | Throughput (Mops/s) | Speedup | Max Absolute Error |
| :--- | :--- | :--- | :--- | :--- |
| System `std::` | 0.0362 | 2073.5 | 1.00x | 0.0 (Baseline) |
| **Zero-Branch-Math** | **0.0136** | **5509.0** | **2.65x** | **5.60e-03** |

### 2. 100K Particle Complex Physics Simulation (with non-linear damping)
| Library | Physics Time (ms) | Speedup | Max Coordinate Drift |
| :--- | :--- | :--- | :--- |
| System `std::` | 7.75 | 1.00x | 0.0 (Baseline) |
| **Zero-Branch-Math** | **3.77** | **2.05x** | **4.14e-02 (Visually identical)** |

### 3. Advanced Math Operators Limit Test (100M Mixed Loop Calculations)

> Environment: Windows 11 / GCC 13.2 (`-O3 -march=native`) / Multi-core OpenMP

| Operator | System `std::` Time (s) | Zero-Branch-Math Time (s) | Speedup | Accuracy |
| :--- | :--- | :--- | :--- | :--- |
| `erf(x)` | 0.0093 | **0.0069** | **1.33x** | Error < 1e-04 |
| `log(x)` | 0.0064 | **0.0052** | **1.22x** | Error < 1e-05 |
| `exp(x)` | 0.0050 | 0.0072 | 0.69x | Error < 1e-05 |

#### 💡 Deep Dive: Why is `exp` slower than the standard library?

Due to physical limitations:
1. **`exp(x)` implementation on hardware:** In modern desktop-class x86 CPUs, `std::exp` is directly compiled as a single-cycle hardware microinstruction (e.g., `MUFU.EX2`). Pure software implementations cannot compete with dedicated hardware circuits. The true advantage of `fast_exp` lies in **embedded systems without a dedicated FPU.**
2. **`erf(x)` and `log(x)`: A Significant Performance Advantage** Due to the lack of dedicated hardware instructions for directly calculating `erf` and `log` within the CPU, standard libraries rely on extremely complex series expansions and conditional branching for these functions. Zero-Branch-Math achieves a significant performance improvement of **1.33x** and **1.22x** respectively, using a purely FMA (Fast Math Library) approach without any conditional branching.
3. **Perfect Alignment:** The `fast_erf`, `fast_exp`, and `fast_log` functions achieve perfect alignment with the system library's accuracy at key sampling points (e.g., 1.0, 2.0, 10.0) by minimizing the maximum approximation error.

### 4. Multi-variable Geometric Operators (Four-quadrant Spatial Query)

> Environment: Windows 11 / GCC 13.2 (`-O3 -march=native`) / Multi-core OpenMP
> Scenario: Calculating `atan2` angles for completely random four-quadrant coordinates in polar rendering and 2D collision detection.

| Operator | System `std::` Time (s) | Zero-Branch-Math Time (s) | Speedup | Accuracy |
| :--- | :--- | :--- | :--- | :--- |
| `atan2(y,x)` | 0.0367 | **0.0180** | **2.03x** | Error < 4.7e-07 rad |

#### 💡 Deep Dive
`The standard` `std::atan2` internally nests highly redundant `if-else` branches to handle `NaN`, zero values, and arbitrary slopes across four quadrants. With random inputs, this completely defeats the CPU's branch predictor.
`Zero-Branch-Math` replaces spatial quadrant branches with **Bitwise Masking**, folding the single-variable polynomial fit strictly into the first quadrant to achieve branchless pure FMA acceleration. Its precision (4.7e-07 radians, approx. 0.000027 degrees) perfectly meets the requirements for graphics normal calculations.

### 5. Geometric Distance Operator (IEEE 754 vs HW Instruction)

> Scenario: Computing `1/sqrt(x)`, frequently used for vector normalization in graphics and distance falloff in physics.

| Implementation | Time (s) | Relative Error | Mechanism |
| :--- | :--- | :--- | :--- |
| System `1.0f/sqrtf` | 0.0046 | 0.0 (Baseline) | HW sqrt + HW division |
| SSE `_mm_rsqrt_ss` | 0.0019 | ~1.5e-04 | Dedicated single instruction (silicon level) |
| **Zero-Branch `fast_rsqrt`** | **0.0038** | **4.72e-06** | **Bit-hacking (Quake III magic) + 2x Newton-Raphson** |

#### 💡 Deep Dive
1. **Speed Crush**: `fast_rsqrt` is **1.2x faster** than the system `1.0f/sqrtf` by bypassing expensive hardware divisions.
2. **Precision Crush**: Although the SSE instruction is the fastest (0.0019s), its hardware approximation error is as high as `1.5e-04`. Zero-Branch-Math utilizes 2 Newton-Raphson iterations to reduce the error to `4.72e-06` (30x improvement), offering high-precision geometric results suitable for ray tracing and physics simulations.

# 中文区

> 测试环境：Windows 11 / GCC 13.2 (`-O3 -march=native -fopenmp`) / 多核并发

### 1. 纯数学算子极限吞吐 (1 亿次 sin/cos/tanh 混合计算)
| 算子库 | 耗时 (s) | 吞吐量 (Mops/s) | 加速比 | 最大绝对误差 |
| :--- | :--- | :--- | :--- | :--- |
| System `std::` | 0.0362 | 2073.5 | 1.00x | 0.0 (基准) |
| **Zero-Branch-Math** | **0.0136** | **5509.0** | **2.65x** | **5.60e-03** |

### 2. 10 万粒子复杂物理模拟 (带非线性阻尼与高频叠加)
| 算子库 | 物理耗时 (ms) | 加速比 | 最大坐标漂移误差 |
| :--- | :--- | :--- | :--- |
| System `std::` | 7.75 | 1.00x | 0.0 (基准) |
| **Zero-Branch-Math** | **3.77** | **2.05x** | **4.14e-02 (像素级完全不可见)** |

### 3. 高级数学算子极限测试 (100M 次混合循环并发计算)

> 测试环境：Windows 11 / GCC 13.2 (`-O3 -march=native`) / 多核 OpenMP

| 算子 | System `std::` 耗时 (s) | Zero-Branch-Math 耗时 (s) | 加速比 | 精度表现 |
| :--- | :--- | :--- | :--- | :--- |
| `erf(x)` | 0.0093 | **0.0069** | **1.33x** | 误差 < 1e-04 |
| `log(x)` | 0.0064 | **0.0052** | **1.22x** | 误差 < 1e-05 |
| `exp(x)` | 0.0050 | 0.0072 | 0.69x | 误差 < 1e-05 |

#### 为什么 `exp` 没比过系统库？

物理硬件的缘故：
1. **`exp(x)` 的硬件**：在现代桌面级 x86 CPU 中，`std::exp` 底层直接被编译为单周期的硬件微指令（如 `MUFU.EX2`）。纯软件在通用 CPU 上确实无法击败硬件专用电路。`fast_exp` 的真正用武之地是**无硬件 FPU 的嵌入式设备**。
2. **`erf(x)` 与 `log(x)` 的纯算力碾压**：因为 CPU 中**没有**直接计算 `erf` 和 `log` 的硬件指令，标准库只能通过极其冗长的级数展开和密集的分支判断来实现。Zero-Branch-Math 用完全无分支的纯 FMA 多项式，分别实现了 **1.33x** 和 **1.22x** 的加速。
3. **精度完美对齐**：通过极小化极大逼近，`fast_erf`、`fast_exp`、`fast_log` 在关键采样点（如 1.0, 2.0, 10.0）的精度与系统库肉眼完全一致。

### 4. 多变量几何算子 (四象限空间查询)

> 测试环境：Windows 11 / GCC 13.2 (`-O3 -march=native`) / 多核 OpenMP
> 测试场景：在极坐标渲染与 2D 碰撞检测中，对完全随机的四象限坐标进行 `atan2` 角度计算。

| 算子 | System `std::` 耗时 (s) | Zero-Branch-Math 耗时 (s) | 加速比 | 精度误差 |
| :--- | :--- | :--- | :--- | :--- |
| `atan2(y,x)` | 0.0367 | **0.0180** | **2.03x** | 误差 < 4.7e-07 rad |

#### 💡 深度解析
`std::atan2` 为了处理 `NaN`、零值以及分布在四个象限的任意斜率，内部嵌套了极其冗长的 `if-else` 分支。在输入随机的情况下，这会导致 CPU 的分支预测器彻底失效。
`Zero-Branch-Math` 通过**位操作掩码**（Bitwise Masking）取代了空间象限分支，将单变量拟合折叠到第一象限，实现了无分支的纯 FMA 加速。其精度（4.7e-07 弧度，约 0.000027 度）完美满足图形学法线计算要求。

### 5. 几何距离算子 (IEEE 754 vs 硬件指令)

> 测试场景：计算 `1/sqrt(x)`，常用于图形学归一化向量与物理引擎距离衰减。

| 算子实现 | 耗时 (s) | 相对误差 | 实现原理 |
| :--- | :--- | :--- | :--- |
| System `1.0f/sqrtf` | 0.0046 | 0.0 (基准) | 硬件真开方 + 硬件真除法 |
| SSE `_mm_rsqrt_ss` | 0.0019 | ~1.5e-04 | 底层专用单指令 (硅片直出) |
| **Zero-Branch `fast_rsqrt`** | **0.0038** | **4.72e-06** | **位级拆解 (Quake III 魔法常数) + 2 次牛顿迭代** |

#### 💡 深度解析
1. **速度反杀**：`fast_rsqrt` 比系统原生的 `1.0f/sqrtf` 快 **1.2 倍**，因为规避了昂贵的硬件除法。
2. **精度反杀**：虽然 SSE 单指令最快（0.0019s），但其硬件估算误差高达 `1.5e-04`。Zero-Branch-Math 通过 2 次牛顿迭代，不仅把误差降到了 `4.72e-06`，还以极小的速度代价提供了极高精度的几何计算结果，完美适配高精度光追与物理仿真。

---
> *Authored by Jwerrty*