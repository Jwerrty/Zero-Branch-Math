#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>

// 编译器极速优化属性
#if defined(__clang__)
    #define ZERO_BRANCH_FAST
#elif defined(__GNUC__)
    #define ZERO_BRANCH_FAST __attribute__((optimize("fast-math")))
#else
    #define ZERO_BRANCH_FAST
#endif

// CUDA 兼容宏
#ifndef __CUDACC__
#define __device__
#define __forceinline__ inline
#endif

// =========================================================
// Zero-Branch-Math: Single-Header Extreme Math Library
// =========================================================

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_sin(float x) {
    const float TWO_PI_HI = 6.283185307179586f;
    const float TWO_PI_LO = 2.4492935982947064e-16f;
    float n = floorf(x * 0.15915494309189535f + 0.5f);
    float x_reduced = x - n * TWO_PI_HI - n * TWO_PI_LO;
    const float PI = 3.141592653589793f;
    float sgn = (x_reduced < 0.0f) ? -1.0f : 1.0f;
    float a = fabsf(x_reduced); a = fminf(a, PI - a);
    float t = sgn * a * (1.0f / PI);
    float t_norm = t * 2.0f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_xp_t_norm_1_1 = xp_t_norm_1 * xp_t_norm_1;
    float xp_xp_t_norm_1_2 = xp_xp_t_norm_1_1 * xp_xp_t_norm_1_1;
    float poly = (((((1.570796326794705f) + (-0.6459640974188229f) * xp_t_norm_1) + ((0.07969261903817167f) + (-0.004681700763830354f) * xp_t_norm_1) * xp_xp_t_norm_1_1) + ((0.000160302585647691f) + (-3.4502821984217833e-06f) * xp_t_norm_1) * xp_xp_t_norm_1_2)) * t_norm;
    return poly;
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_cos(float x) {
    const float TWO_PI_HI = 6.283185307179586f;
    const float TWO_PI_LO = 2.4492935982947064e-16f;
    float n = floorf(x * 0.15915494309189535f + 0.5f);
    float x_reduced = x - n * TWO_PI_HI - n * TWO_PI_LO;
    const float PI = 3.141592653589793f;
    const float PI_HALF = 1.5707963267948966f;
    float a = fabsf(x_reduced); int cond = a > PI_HALF;
    float sign = 1.0f - 2.0f * (float)cond;
    a = fminf(a, PI - a); float t = a * (1.0f / PI);
    float t_norm = t * 2.0f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_xp_t_norm_1_1 = xp_t_norm_1 * xp_t_norm_1;
    float xp_xp_t_norm_1_2 = xp_xp_t_norm_1_1 * xp_xp_t_norm_1_1;
    float poly = ((((1.0f) + (-1.2337005501237015f) * xp_t_norm_1) + ((0.25366950490720364f) + (-0.020863333173169924f) * xp_t_norm_1) * xp_xp_t_norm_1_1) + ((0.0009185933625133473f) + (-2.4214972839842205e-05f) * xp_t_norm_1) * xp_xp_t_norm_1_2);
    return sign * poly;
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_exp(float x) {
    if (x != x) return x;
    if (x > 88.72283935546875f) return INFINITY;
    if (x < -103.97208404541016f) return 0.0f;
    const float LOG2E = 1.4426950408889634f;
    float y = x * LOG2E;
    float n = floorf(y + 0.5f);
    float t = y - n;
    float t_norm = t * 2.0f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((1.000000000004541f) + (0.3465735902890194f) * t_norm) + ((0.060056625807462106f) + (0.006938011720099898f) * t_norm) * xp_t_norm_1) + (((0.0006011625785226175f) + (4.172733231412691e-05f) * t_norm) + ((2.365903420097291e-06f) ) * xp_t_norm_1) * xp_t_norm_2) + (((6.125082344559473e-08f) * t_norm) + ((1.748460280931767e-08f) ) * xp_t_norm_1) * xp_t_norm_3);
    int32_t ni = (int32_t)n;
    union { float f; uint32_t u; } u2;
    u2.f = poly;
    if (ni < -126) {
        int shift = -126 - ni; 
        if (shift > 23) return 0.0f; 
        uint32_t frac = u2.u & 0x007FFFFF;
        frac = (frac | 0x00800000) >> shift; 
        u2.u = frac;
        return u2.f;
    }
    u2.u += ((uint32_t)ni << 23);
    return u2.f;
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_log(float x) {
    if (x != x) return x;
    if (x < 0.0f) return NAN;
    if (x == 0.0f) return -INFINITY;
    if (x == INFINITY) return x;
    union { float f; uint32_t u; } u;
    u.f = x;
    int E = ((u.u >> 23) & 0xFF) - 127;
    u.u = (u.u & 0x007FFFFF) | 0x3F800000;
    float t = u.f;
    if (E == -127) {
        x *= 8388608.0f;
        u.f = x;
        E = ((u.u >> 23) & 0xFF) - 127 - 23;
        u.u = (u.u & 0x007FFFFF) | 0x3F800000;
        t = u.f;
    }
    float t_norm = t * 2.0f + -3.0f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.40546510849625195f) + (0.3333333332141449f) * t_norm) + ((-0.05555557387437433f) + (0.012345684638675938f) * t_norm) * xp_t_norm_1) + (((-0.003086261523118384f) + (0.0008229966549807022f) * t_norm) + ((-0.00022916761201536158f) + (6.548827109959978e-05f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + (((-1.817777060075449e-05f) + (5.37603857184901e-06f) * t_norm) + ((-2.3374351983668295e-06f) + (7.114602643480679e-07f) * t_norm) * xp_t_norm_1) * xp_t_norm_3);
    return poly + (float)E * 0.6931471805599453f;
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_tanh(float x) {
    float t = fmaxf(fminf(x, 3.0f), -3.0f);
    float t_norm = t * 0.3333333333333333f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((2.969487847122949f) * t_norm) + ((-7.802402889067842f) * t_norm) * xp_t_norm_1) + (((17.773859639427762f) * t_norm) + ((-24.99501044158473f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + (((18.619411933244926f) * t_norm) + ((-5.572284084070852f) * t_norm) * xp_t_norm_1) * xp_t_norm_3);
    return (x > 3.0f) ? 1.0f : ((x < -3.0f) ? -1.0f : poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_gelu(float x) {
    float t = fmaxf(fminf(x, 6.0f), -6.0f);
    float t_norm = t * 0.16666666666666666f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.022644110266097854f) + (2.999999999999951f) * t_norm) + ((11.824092498638983f) ) * xp_t_norm_1) + (((-37.9505487020952f) ) + ((73.22912775958952f) ) * xp_t_norm_1) * xp_t_norm_2) + (((-67.69931551818196f) ) + ((23.596643910204122f) ) * xp_t_norm_1) * xp_t_norm_3);
    return (x >= 6.0f) ? x : ((x <= -6.0f) ? 0.0f : poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_silu(float x) {
    float t = fmaxf(fminf(x, 5.0f), -5.0f);
    float t_norm = t * 0.2f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.4999999943762416f) + (1.2450487203739289f) * t_norm) + ((5.453336308877366e-07f) + (-2.4241646729607838f) * t_norm) * xp_t_norm_1) + (((-4.272125230018163e-06f) + (4.524714477558919f) * t_norm) + ((1.2134445055266496e-05f) + (-5.655626638343733f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + (((-1.4501559162312277e-05f) + (3.927662214284702f) * t_norm) + ((6.141821316500314e-06f) + (-1.1246624589716325f) * t_norm) * xp_t_norm_1) * xp_t_norm_3);
    return (x > 5.0f) ? x : ((x < -5.0f) ? 0.0f : x * poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_mish(float x) {
    float t = fmaxf(fminf(x, 6.0f), -6.0f);
    float t_norm = t * 0.16666666666666666f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.013283304450491789f) + (3.596967093294224f) * t_norm) + ((9.943569730617462f) + (-3.646889777380505f) * t_norm) * xp_t_norm_1) + (((-26.612499372718577f) + (10.194888144389395f) * t_norm) + ((47.589019407262235f) + (-14.888938274885124f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + (((-42.43316136468314f) + (10.883550673118132f) * t_norm) + ((14.505609329435195f) + (-3.1321875012637905f) * t_norm) * xp_t_norm_1) * xp_t_norm_3);
    return (x > 6.0f) ? x : ((x < -6.0f) ? 0.0f : poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_erf(float x) {
    float t = fmaxf(fminf(x, 4.0f), -4.0f);
    float t_norm = t * 0.25f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((4.4915010391978845f) * t_norm) + ((-22.770048515070297f) * t_norm) * xp_t_norm_1) + (((92.50140058434354f) * t_norm) + ((-246.3959610781379f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + ((((413.5995806869972f) * t_norm) + ((-418.00744260834665f) * t_norm) * xp_t_norm_1) + (((231.11308925140585f) * t_norm) + ((-53.533287980256496f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) * xp_t_norm_3);
    return (x > 4.0f) ? 1.0f : ((x < -4.0f) ? -1.0f : poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_exact_gelu(float x) {
    float t = fmaxf(fminf(x, 5.0f), -5.0f);
    float t_norm = t * 0.2f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.0012177300377380228f) + (2.499999996063396f) * t_norm) + ((9.784427361482084f) + (1.9140030041930625e-07f) * t_norm) * xp_t_norm_1) + (((-36.67623799861022f) + (-2.5248019022558538e-06f) * t_norm) + ((105.89716386016732f) + (1.3911891898122253e-05f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + ((((-192.47088701539772f) + (-3.759972578942133e-05f) * t_norm) + ((208.42682858384723f) + (5.2448623343626516e-05f) * t_norm) * xp_t_norm_1) + (((-122.24262493386405f) + (-3.626539329856535e-05f) * t_norm) + ((29.78132870704269f) + (9.84194212353113e-06f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) * xp_t_norm_3);
    return (x >= 5.0f) ? x : ((x <= -5.0f) ? 0.0f : poly);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_rsqrt_core(float x) {
    float t = fmaxf(fminf(x, 4.0f), 1.0f);
    float t_norm = t * 0.6666666666666666f + -1.6666666666666667f;
    float xp_t_norm_1 = t_norm * t_norm;
    float xp_t_norm_2 = xp_t_norm_1 * xp_t_norm_1;
    float xp_t_norm_3 = xp_t_norm_2 * xp_t_norm_2;
    float poly = (((((0.632459392516f) + (-0.18976327341476107f) * t_norm) + ((0.0851976666063501f) + (-0.04225969637053149f) * t_norm) * xp_t_norm_1) + (((0.02379646133587066f) + (-0.014071473412603662f) * t_norm) + ((0.0031415156389251347f) + (-0.00012091791807628678f) * t_norm) * xp_t_norm_1) * xp_t_norm_2) + ((0.005400294447267144f) + (-0.0037846388840357924f) * t_norm) * xp_t_norm_3);
    return poly;
}


ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_double_mish(float x) {
    return fast_mish(fast_mish(x));
}

constexpr float ATAN2_PI = 3.14159265358979323846f;
constexpr float ATAN2_PI_2 = 1.57079632679489661923f;

ZERO_BRANCH_FAST
__device__ __forceinline__ float sign_bit(float x) {
    union { float f; uint32_t u; } u;
    u.f = x;
    return (float)(u.u >> 31);
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_atan2(float y, float x) {
    float sx = sign_bit(x);
    float sy = sign_bit(y);
    float ax = fabsf(x);
    float ay = fabsf(y);
    
    int cond = (ay > ax);
    float num = cond ? ax : ay;
    float den = cond ? ay : ax;
    
    // 【核心修复】防除零，且不篡改极小值
    float t = (den == 0.0f) ? 0.0f : num / den;
    t = fminf(t, 1.0f); 
    
    float z = t * 2.0f - 1.0f;
    
        float xp_z_1 = z * z;
        float xp_z_2 = xp_z_1 * xp_z_1;
        float xp_z_3 = xp_z_2 * xp_z_2;
    float poly = (((((0.4636476229729696f) + (0.39999999907497025f) * z) + ((-0.08000062863453954f) + (-0.00533328314575659f) * z) * xp_z_1) + (((0.009604370812380029f) + (-0.002432659382369414f) * z) + ((-0.00048200853868669445f) + (0.0005111958918840302f) * z) * xp_z_1) * xp_z_2) + (((-8.92989739588294e-05f) + (-4.632079141927805e-05f) * z) + ((1.9024063767178884e-05f) + (1.500584548954503e-07f) * z) * xp_z_1) * xp_z_3);
    
    float base_angle = cond ? (ATAN2_PI_2 - poly) : poly;
    float angle = (1.0f - sx) * base_angle + sx * (ATAN2_PI - base_angle);
    float final_angle = (1.0f - 2.0f * sy) * angle;
    return final_angle;
}

ZERO_BRANCH_FAST
__device__ __forceinline__ float fast_rsqrt(float x) {
    if (x <= 0.0f) return x == 0.0f ? INFINITY : NAN;
    union { float f; uint32_t i; } conv;
    conv.f = x;
    conv.i = 0x5f3759df - (conv.i >> 1);
    float y = conv.f;
    y = y * (1.5f - 0.5f * x * y * y);
    y = y * (1.5f - 0.5f * x * y * y);
    return y;
}
