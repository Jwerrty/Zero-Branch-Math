#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <omp.h>
#include "fast_math.h"

using namespace std;
using namespace std::chrono;

const int ITERATIONS = 25000000;
const int NUM_DATA = 100000;

int main() {
    cout << "===========================================" << endl;
    cout << "   Zero-Branch-Math: Pure Math Benchmark" << endl;
    cout << "===========================================" << endl;

    vector<float> x_data(NUM_DATA);
    for(int i=0; i<NUM_DATA; ++i) x_data[i] = (float)i / NUM_DATA * 6.28f - 3.14f;

    float sum = 0.0f;
    float sum_fast = 0.0f;
    double total_ops = (double)ITERATIONS * 3.0;

    auto start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum)
    for(int i=0; i<ITERATIONS; ++i) {
        float x = x_data[i % NUM_DATA];
        sum += std::sin(x) + std::cos(x) + std::tanh(x);
    }
    auto end = high_resolution_clock::now();
    duration<double> time_std = end - start;
    
    start = high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_fast)
    for(int i=0; i<ITERATIONS; ++i) {
        float x = x_data[i % NUM_DATA];
        sum_fast += fast_sin(x) + fast_cos(x) + fast_tanh(x);
    }
    end = high_resolution_clock::now();
    duration<double> time_fast = end - start;

    float max_err = 0.0f;
    double rmse = 0.0;
    for(int i=0; i<NUM_DATA; ++i) {
        float x = x_data[i];
        float s_err = std::abs(std::sin(x) - fast_sin(x));
        float c_err = std::abs(std::cos(x) - fast_cos(x));
        float t_err = std::abs(std::tanh(x) - fast_tanh(x));
        
        max_err = std::max({max_err, s_err, c_err, t_err});
        rmse += s_err*s_err + c_err*c_err + t_err*t_err;
    }
    rmse = std::sqrt(rmse / (NUM_DATA * 3));

    cout << fixed << setprecision(4);
    cout << "[Performance]" << endl;
    cout << "System Library (std) : " << time_std.count() << " s" << endl;
    cout << "Zero-Branch-Math       : " << time_fast.count() << " s" << endl;
    cout << "🔥 Speedup           : " << time_std.count() / time_fast.count() << "x" << endl;
    
    cout << "\n[Throughput]" << endl;
    cout << "System Library       : " << (total_ops / time_std.count() / 1e6) << " Mops/s" << endl;
    cout << "Zero-Branch-Math       : " << (total_ops / time_fast.count() / 1e6) << " Mops/s" << endl;

    cout << "\n[Accuracy (vs std)]" << endl;
    cout << scientific << setprecision(4);
    cout << "Max Absolute Error   : " << max_err << endl;
    cout << "Root Mean Sq. Error  : " << rmse << endl;
    
    if(sum == 123.456f) cout << sum << endl; 

    return 0;
}