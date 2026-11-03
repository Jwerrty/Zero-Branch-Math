#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <omp.h>
#include "../include/cpu/fast_math.h"

using namespace std;
using namespace std::chrono;

const int NUM_PARTICLES = 100000;
const int INNER_LOOPS = 50;

struct Particle { float angle, speed, radius, layer; };

int main() {
    cout << "===========================================" << endl;
    cout << "   Zero-Branch-Math: 100K Particle Physics" << endl;
    cout << "===========================================" << endl;

    vector<Particle> particles(NUM_PARTICLES);
    vector<float> out_x_std(NUM_PARTICLES), out_y_std(NUM_PARTICLES);
    vector<float> out_x_fast(NUM_PARTICLES), out_y_fast(NUM_PARTICLES);

    srand(1337);
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 6.28f - 3.14f;
        float r = 20.0f + (float)rand() / RAND_MAX * 350.0f;
        particles[i] = {theta, 0.005f + 2.0f / (r + 10.0f), r, (float)rand() / RAND_MAX * 10.0f};
    }

    float time_val = 1.0f;

    auto start = high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float theta = particles[i].angle + time_val * particles[i].speed;
        float dummy = 0.0f;
        for(int j = 0; j < INNER_LOOPS; ++j) {
            float phase = theta + j * 0.1f + particles[i].layer;
            dummy += std::sin(phase * 5.0f) * std::cos(phase * 3.0f) * std::tanh(phase);
        }
        out_x_std[i] = std::cos(theta) * (particles[i].radius + dummy);
        out_y_std[i] = std::sin(theta) * (particles[i].radius + dummy);
    }
    auto end = high_resolution_clock::now();
    duration<double, std::milli> time_std = end - start;

    start = high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float theta = particles[i].angle + time_val * particles[i].speed;
        float dummy = 0.0f;
        for(int j = 0; j < INNER_LOOPS; ++j) {
            float phase = theta + j * 0.1f + particles[i].layer;
            dummy += fast_sin(phase * 5.0f) * fast_cos(phase * 3.0f) * fast_tanh(phase);
        }
        out_x_fast[i] = fast_cos(theta) * (particles[i].radius + dummy);
        out_y_fast[i] = fast_sin(theta) * (particles[i].radius + dummy);
    }
    end = high_resolution_clock::now();
    duration<double, std::milli> time_fast = end - start;

    float max_drift = 0.0f;
    for(int i=0; i<NUM_PARTICLES; ++i) {
        float dx = std::abs(out_x_std[i] - out_x_fast[i]);
        float dy = std::abs(out_y_std[i] - out_y_fast[i]);
        float dist = std::sqrt(dx*dx + dy*dy);
        if(dist > max_drift) max_drift = dist;
    }

    cout << fixed << setprecision(2);
    cout << "[Performance]" << endl;
    cout << "System Library (std) : " << time_std.count() << " ms" << endl;
    cout << "Zero-Branch-Math       : " << time_fast.count() << " ms" << endl;
    cout << "🔥 Speedup           : " << time_std.count() / time_fast.count() << "x" << endl;

    cout << "\n[Physical Accuracy]" << endl;
    cout << scientific << setprecision(4);
    cout << "Max Coordinate Drift : " << max_drift << " pixels" << endl;
    cout << "(Due to float32 approximation, < 1e-3 means visually identical)" << endl;

    return 0;
}