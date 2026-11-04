/*
 * Zero-Branch-Math (fast_math.h)
 * Copyright (c) 2026 Jwerrty <azqwerrty@163.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <cmath>
#include <stdint.h>

// 提取浮点数的尾数和指数 / Extract the mantissa and exponent of a float
inline float e_frexp(float x, int *E) {
    union { float f; uint32_t u; } u;
    u.f = x; int exp = (u.u >> 23) & 0xFF;
    if (exp == 0) { *E = 0; return x; }
    *E = exp - 127;
    u.u &= 0x807FFFFFUL;
    u.u |= 0x3F800000UL;
    return u.f;
}

// 极速乘 2^n（通过直接修改 IEEE 754 指数位实现，零算力开销） / Fast multiplication by 2^n (implemented by directly modifying IEEE 754 exponent bits, zero computational overhead)
inline float e_ldexp(float x, int n) {
    union { float f; uint32_t u; } u;
    u.f = x; u.u += ((int64_t)n << 23); return u.f;
}

// 快速平方根倒数算法 / Fast inverse square root algorithm
inline float fast_rsqrt(float x) {
    // 1. 硬件级防御（处理 0 和负数） / 1. Hardware-level defensive check (handles 0 and negative numbers)
    if (x <= 0.0f) return x == 0.0f ? INFINITY : NAN;
    
    // 2. 提取尾数 m 和指数 E / 2. Extract mantissa m and exponent E
    int E;
    float m = e_frexp(x, &E); // m 的取值范围是 [1.0, 2.0) / m is in the range [1.0, 2.0)
    
    // 3. 保证 E 为偶数：若 E 为奇数，则将 2 乘入 m，并将 E 减 1 / 3. Ensure E is even: if E is odd, multiply m by 2 and decrement E by 1
    // 这将 m 的映射区间转移至 [1.0, 4.0)，完美契合母机的多项式拟合范围 / This shifts the mapped range of m to [1.0, 4.0), perfectly matching the polynomial fitting range of the meta-operator
    if (E & 1) {
        m *= 2.0f;
        E -= 1;
    }
    
    float t = m;
    float t_norm = t * 0.6666666666666666f + -1.6666666666666667f;
        float xp_t_norm_1 = t_norm * t_norm;
        float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
        float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.632459392516f) + (-0.18976327341476107f) * t_norm) + ((0.0851976666063501f) + (-0.04225969637053149f) * t_norm) * xp_t_norm_1) + (((0.02379646133587066f) + (-0.014071473412603662f) * t_norm) + ((0.0031415156389251347f) + (-0.00012091791807628678f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + ((0.005400294447267144f) + (-0.0037846388840357924f) * t_norm) * xp_t_norm_3);
    
    // 5. 拼装结果： (1/sqrt(m)) * 2^(-E/2) / 5. Reassemble the final result: (1/sqrt(m)) * 2^(-E/2)
    return e_ldexp(poly, -(E / 2));
}