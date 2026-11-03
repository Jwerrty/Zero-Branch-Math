# Zero-Branch-Math

[点击查看中文介绍](README_zh.md)

> **Ultra-Fast Branchless Math Library — Designed for Game Physics Engines, Real-Time Rendering, and AI Inference.**

Through extreme value approximation and Estrin's scheme evaluation structure, complex transcendental functions (such as `sin`, `cos`, `tanh`) are collapsed into pure FMA (Fused Multiply-Add) instructions. By removing slow branch judgments and exception handling found in standard libraries, we maximize instruction-level parallelism (ILP) directly on the silicon.

## ⚡ Benchmarks

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

> **💡 Note on Precision**: Due to the physical limits of single-precision (float32) fitting in specific ranges, the maximum absolute error of `fast_tanh` is `5.6e-03`. However, in the real physics engine accumulation, the final coordinate drift is only `0.04` pixels, making the visual effect perfect. If you require extreme precision of `1e-06` across the full range, please contact the author for a customized version.

### 🚀 Extreme Case Study: 1900+ FPS Real-time SDF Rendering (Pure CPU)

*   **Task**: Render a complex 1000x1000 SDF (Signed Distance Field) Clover with smooth `erf` anti-aliasing and harmonic distortion.
*   **Tech Stack**: Native Win32 API, Lock-free Thread Pool (16 Cores), Zero-Branch-Math.

| Engine | Calc Time (ms) | Framerate (FPS) | Speedup |
| :--- | :--- | :--- | :--- |
| System `std::` | 2.57 ms | 325.8 | 1.00x |
| **Zero-Branch-Math** | **0.75 ms** | **1903.1** | **3.42x** |

## 📽️ Demo Presentation

> **Note:** If images fail to load, please navigate to the `assets` folder in the repository to view them.

### 100,000 Particles Mandala Rendering
![System Math](https://github.com/user-attachments/assets/1e8ce208-8b7a-4cdc-b6cb-48c1b6a7fabf)
![Zero-Branch-Math](https://github.com/user-attachments/assets/b2444428-4d05-4120-a161-906921a85ebe)

### four-leaf-clover
![System Math](https://github.com/user-attachments/assets/1b9e8d9c-9dc9-4785-a048-9a0d88755742)
![Zero-Branch-Math](https://github.com/user-attachments/assets/cd9e5217-dfd1-4b4f-9b4a-172212c05080)


## 📦 Supported Operators

The currently open-sourced pure C++ version includes the following high-frequency core operators (built-in single-cycle range reduction and saturation clamping, absolutely safe):

*   `fast_sin(x)`: Fitted interval **[-π, π]**
*   `fast_cos(x)`: Fitted interval **[-π, π]**
*   `fast_tanh(x)`: Fitted interval \([-5, 5]\)
*   `fast_erf(x)`: 1.33x speedup
*   `fast_log(x)`: 1.22x speed improvement
*   `fast_exp(x)`: A software fallback solution designed specifically for embedded devices without a built-in floating-point unit (FPU).


*(Error controlled at the `1e-06` level, perfectly meeting the needs of graphics and AI inference)*

Next operators to be optimized:

* ` None `


---

## 🛠️ Quick Start

### Run Demo
You can directly run the bat script within the demo.

### ⚙️ Compilation Flags (Crucial)

Because `Zero-Branch-Math` generates highly compressed FMA (Fused Multiply-Add) instructions, **you MUST compile with optimization flags**. Otherwise, the compiler will NOT utilize the CPU's AVX hardware extensions:


**Visual Studio (MSVC):**
Please configure in project properties:
*   C/C++ -> Optimization -> Optimization -> **Maximum Optimization (Favor Speed) (/O2)**
*   C/C++ -> Code Generation -> Enable Enhanced Instruction Set -> **Advanced Vector Extensions (AVX2)**
*   C/C++ -> Code Generation -> Floating Point Model -> **Fast (/fp:fast)**
```
```cpp
#include "fast_math.h"

int main() {
    float angle = 1.234f;
    float s = fast_sin(angle); // Extremely smooth
    float t = fast_tanh(angle);
    return 0;
}
```

---

## 📚 Documentation
*   [Supported Operators](docs/supported_ops.md)
*   [Benchmarks](docs/benchmarks.md)
*   [Architecture & Principles](docs/architecture.md)

---

## Repository Architecture

```text
Zero-Branch-Math/
├── README.md
├── LICENSE
├── .gitignore
│
├── include/                   # [Core] Plug-and-play header files
│   ├── cpu/
│   └── gpu/
│
├── examples/                  # Demo presentation area
│
├── docs/
│   ├── benchmarks.md          # Detailed benchmark description (HW/SW environment, compile flags)
│   ├── supported_ops.md       # All operators list and precision error table
│   └── architecture.md        # In-depth analysis
│
├── benchmarks/                # Test scripts to reproduce benchmarks
│
└── assets/                    # Assets: Demo images
```

## License

The license used by this project is: `Apache-2.0`

## Contact
Are you looking for custom ultra-fast FMA kernels for proprietary math formulas (e.g., specific NN architectures or simulation models)? The underlying E-Meta-Operator code generation engine is available for enterprise licensing. Contact: `Email: azqwerrty@163.com`