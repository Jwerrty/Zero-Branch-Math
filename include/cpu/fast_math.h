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

constexpr float PI_F = 3.14159265358979323846f;
constexpr float INV_PI_F = 0.31830988618379067154f;


inline float fast_sin(float x) {
    x = x - floorf(x * 0.15915494309f + 0.5f) * 6.28318530718f;
    float z = x * 0.3183098861837907f + -0.0f;
    float z2 = z * z;
        float xp_z2_1 = z2 * z2;
        float xp_z2_2 = xp_z2_1 * xp_z2_1;
    return ((((3.141591543326338f) + (-5.167679802389405f) * z2) + ((2.5498935540784484f) + (-0.5983149800721643f) * z2) * xp_z2_1) + ((0.08051600626810543f) + (-0.006006403726475159f) * z2) * xp_z2_2) * z;
}


inline float fast_cos(float x) {
    x = x - floorf(x * 0.15915494309f + 0.5f) * 6.28318530718f;
    float z = x * 0.3183098861837907f + -0.0f;
    float z2 = z * z;
        float xp_z2_1 = z2 * z2;
        float xp_z2_2 = xp_z2_1 * xp_z2_1;
    return ((((0.9999992413295865f) + (-4.934744593579143f) * z2) + ((4.0580352194963005f) + (-1.332352139363665f) * z2) * xp_z2_1) + ((0.2296315934182805f) + (-0.0205700799703538f) * z2) * xp_z2_2);
}


inline float fast_tanh(float x) {
    x = fminf(fmaxf(x, -5.0f), 5.0f);
    float z = x * 0.2f + -0.0f;
    float z2 = z * z;
        float xp_z2_1 = z2 * z2;
        float xp_z2_2 = xp_z2_1 * xp_z2_1;
    return ((((4.87848542731178f) + (-32.04776030931326f) * z2) + ((167.59407519853764f) + (-541.5724041472291f) * z2) * xp_z2_1) + (((1044.9019106724381f) + (-1167.8683771729968f) * z2) + ((695.6264344034012f) + (-170.51806167419946f) * z2) * xp_z2_1) * xp_z2_2) * z;
}

