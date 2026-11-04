#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <omp.h>
#include <xmmintrin.h>
#include "zero_branch_math.h"

using namespace std;
using namespace std::chrono;

const int ITERATIONS = 25000000;
const int NUM_SAMPLES = 100000;

int main() {
    cout << "=== Zero-Branch-Math: fast_rsqrt Benchmark ===" << endl;

    vector<float> x_data(NUM_SAMPLES);
    mt19937 gen(1337);
    uniform_real_distribution<float> dist(0.001f, 10000.0f);
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        x_data[i] = dist(gen);
    }

    float sum_std = 0.0f;
    float sum_sse = 0.0f;
    float sum_fast = 0.0f;

    auto start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_std)
    for(int i = 0; i < ITERATIONS; ++i) {
        volatile float x = x_data[i % NUM_SAMPLES]; // 防止常量折叠
        sum_std += 1.0f / std::sqrt(x);
    }
    auto end = high_resolution_clock::now();
    double time_std = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_sse)
    for(int i = 0; i < ITERATIONS; ++i) {
        volatile float x = x_data[i % NUM_SAMPLES];
        __m128 vx = _mm_set_ss(x);
        __m128 vres = _mm_rsqrt_ss(vx);
        sum_sse += _mm_cvtss_f32(vres);
    }
    end = high_resolution_clock::now();
    double time_sse = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_fast)
    for(int i = 0; i < ITERATIONS; ++i) {
        volatile float x = x_data[i % NUM_SAMPLES];
        sum_fast += fast_rsqrt(x);
    }
    end = high_resolution_clock::now();
    double time_fast = duration<double>(end - start).count();

    float max_rel_err = 0.0f;
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        float x = x_data[i];
        float std_val = 1.0f / std::sqrt(x);
        float fast_val = fast_rsqrt(x);
        
        float rel_err = std::abs((std_val - fast_val) / std_val);
        if (rel_err > max_rel_err) max_rel_err = rel_err;
    }

    cout << fixed << setprecision(4);
    cout << "System 1.0f/sqrtf : " << time_std << " s" << endl;
    cout << "SSE RSQRTSS (HW)  : " << time_sse << " s" << endl;
    cout << "Zero-Branch rsqrt : " << time_fast << " s" << endl;
    
    cout << "\n🔥 Speedup vs System: " << time_std / time_fast << "x" << endl;
    cout << "🔥 Speedup vs SSE HW: " << time_sse / time_fast << "x" << endl;
    
    cout << scientific << setprecision(4);
    cout << "Max Relative Error: " << max_rel_err << endl;

    return 0;
}