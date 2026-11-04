# Supported Operators / 支持的算子

**Zero-Branch-Math** provides extreme performance for high-frequency math functions. 
*Zero-Branch-Math 为高频数学函数提供极致性能。*

## 📌 Core CPU Operators / 核心 CPU 算子

### 1. `fast_sin(float x)`
*   **Description / 描述**: Computes the sine of an angle in radians. / 计算弧度的正弦值。
*   **Valid Range / 有效区间**: `[-π, π]` (Built-in range reduction / 内置周期规约)
*   **Max Error / 最大误差**: `< 1e-06`
*   **Use Case / 适用场景**: Particle physics, orbital mechanics, signal processing. / 粒子物理，轨道力学，信号处理。

### 2. `fast_cos(float x)`
*   **Description / 描述**: Computes the cosine of an angle in radians. / 计算弧度的余弦值。
*   **Valid Range / 有效区间**: `[-π, π]` (Built-in range reduction / 内置周期规约)
*   **Max Error / 最大误差**: `< 1e-06`
*   **Use Case / 适用场景**: 2D/3D rotations, wave simulations. / 2D/3D 旋转，波浪模拟。

### 3. `fast_tanh(float x)`
*   **Description / 描述**: Computes the hyperbolic tangent. / 计算双曲正切。
*   **Valid Range / 有效区间**: `[-5, 5]` (Saturation clamping outside / 区间外自动饱和截断)
*   **Max Error / 最大误差**: `~5.60e-03` (For graphics & AI, visually perfect / 对图形与 AI 而言视觉完美)
*   **Use Case / 适用场景**: Neural network activation (RNN), non-linear damping. / 神经网络激活 (RNN)，非线性阻尼。

### 4. `fast_erf(float x)`
*   **Description / 描述**: Computes the error function, heavily used in AI and statistics. / 计算误差函数，广泛用于 AI 与统计学。
*   **Valid Range / 有效区间**: `[-4, 4]` (Saturation clamping outside / 区间外自动饱和截断)
*   **Max Error / 最大误差**: `< 1e-04`
*   **Speedup / 加速比**: **1.33x** (vs `std::erf`)
*   **Use Case / 适用场景**: Exact GELU activation, Gaussian blur, probability diffusion. / 精确版 GELU 激活，高斯模糊，概率扩散。

### 5. `fast_log(float x)`
*   **Description / 描述**: Computes the natural logarithm. / 计算自然对数。
*   **Valid Range / 有效区间**: `x > 0` (All positive ranges supported via bit-level decomposition / 通过位级拆解支持全正数区间)
*   **Max Error / 最大误差**: `< 1e-05`
*   **Speedup / 加速比**: **1.22x** (vs `std::log`)
*   **Use Case / 适用场景**: Audio decibel calculation, Cross-Entropy loss, HDR tone mapping. / 音频分贝计算，交叉熵损失，HDR 色调映射。

### 6. `fast_exp(float x)`
*   **Description / 描述**: Computes the natural exponential. / 计算自然指数。
*   **Valid Range / 有效区间**: `[-87, 88]` (Hardware float32 overflow limits / 硬件 float32 防溢出限制)
*   **Max Error / 最大误差**: `< 1e-05`
*   **Use Case / 适用场景**: Softmax probability, continuous compounding, physical decay. *Note: Designed as a software fallback for MCUs lacking Hardware FPUs.* / Softmax 概率，连续复利，物理衰减。*注：专为无硬件 FPU 的嵌入式单片机设计的软件后备方案。*

### 7. `fast_atan2(float y, float x)`
*   **Description / 描述**: Computes the 4-quadrant arctangent. / 计算四象限反正切。
*   **Valid Range / 有效区间**: `[-π, π]` (All quadrants / 全四象限)
*   **Max Error / 最大误差**: `< 4.7e-07` rad
*   **Speedup / 加速比**: **2.03x** (vs `std::atan2`)
*   **Use Case / 适用场景**: Polar coordinates, collision detection, shading normals. / 极坐标转换，碰撞检测，光照法线。
*   **Core Highlight / 核心亮点**: Uses bitwise masking to completely eliminate the massive `if-else` branch trees used for quadrant mapping. / 使用位掩码彻底消除了用于象限映射的庞大 `if-else` 分支树。

### 8. `fast_rsqrt(float x)`
*   **Description / 描述**: Computes the reciprocal square root (`1 / √x`). / 计算平方根倒数。
*   **Valid Range / 有效区间**: `x > 0`
*   **Max Error / 最大误差**: `< 5e-06`
*   **Speedup / 加速比**: **1.20x** (vs `1.0f / std::sqrt`)
*   **Use Case / 适用场景**: 3D vector normalization, distance falloff, physics engines. / 3D 向量归一化，距离衰减，物理引擎。
*   **Core Highlight / 核心亮点**: Utilizes IEEE 754 bit-hacking (Quake III magic constant) combined with 2 iterations of Newton-Raphson. Achieves 30x higher precision than SSE hardware instructions. / 利用 IEEE 754 位级黑魔法（Quake III 魔法常数）结合 2 次牛顿迭代，精度比 SSE 硬件指令高 30 倍。

---

## ⚠️ Important Notes / 注意事项

1.  **Out of Bounds**: Inputs far outside the valid range will result in polynomial explosion or clamping. Please ensure inputs are normalized. / **越界警告**：超出有效区间的输入会导致多项式爆炸或截断，请确保输入已归一化。
2.  **Commercial Grade Precision**: If your project requires extreme precision (`< 1e-06`) over a much wider range (e.g., `[-20, 20]`), please contact the author for the customized Piecewise version. / **商业级精度**：如果您的项目需要更宽区间及更高精度的支持，请联系作者获取定制的分段拟合商业版。

---
> *Authored by Jwerrty*