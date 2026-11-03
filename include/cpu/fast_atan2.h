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
#include <cstdint>

constexpr float ATAN2_PI = 3.14159265358979323846f;
constexpr float ATAN2_PI_2 = 1.57079632679489661923f;

inline float sign_bit(float x) {
    union { float f; uint32_t u; } u;
    u.f = x;
    return (float)(u.u >> 31);
}

inline float fast_atan2(float y, float x) {
    float sx = sign_bit(x);
    float sy = sign_bit(y);
    
    float ax = fabsf(x);
    float ay = fabsf(y);
    
    float safe_ax = ax + 1e-30f;
    float safe_ay = ay + 1e-30f;
    
    int cond = (ay > ax);
    float num = cond ? safe_ax : safe_ay;
    float den = cond ? safe_ay : safe_ax;
    
    float t = num / den;
    t = fminf(t, 1.0f); 
    
    float z = t * 2.0f - 1.0f;
    float z2 = z * z; 
    
        float xp_z_1 = z * z;
        float xp_z_2 = xp_z_1 * xp_z_1;
        float xp_z_3 = xp_z_2 * xp_z_2;
    float poly = (((((0.46364764586498014f) + (0.4000000231456264f) * z) + ((-0.08000249532355586f) + (-0.005333493792282668f) * z) * xp_z_1) + (((0.009619559296076807f) + (-0.0024320325117698875f) * z) + ((-0.000524340747174896f) + (0.0005103249800177253f) * z) * xp_z_1) * xp_z_2) + ((-4.1318918374358074e-05f) + (-4.574011240386412e-05f) * z) * xp_z_3);
    
    float base_angle = cond ? (ATAN2_PI_2 - poly) : poly;
    
    float angle = (1.0f - sx) * base_angle + sx * (ATAN2_PI - base_angle);
    
    float final_angle = (1.0f - 2.0f * sy) * angle;
    
    return final_angle;
}
