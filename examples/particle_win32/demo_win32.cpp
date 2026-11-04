// ===================== 粒子测试配置文件 ============================
// 若暂没有安装 GCC 或 Clang 等编译器，可直接运行同目录下的 demo_win32.exe 文件，或使用 Visual Studio 打开本项目并编译运行。

#include <windows.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "zero_branch_math.h"


const int WIDTH = 800;
const int HEIGHT = 800;
const int NUM_PARTICLES = 100000;

struct Particle {
    float angle;
    float speed;
    float radius;
    float layer;
};

std::vector<Particle> particles(NUM_PARTICLES);
std::vector<float> out_x(NUM_PARTICLES);
std::vector<float> out_y(NUM_PARTICLES);
bool use_fast_math = false;
float time_val = 0.0f;
float calc_time = 0.0f;

uint32_t* buffer = nullptr;
BITMAPINFO bmi = {0};
HDC memDC = nullptr;
HBITMAP memBitmap = nullptr;
HBITMAP oldBitmap = nullptr;

void update_physics() {
    float cx = WIDTH * 0.5f;
    float cy = HEIGHT * 0.5f;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float theta = particles[i].angle + time_val * particles[i].speed;
        float layer = particles[i].layer;
        
        float dummy = 0.0f;
        for(int j = 0; j < 50; ++j) {
            float phase = theta + j * 0.1f + layer;
            float s, c, t;
            if (use_fast_math) {
                s = fast_sin(phase * 5.0f);
                c = fast_cos(phase * 3.0f);
                t = fast_tanh(phase);
            } else {
                s = std::sin(phase * 5.0f);
                c = std::cos(phase * 3.0f);
                t = std::tanh(phase);
            }
            dummy += s * c * t;
        }
        
        float current_radius = particles[i].radius + 15.0f * dummy;
        if (current_radius > 380.0f) current_radius = 380.0f;
        if (current_radius < 10.0f) current_radius = 10.0f;
        
        float final_s = use_fast_math ? fast_sin(theta) : std::sin(theta);
        float final_c = use_fast_math ? fast_cos(theta) : std::cos(theta);
        
        out_x[i] = cx + current_radius * final_c;
        out_y[i] = cy + current_radius * final_s;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end - start;
    calc_time = duration.count();
}

void init_particles() {
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.14159f - 3.14159f;
        float r = 20.0f + (float)rand() / RAND_MAX * 350.0f;
        particles[i].angle = theta;
        particles[i].radius = r;
        particles[i].speed = 0.005f + 2.0f / (r + 10.0f);
        particles[i].layer = (float)rand() / RAND_MAX * 10.0f;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    } else if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_SPACE) {
            use_fast_math = !use_fast_math;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void render_frame(HWND hwnd, float fps) {
    // 清空背景为深紫色
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        buffer[i] = 0x000A0005; // 对应 RGB(5, 0, 10)
    }
    
    float cx = WIDTH * 0.5f;
    float cy = HEIGHT * 0.5f;
    
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        int px = (int)out_x[i];
        int py = (int)out_y[i];
        
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            float dx = px - cx;
            float dy = py - cy;
            
            float r2 = dx*dx + dy*dy + 0.1f; // 防止除零
            float inv_dist = use_fast_math ? fast_rsqrt(r2) : 1.0f / std::sqrt(r2);
            float dist = 1.0f / inv_dist; // 还原真实距离
            
            float angle = use_fast_math ? fast_atan2(dy, dx) : std::atan2(dy, dx);
            
            float norm_dist = std::clamp(dist / 400.0f, 0.0f, 1.0f);
            float fade = 1.0f - norm_dist;
            
            uint8_t r, g, b;
            if (use_fast_math) {
                float s_val = fast_sin(angle * 2.0f + time_val);
                float c_val = fast_cos(angle * 3.0f + time_val);
                r = (uint8_t)((s_val * 0.5f + 0.5f) * 255 * fade);
                g = (uint8_t)((c_val * 0.5f + 0.5f) * 255 * fade);
                b = (uint8_t)(255 * fade);
            } else {
                float s_val = std::sin(angle * 2.0f + time_val);
                float c_val = std::cos(angle * 3.0f + time_val);
                r = (uint8_t)(255 * fade);
                g = (uint8_t)((s_val * 0.5f + 0.5f) * 255 * fade);
                b = (uint8_t)((c_val * 0.5f + 0.5f) * 255 * fade);
            }
            
            if (dist < 50.0f) {
                r = 255; g = 255; b = 255;
            }

            buffer[py * WIDTH + px] = (255 << 24) | (r << 16) | (g << 8) | b;
        }
    }
    
    SetDIBitsToDevice(
        memDC, 0, 0, WIDTH, HEIGHT, 0, 0, 0, HEIGHT,
        buffer, &bmi, DIB_RGB_COLORS
    );

    char text[256];
    const char* mode = use_fast_math ? "FAST MATH" : "SYSTEM MATH";
    sprintf(text, "%s | Calc Time: %.2f ms | FPS: %.1f", mode, calc_time, fps);
    
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, use_fast_math ? RGB(0, 255, 100) : RGB(255, 100, 100));
    TextOutA(memDC, 20, 20, text, strlen(text));

    SetTextColor(memDC, RGB(200, 200, 200));
    TextOutA(memDC, 20, HEIGHT - 40, "Press [SPACE] to switch algorithm", 33);

    HDC hdc = GetDC(hwnd);
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

int main() {
    srand(1337);
    init_particles();

    const char CLASS_NAME[] = "MetaOperatorDemo";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    RECT rect = {0, 0, WIDTH, HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Meta-Operator Extreme Test (Native C++)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    HDC hdc = GetDC(hwnd);
    memDC = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    memBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
    oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    MSG msg = {0};
    float fps = 0.0f;
    float fpsTimer = 0.0f;
    int frames = 0;

    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto cleanup;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&currentTime);
        float dt = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        lastTime = currentTime;
        
        if (dt > 0.1f) dt = 0.1f;
        time_val += dt;

        update_physics();

        frames++;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) {
            fps = frames / fpsTimer;
            frames = 0;
            fpsTimer = 0.0f;
        }

        render_frame(hwnd, fps);
    }

cleanup:
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    return 0;
}