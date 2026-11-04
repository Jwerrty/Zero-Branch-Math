#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <omp.h>
#include "fast_math_next.h"

using namespace std;
using namespace std::chrono;

const int ITERATIONS = 25000000;
const int NUM_SAMPLES = 100000;

int main() {
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif

    cout << "===========================================" << endl;
    cout << "   Zero-Branch-Math-Next: erf / exp / log Benchmark" << endl;
    cout << "===========================================" << endl;

    vector<float> data_erf(NUM_SAMPLES), data_exp(NUM_SAMPLES), data_log(NUM_SAMPLES);
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        data_erf[i] = (float)i / NUM_SAMPLES * 8.0f - 4.0f; // [-4, 4]
        data_exp[i] = (float)i / NUM_SAMPLES * 20.0f - 10.0f; // [-10, 10]
        data_log[i] = (float)i / NUM_SAMPLES * 100.0f + 0.1f; // [0.1, 100.1]
    }

    // ==========================================
    // 1. erf 
    // ==========================================
    float sum_erf_std = 0.0f, sum_erf_fast = 0.0f;
    auto start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_erf_std)
    for(int i = 0; i < ITERATIONS; ++i) sum_erf_std += std::erf(data_erf[i % NUM_SAMPLES]);
    auto end = high_resolution_clock::now();
    double time_erf_std = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_erf_fast)
    for(int i = 0; i < ITERATIONS; ++i) sum_erf_fast += fast_erf(data_erf[i % NUM_SAMPLES]);
    end = high_resolution_clock::now();
    double time_erf_fast = duration<double>(end - start).count();

    // ==========================================
    // 2. exp 
    // ==========================================
    float sum_exp_std = 0.0f, sum_exp_fast = 0.0f;
    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_exp_std)
    for(int i = 0; i < ITERATIONS; ++i) sum_exp_std += std::exp(data_exp[i % NUM_SAMPLES]);
    end = high_resolution_clock::now();
    double time_exp_std = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_exp_fast)
    for(int i = 0; i < ITERATIONS; ++i) sum_exp_fast += fast_exp(data_exp[i % NUM_SAMPLES]);
    end = high_resolution_clock::now();
    double time_exp_fast = duration<double>(end - start).count();

    // ==========================================
    // 3. log 
    // ==========================================
    float sum_log_std = 0.0f, sum_log_fast = 0.0f;
    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_log_std)
    for(int i = 0; i < ITERATIONS; ++i) sum_log_std += std::log(data_log[i % NUM_SAMPLES]);
    end = high_resolution_clock::now();
    double time_log_std = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_log_fast)
    for(int i = 0; i < ITERATIONS; ++i) sum_log_fast += fast_log(data_log[i % NUM_SAMPLES]);
    end = high_resolution_clock::now();
    double time_log_fast = duration<double>(end - start).count();

    // ==========================================
    // Print results
    // ==========================================
    cout << fixed << setprecision(4);
    cout << "\n[erf(x)]" << endl;
    cout << "std : " << time_erf_std << " s | fast : " << time_erf_fast << " s | Speedup: " << time_erf_std / time_erf_fast << "x" << endl;

    cout << "\n[exp(x)]" << endl;
    cout << "std : " << time_exp_std << " s | fast : " << time_exp_fast << " s | Speedup: " << time_exp_std / time_exp_fast << "x" << endl;

    cout << "\n[log(x)]" << endl;
    cout << "std : " << time_log_std << " s | fast : " << time_log_fast << " s | Speedup: " << time_log_std / time_log_fast << "x" << endl;

    cout << "\n[Accuracy Snippet]" << endl;
    cout << "erf(1.0)  std: " << std::erf(1.0f) << " | fast: " << fast_erf(1.0f) << endl;
    cout << "exp(2.0)  std: " << std::exp(2.0f) << " | fast: " << fast_exp(2.0f) << endl;
    cout << "log(10.0) std: " << std::log(10.0f) << " | fast: " << fast_log(10.0f) << endl;

    return 0;
}