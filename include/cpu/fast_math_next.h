#pragma once
#include <cmath>
#include <cstdint>

constexpr float LOG2E_F = 1.4426950408889634f;
constexpr float LN2_F = 0.6931471805599453f;


inline float fast_erf(float x) {
    x = fminf(fmaxf(x, -4.0f), 4.0f);
    float z = x * 0.25f;
    float z2 = z * z;
        float xp_z2_1 = z2 * z2;
        float xp_z2_2 = xp_z2_1 * xp_z2_1;
    return ((((4.491501051390836f) + (-22.77004783411526f) * z2) + ((92.50138838799643f) + (-246.39589236815897f) * z2) * xp_z2_1) + (((413.59940027240583f) + (-418.00719923149165f) * z2) + ((231.11292550173138f) + (-53.53324439751516f) * z2) * xp_z2_1) * xp_z2_2) * z;
}

inline float fast_ldexp(float x, int n) {
    union { float f; uint32_t u; } u;
    u.f = x;
    u.u += ((int64_t)n << 23);
    return u.f;
}

inline float fast_exp(float x) {
    x = fminf(fmaxf(x, -87.0f), 88.0f);
    float y = x * LOG2E_F;
    float n = floorf(y + 0.5f);
    float t = y - n; 
    
    float z = t * 2.0f; 
    
    float z2 = z * z;
        float xp_z_1 = z * z;
        float xp_z_2 = xp_z_1 * xp_z_1;
    float poly = ((((0.9999999999999861f) + (0.34657359027994983f) * z) + ((0.060056626754558945f) + (0.006938013600769187f) * z) * xp_z_1) + (((0.0006011305309681288f) + (4.1668448220011496e-05f) * z) + ((2.4144943071528993e-06f) + (1.1826434599269576e-07f) * z) * xp_z_1) * xp_z_2);
    
    return fast_ldexp(poly, (int)n);
}

inline float fast_log(float x) {
    if (x <= 0.0f) return -INFINITY;
    union { float f; uint32_t u; } u;
    u.f = x;
    int E = ((u.u >> 23) & 0xFF) - 127;
    u.u = (u.u & 0x007FFFFF) | 0x3F800000;
    float m = u.f;
    
    float z = (m - 1.5f) * 2.0f;
    float z2 = z * z;
        float xp_z_1 = z * z;
        float xp_z_2 = xp_z_1 * xp_z_1;
        float xp_z_3 = xp_z_2 * xp_z_2;
    float poly = (((((0.5849624734779594f) + (0.48089773596421037f) * z) + ((-0.08014875617170336f) + (0.017817256832335246f) * z) * xp_z_1) + (((-0.0044580250335301665f) + (0.0011682001808822583f) * z) + ((-0.0003192043658402271f) + (0.00011677910931795143f) * z) * xp_z_1) * xp_z_2) + (-3.648790744604744e-05f) * xp_z_3);
    
    return (poly + E) * LN2_F;
}
