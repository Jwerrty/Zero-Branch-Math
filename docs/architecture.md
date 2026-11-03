# Architecture & Underlying Principles / 架构与底层原理

Zero-Branch-Math is not just a fast math library; it's a physics-driven instruction optimizer. 
*Zero-Branch-Math 不仅是极速数学库，更是物理驱动的指令优化器。*

## 1. Minimax Approximation vs. Taylor Series / 极小化极大逼近 vs 泰勒展开
Standard libraries often use complex range reduction and lookup tables. We use **Remez-style Minimax approximation**. Unlike Taylor series which diverge at the edges, Minimax distributes the error equally across the entire interval, allowing us to achieve `1e-06` precision with a very low-degree polynomial. 
*标准库通常使用复杂的周期规约与查表。我们采用**极小化极大逼近**。与泰勒展开在边缘发散不同，Minimax 将误差均匀分布在整个区间，使我们在极低次数的多项式下即可达到 `1e-06` 的精度。*

## 2. Estrin's Scheme over Horner's Method / Estrin 方案取代霍纳法则
Traditional polynomial evaluation uses Horner's method (`a + x*(b + x*c)`), which creates a long, serial dependency chain. We generate code using **Estrin's scheme**, which computes the polynomial as a balanced binary tree. This dramatically improves **Instruction-Level Parallelism (ILP)**, allowing the CPU's out-of-order engine and SIMD units to execute multiple FMAs simultaneously.
*传统多项式求值使用霍纳法则，这会产生极长的串行依赖链。我们使用 **Estrin 方案**生成代码，将其变为平衡二叉树。这极大提升了**指令级并行 (ILP)**，让 CPU 的乱序执行引擎和 SIMD 单元能同时执行多个 FMA 指令。*

## 3. Branchless Execution / 无分支执行
Standard `std::sin` contains multiple `if-else` branches to handle `NaN`, `Inf`, and extreme values. Branch mispredictions cause massive pipeline stalls. Our operators are **100% branchless** (using single-cycle `fminf/fmaxf` for clamping). This makes them perfectly suited for GPU CUDA cores and CPU vectorization.
*标准 `std::sin` 包含大量处理 `NaN`、`Inf` 和极值的分支。分支预测失败会导致严重的流水线停顿。我们的算子是**100% 无分支**的（使用单周期的 `fminf/fmaxf` 进行截断）。这使其完美契合 GPU CUDA 核心与 CPU 向量化。*

## 4. FMA Fusion / 乘加指令融合
By leveraging the `-O3 -march=native` compiler flags, our generated code is automatically fused into hardware `FMA` (Fused Multiply-Add) instructions. This halves the instruction count and pushes the silicon to its absolute thermal and physical limits.
*通过利用 `-O3 -march=native` 编译选项，我们生成的代码会被自动融合为硬件 `FMA` 指令。这不仅将指令数量减半，更将硅片压榨至绝对的物理极限。*

---
> *Authored by Jwerrty*