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

inline float fast_rsqrt(float x) {
    if (x <= 0.0f) return x == 0.0f ? INFINITY : NAN;
    
    union {
        float f;
        uint32_t i;
    } conv;

    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);
    float y = conv.f;
    
    y = y * (1.5f - 0.5f * x * y * y);
    y = y * (1.5f - 0.5f * x * y * y);
    
    return y;
}
