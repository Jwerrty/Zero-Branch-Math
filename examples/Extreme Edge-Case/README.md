# Extreme Edge-Case Unit Tests / 极限边界单元测试

> **"Speed means nothing if it crashes in production."**
> **“如果在生产环境中崩溃，速度将毫无意义。”**

This directory contains the ultimate unit test suite for **Zero-Branch-Math**. While benchmarks prove our raw speed, these tests prove our absolute robustness. All 25 extreme edge-case tests must pass flawlessly.
*本目录包含了 Zero-Branch-Math 的最终单元测试套件。跑分证明了我们的绝对速度，而这些测试证明了我们的绝对鲁棒性。所有 25 项极限边界测试必须完美通过。*

## 🏆 Test Results / 测试结果

```text
测试完成！ / Testing completed!
通过: 25 项 / Passed: 25 items
失败: 0 项 / Failed: 0 items
```

## 🚀 Quick Start / 快速开始

Ensure you have a C++ compiler (GCC/Clang) and `zero_branch_math.h` in the same directory.
*确保你安装了 C++ 编译器（GCC/Clang），并将 `zero_branch_math.h` 放在同一目录下。*

**Compile & Run (Windows):**
```bat
compile.bat
```
**Compile & Run (Manual):**
```bash
g++ -O3 -march=native test_operator.cpp -o extreme_edge_case.exe -static
./extreme_edge_case.exe
```

## 🛡️ What Are We Testing? / 我们在测什么？

This suite aggressively attacks the library with pathological inputs to ensure mathematical integrity:
*本测试套件使用病态输入对库进行攻击，以确保数学完整性：*

1.  **Undefined Behavior (UB) & Bit-Shifts**:
    *   Testing `fast_exp(-10.0f)`. Ensures our safe `uint32_t` bit-shifting prevents C++ undefined behavior on negative exponents. (Safe / 安全)
2.  **IEEE 754 Exceptions**:
    *   Testing `fast_log(0.0f)` and `fast_log(-1.0f)`. Ensures it returns exact `-Inf` and `NaN` without polluting subsequent calculations. (Safe / 安全)
3.  **AI Gradient Cliffs**:
    *   Testing `fast_mish(-10.0f)` and `fast_silu(-10.0f)`. Ensures outputs smoothly converge to `0.0` rather than jumping to a discontinuous constant. (Safe / 安全)
4.  **L2 Physical Priors**:
    *   Testing `fast_rational_gelu(0.0f)`. Ensures the rational function strictly outputs `0.0` at the origin. (Safe / 安全)

## ⚠️ A Note on `-ffast-math` / 关于 `-ffast-math` 的特别说明

In `compile.bat`, we **do not** use the `-ffast-math` flag for unit tests. Why? Because `-ffast-math` strips away `NaN`/`Inf` protections, making standard library comparisons invalid.
*在 `compile.bat` 中，我们**没有**为单元测试开启 `-ffast-math` 标志。为什么？因为 `-ffast-math` 会剥离 `NaN`/`Inf` 的保护，导致标准库的对比失效。*

However, **Zero-Branch-Math** uses `__builtin_inff()` and `__builtin_nanf("")` for edge cases. This guarantees 100% IEEE 754 compatibility *even if* you compile your production code with `-ffast-math`.

---
*Happy hacking, and stay safe! / 尽情敲代码吧，保持安全！*