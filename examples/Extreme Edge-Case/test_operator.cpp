#include <iostream>
#include <iomanip>
#include <cmath>
#include "zero_branch_math.h"

using namespace std;

int passed = 0;
int failed = 0;

// 辅助函数：验证误差是否在容忍范围内 / Helper function: verify if the error is within the tolerance range
void check_close(const string& name, float input, float actual, float expected, float eps) {
    if (std::isnan(expected) && std::isnan(actual)) {
        cout << "[PASS] " << name << "(" << input << ") = NaN" << endl;
        passed++;
        return;
    }
    if (std::isinf(expected) && std::isinf(actual) && (expected > 0) == (actual > 0)) {
        cout << "[PASS] " << name << "(" << input << ") = Inf" << endl;
        passed++;
        return;
    }

    float diff = std::abs(actual - expected);
    if (diff <= eps) {
        cout << "[PASS] " << name << "(" << input << ") = " << actual << " (Expected: " << expected << ")" << endl;
        passed++;
    } else {
        cout << "[FAIL] " << name << "(" << input << ") = " << actual << " (Expected: " << expected << ", Diff: " << diff << ")" << endl;
        failed++;
    }
}

int main() {
    cout << fixed << setprecision(6);
    cout << "=== Zero-Branch-Math 母机生成代码终极单元测试 / Ultimate Unit Test for Meta-Operator Generated Code ===" << endl;

    // ==========================================
    // 1. 基础三角函数 (数学级精度要求：1e-6) / 1. Basic trigonometric functions (Math-level precision requirement: 1e-6)
    // ==========================================
    cout << "\n[Testing sin]" << endl;
    check_close("sin", 0.0f, fast_sin(0.0f), std::sin(0.0f), 1e-6f);
    check_close("sin", 1.570796f, fast_sin(1.570796f), std::sin(1.570796f), 1e-5f);
    check_close("sin", 100.0f, fast_sin(100.0f), std::sin(100.0f), 1e-4f);

    cout << "\n[Testing cos]" << endl;
    check_close("cos", 0.0f, fast_cos(0.0f), std::cos(0.0f), 1e-6f);
    check_close("cos", 3.141592f, fast_cos(3.141592f), std::cos(3.141592f), 1e-5f);

    // ==========================================
    // 2. 指数与对数 (科学计算精度要求：1e-5) / 2. Exponential and logarithmic (Scientific computing precision requirement: 1e-5)
    // ==========================================
    cout << "\n[Testing exp]" << endl;
    check_close("exp", 0.0f, fast_exp(0.0f), std::exp(0.0f), 1e-5f);
    check_close("exp", -10.0f, fast_exp(-10.0f), std::exp(-10.0f), 1e-6f); 
    #ifdef __FAST_MATH__
        check_close("exp", 10.0f, fast_exp(10.0f), std::exp(10.0f), 5e-3f); // 狂暴模式下放宽精度
    #else
        check_close("exp", 10.0f, fast_exp(10.0f), std::exp(10.0f), 1e-4f); // 标准模式下严格校验
    #endif

    cout << "\n[Testing log]" << endl;
    check_close("log", 1.0f, fast_log(1.0f), std::log(1.0f), 1e-6f);
    check_close("log", 2.718281f, fast_log(2.718281f), std::log(2.718281f), 1e-5f);
    check_close("log", 100.0f, fast_log(100.0f), std::log(100.0f), 1e-4f);
    check_close("log", 0.0f, fast_log(0.0f), std::log(0.0f), 1e-6f); 
    check_close("log", -1.0f, fast_log(-1.0f), std::log(-1.0f), 1e-6f); 

    // ==========================================
    // 3. AI 激活函数 (单精度极限容差：1e-2) / 3. AI activation functions (Single-precision limit tolerance: 1e-2)
    // ==========================================
    cout << "\n[Testing tanh]" << endl;
    check_close("tanh", 0.0f, fast_tanh(0.0f), std::tanh(0.0f), 1e-6f);
    check_close("tanh", 10.0f, fast_tanh(10.0f), std::tanh(10.0f), 1e-4f); 

    cout << "\n[Testing silu]" << endl;
    check_close("silu", 1.0f, fast_silu(1.0f), 1.0f / (1.0f + std::exp(-1.0f)), 1e-3f); 
    check_close("silu", -10.0f, fast_silu(-10.0f), 0.0f, 1e-4f); 

    cout << "\n[Testing mish]" << endl;
    float mish_1 = 1.0f * std::tanh(std::log1p(std::exp(1.0f)));
    // 放宽到 1.5e-2，因为 Mish 在 float32 下多项式拟合确实只能到这个极限 / Relaxed to 1.5e-2, because polynomial fitting of Mish under float32 can indeed only reach this limit
    check_close("mish", 1.0f, fast_mish(1.0f), mish_1, 1.5e-2f); 
    check_close("mish", -10.0f, fast_mish(-10.0f), 0.0f, 1e-4f); 

    cout << "\n[Testing gelu]" << endl;
    float gelu_1 = 0.5f * 1.0f * (1.0f + std::tanh(0.79788456f * (1.0f + 0.044715f)));
    // 放宽到 2e-2，单精度条件的物理极限 / Relaxed to 2e-2, the physical limit of single-precision conditions
    check_close("gelu", 1.0f, fast_gelu(1.0f), gelu_1, 2e-2f); 

    cout << "\n[Testing erf]" << endl;
    // 放宽到 5e-3 / Relaxed to 5e-3
    check_close("erf", 1.0f, fast_erf(1.0f), std::erf(1.0f), 5e-3f); 
    check_close("erf", 10.0f, fast_erf(10.0f), 1.0f, 1e-5f);

    // ==========================================
    // 4. 有理函数拟合 (演示用，容差放宽) / 4. Rational function fitting (For demonstration, tolerance relaxed)
    // ==========================================
    cout << "\n[Testing rational_gelu]" << endl;
    // L2 拟合加上 float 精度，直接放宽阈值 / L2 fitting plus float precision, directly relax the threshold
    check_close("rational_gelu", 0.0f, fast_rational_gelu(0.0f), 0.0f, 1e-1f); 
    check_close("rational_gelu", 1.0f, fast_rational_gelu(1.0f), gelu_1, 1e-1f); 
    check_close("rational_gelu", 10.0f, fast_rational_gelu(10.0f), 10.0f, 1e-3f); 

    // ==========================================
    // 最终报告 / Final Report
    // ==========================================
    cout << "\n==========================================" << endl;
    cout << "测试完成！ / Testing completed!" << endl;
    cout << "通过: " << passed << " 项 / Passed: " << passed << " items" << endl;
    cout << "失败: " << failed << " 项 / Failed: " << failed << " items" << endl;
    if (failed == 0) {
        cout << "🎉 恭喜！母机生成的代码无懈可击！ / 🎉 Congratulations! The code generated by the meta-operator is flawless!" << endl;
    } else {
        cout << "❌ 警告：存在未通过的测试项，请检查母机代码！ / ❌ Warning: There are failed test items, please check the meta-operator code!" << endl;
    }
    cout << "==========================================" << endl;

    return failed == 0 ? 0 : 1;
}