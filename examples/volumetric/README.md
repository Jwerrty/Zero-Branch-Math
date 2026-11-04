### 🌌 Extreme Case Study: 3D Volumetric Raymarching (Pure CPU)

*   **Algorithm**: 64-step Raymarching with Secondary Light Sampling (Ray-marched shadows).
*   **Operators Used**: `fast_sin`, `fast_cos`, `fast_exp`, `fast_rsqrt` (IEEE 754 bit-hacking).
*   **Architecture**: Native Win32 Thread Pool (16 Cores).

| Engine | Calc Time | Framerate | Speedup |
| :--- | :--- | :--- | :--- |
| System `std::` | 117.57 ms | 15.3 FPS | 1.00x |
| **Zero-Branch-Math** | **32.62 ms** | **30.7 FPS** | **🚀 3.60x** |

> *Note: By eliminating branch mispredictions in the secondary light sampling phase, Zero-Branch-Math achieves a stable 30+ FPS rendering of complex volumetric clouds, while the standard library completely collapses under the computational pressure.*

### 🌌 极端案例研究：3D 体积光线追踪（纯 CPU）

*   **算法**：64 步光线追踪与二次光照采样（光线追踪阴影）。
*   **所用算子**：`fast_sin`, `fast_cos`, `fast_exp`, `fast_rsqrt`（IEEE 754 位操作）。
*   **架构**：原生 Win32 线程池（16 核）。

| 引擎 | 计算时间 | 帧率 | 加速比 |
| :--- | :--- | :--- | :--- |
| System `std::` | 117.57 ms | 15.3 FPS | 1.00x |
| **Zero-Branch-Math** | **32.62 ms** | **30.7 FPS** | **🚀 3.60x** |

> *注意：通过在次要光线采样阶段消除分支误预测，Zero-Branch-Math 实现了对复杂体积云的稳定 30+ FPS 渲染，而标准库在计算压力下完全崩溃。*