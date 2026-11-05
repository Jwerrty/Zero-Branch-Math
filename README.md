# Zero-Branch-Math

[简体中文](README_zh.md)

> **Ultra-Fast Branchless Math Library — Designed for Game Physics Engines, Real-Time Rendering, and AI Inference.**
>
> License: `Apache-2.0`

Through extreme value approximation and Estrin's scheme evaluation structure, complex transcendental functions (such as `sin`, `cos`, `tanh`) are collapsed into pure FMA (Fused Multiply-Add) instructions. This significantly avoids redundant branch judgments in the standard library and squeezes every drop of computing power from the silicon.

## ⚡ Core Performance Highlights

> **Environment**: Windows 11 / GCC 13.2 (`-O3 -march=native`) / Native multi-threaded concurrency

Thanks to the minimalist pure FMA architecture and compiler instruction-level optimization, Zero-Branch-Math achieves staggering performance without requiring any changes to global compilation flags:

*   🚀 **Basic Math Functions** (`sin/cos/atan2`): Achieves a **2x ~ 3x** computing power crush compared to `std::`.
*   🚀 **AI Activation Functions** (`erf/gelu/mish`): Eliminates lengthy series expansions, achieving a **1.5x ~ 7x** speedup.
*   🚀 **End-to-End Physics Integration**: Achieves **>2x** frame rate improvement in 100K particle complex simulations.
*   🚀 **Extreme Rendering**: Real-time 1000x1000 SDF rendering on pure CPU, breaking through **1900 FPS**.

*(📊 View detailed timings and precision errors: Refer to [Detailed Benchmarks](docs/benchmarks.md))*

## 🖼️ Visual Demos

> Rendered by Native Win32 API + Zero-Branch-Math, pure CPU computation

### Dynamic Aurora
![aurora](assets/aurora-atan2/aurora.jpg)

<details>
<summary><b>👉 Click to expand more visual demos</b></summary>

### atan2
![atan2](assets/atan2/atan2.jpg)

### 100K Particle Mandala
![particle](assets/particle/particle.jpg)

### Four-Leaf Clover
![FourLeafClover](assets/four-leaf-clover/four-leaf-clover.jpg)

</details>

## 📦 Supported Operators

The open-source version currently includes the following high-frequency core operators (built-in range reduction and branchless saturation clamping):

| Operator | Description |
| :--- | :--- |
| `fast_sin(x)` / `fast_cos(x)` | Ultra-fast trigonometric range reduction |
| `fast_tanh(x)` | Hyperbolic tangent, safe saturation at edges |
| `fast_erf(x)` | AI Error Function, eliminates series expansion |
| `fast_log(x)` | Ultra-fast logarithm, eliminates exception branches |
| `fast_exp(x)` | Software fallback designed for embedded devices without FPUs |
| `fast_atan2(y, x)` | Branchless spatial mapping for all quadrants |
| `fast_rsqrt(x)` | IEEE 754 bit-hacking for 1/√x |
| `fast_gelu(x)` / `fast_silu(x)` / `fast_mish(x)` | AI Core Activation Functions |

*(For more precision testing details, see [Supported Operators](docs/supported_ops.md))*

## 🛠️ Quick Start

Zero-Branch-Math is a **Single-Header** library. Simply copy `include/cpu/zero_branch_math.h` into your project and `#include` it. No third-party dynamic libraries need to be linked.

### Recommended Compilation Flags
To squeeze every drop of vectorized computing power from the CPU, please ensure the following optimization flags are enabled:
*   **GCC/Clang**: `-O3 -march=native`
*   **MSVC**: `/O2` (Maximum Optimization) + `/fp:fast` (Fast Floating Point Model) + `AVX2`

### Example Code
```cpp
#include "zero_branch_math.h"

int main() {
    float angle = 1.234f;
    float s = fast_sin(angle); 
    float t = fast_tanh(angle);
    return 0;
}
```

---

## 📚 Documentation
*   [Supported Operators & Precision List](docs/supported_ops.md)
*   [Detailed Benchmarks Report](docs/benchmarks.md)
*   [Architecture & Underlying Principles](docs/architecture.md)
*   [Extreme Edge-Case Tests (25/25 Passed)](examples/Extreme_Edge-Case/README.md)

---

## 💼 Commercial Cooperation
If you have specific private mathematical formulas (such as option pricing models or specific AI composite operators) that require ultra-fast FMA kernel customization, please contact the author:
**Email: azqwerrty@163.com**