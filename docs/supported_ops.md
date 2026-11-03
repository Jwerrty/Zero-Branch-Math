# Supported Operators / 支持的算子

Zero-Branch-Math provides extreme performance for high-frequency math functions. 
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

---

## ⚠️ Important Notes / 注意事项

1.  **Out of Bounds**: Inputs far outside the valid range will result in polynomial explosion or clamping. Please ensure inputs are normalized. / **越界警告**：超出有效区间的输入会导致多项式爆炸或截断，请确保输入已归一化。
2.  **Commercial Grade Precision**: If your project requires extreme precision (`< 1e-06`) over a much wider range (e.g., `[-20, 20]`), please contact the author for the customized Piecewise version. / **商业级精度**：如果您的项目需要更宽区间及更高精度的支持，请联系作者获取定制的分段拟合商业版。

---
> *Authored by Jwerrty*