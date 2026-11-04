#include <iostream>
#include <cmath>
#include <chrono>
#include <random>
#include <iomanip>
#include <omp.h>
#include "fast_atan2.h"

using namespace std;
using namespace std::chrono;

const int ITERATIONS = 25000000;
const int NUM_SAMPLES = 100000;

int main() {
    cout << "=== Zero-Branch-Math: atan2 Benchmark ===" << endl;

    vector<float> x_data(NUM_SAMPLES), y_data(NUM_SAMPLES);
    mt19937 gen(1337);
    uniform_real_distribution<float> dist(-100.0f, 100.0f);
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        x_data[i] = dist(gen);
        y_data[i] = dist(gen);
    }

    float sum_std = 0.0f, sum_fast = 0.0f;

    auto start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_std)
    for(int i = 0; i < ITERATIONS; ++i) {
        sum_std += std::atan2(y_data[i % NUM_SAMPLES], x_data[i % NUM_SAMPLES]);
    }
    auto end = high_resolution_clock::now();
    double time_std = duration<double>(end - start).count();

    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_fast)
    for(int i = 0; i < ITERATIONS; ++i) {
        sum_fast += fast_atan2(y_data[i % NUM_SAMPLES], x_data[i % NUM_SAMPLES]);
    }
    end = high_resolution_clock::now();
    double time_fast = duration<double>(end - start).count();

    float max_err = 0.0f;
    for(int i = 0; i < NUM_SAMPLES; ++i) {
        float std_val = std::atan2(y_data[i], x_data[i]);
        float fast_val = fast_atan2(y_data[i], x_data[i]);
        float err = std::abs(std_val - fast_val);
        if (err > max_err) max_err = err;
    }

    cout << fixed << setprecision(4);
    cout << "System std::atan2 : " << time_std << " s" << endl;
    cout << "Zero-Branch-Math atan2   : " << time_fast << " s" << endl;
    cout << "🔥 Speedup        : " << time_std / time_fast << "x" << endl;
    cout << scientific << setprecision(4);
    cout << "Max Error         : " << max_err << " rad" << endl;

    return 0;
}