#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <process.h>

#include "fast_math.h"        
#include "fast_math_next.h"   
#include "fast_atan2.h"       

const int WIDTH = 800;
const int HEIGHT = 800;
const int MAX_THREADS = 32;

uint32_t* buffer = nullptr;
BITMAPINFO bmi = {0};
HDC memDC = nullptr;
HBITMAP memBitmap = nullptr;
HBITMAP oldBitmap = nullptr;

bool use_fast = true; 
float time_val = 0.0f;
float calc_time = 0.0f;

struct ThreadData { int y_start; int y_end; float t; };
int num_threads = 16;
HANDLE thread_handles[MAX_THREADS];
HANDLE thread_done_events[MAX_THREADS];
HANDLE start_event = NULL;
HANDLE quit_event = NULL;
ThreadData thread_data[MAX_THREADS];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) { SetEvent(quit_event); PostQuitMessage(0); return 0; }
    else if (uMsg == WM_KEYDOWN && wParam == VK_SPACE) { use_fast = !use_fast; }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 工作线程：执行光线步进 / Worker thread: perform ray marching
unsigned __stdcall worker_thread(void* p) {
    int thread_id = (int)(intptr_t)p;
    while (true) {
        HANDLE events[2] = { start_event, quit_event };
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) break;

        ThreadData* data = &thread_data[thread_id];
        float t = data->t;

        for (int y = data->y_start; y < data->y_end; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                // 1. 构建相机射线 (标准化屏幕坐标) / 1. Construct camera ray (normalized screen coordinates)
                float uv_x = (x - WIDTH * 0.5f) / (HEIGHT * 0.5f);
                float uv_y = (y - HEIGHT * 0.5f) / (HEIGHT * 0.5f);
                
                // 射线方向，Z轴朝前 / Ray direction, Z-axis pointing forward
                float dx = uv_x;
                float dy = uv_y;
                float dz = 1.5f; 
                
                // 射线起点（相机位置稍微推后一点） / Ray origin (camera position pushed back a bit)
                float ox = 0.0f;
                float oy = 0.0f;
                float oz = -6.0f + t * 1.0f; 

                // 1. 解决变黑：让时间永远在一个周期内循环，防止 float 精度溢出！ / 1. Fix black screen: keep time looping within a single period to prevent float precision overflow!
                float cyclic_t = fmodf(t, 628.3185307f); // 相当于 100 * 2*PI / Equivalent to 100 * 2*PI

                // 2. 相机推进，但左右摇晃 / 2. Camera advances, but sways left and right
                float cam_x = fast_sin(cyclic_t * 0.5f) * 1.0f;
                float cam_z = -4.0f + cyclic_t * 0.1f; 
                
                // 累加器 / Accumulator
                float accum_r = 0.0f, accum_g = 0.0f, accum_b = 0.0f;
                float transmittance = 1.0f;

                // 步进循环 / Marching loop
                for (int step = 0; step < 64; ++step) { 
                    float dist = (float)step * 0.1f + 0.1f; 
                    
                    float px = cam_x + uv_x * dist * 1.5f;
                    float py = uv_y * dist * 1.5f;
                    float pz = cam_z + dist; 

                    // 1. 获取当前点的密度 / 1. Get the density at the current point
                    float noise_val = 0.0f;
                    if (use_fast) {
                        float n1 = fast_sin(px * 3.0f + cyclic_t) * fast_cos(py * 3.0f - cyclic_t) * fast_sin(pz * 3.0f);
                        float n2 = fast_sin(px * 6.0f - cyclic_t) * fast_cos(py * 6.0f + cyclic_t) * fast_sin(pz * 6.0f) * 0.5f;
                        float n3 = fast_sin(px * 12.0f) * fast_cos(py * 12.0f) * fast_sin(pz * 12.0f + cyclic_t) * 0.25f;
                        noise_val = n1 + n2 + n3;
                    } else {
                        float n1 = std::sin(px * 3.0f + cyclic_t) * std::cos(py * 3.0f - cyclic_t) * std::sin(pz * 3.0f);
                        float n2 = std::sin(px * 6.0f - cyclic_t) * std::cos(py * 6.0f + cyclic_t) * std::sin(pz * 6.0f) * 0.5f;
                        float n3 = std::sin(px * 12.0f) * std::cos(py * 12.0f) * std::sin(pz * 12.0f + cyclic_t) * 0.25f;
                        noise_val = n1 + n2 + n3;
                    }
                    noise_val = noise_val * 0.5f + 0.5f;
                    
                    // 2. 锐化密度，让云层有“结块”感 / 2. Sharpen density to give the clouds a "clumpy" feel
                    float shape = 1.0f - dist * 0.05f; 
                    float density = fmaxf((noise_val * shape) - 0.5f, 0.0f) * 4.0f; 

                                        if (density > 0.01f) {
                        // ========================================================
                        // 【核心加入】体积光照散射 (Light Scattering) / [Core Addition] Volumetric Light Scattering
                        // 假设光源在右上方 (1.0, 1.0, -1.0) 方向 / Assuming the light source is in the upper right direction (1.0, 1.0, -1.0)
                        // ========================================================
                        float lx = px + 0.2f;
                        float ly = py + 0.2f;
                        float lz = pz - 0.2f;
                        
                        float light_noise = 0.0f;
                        float light_transmittance = 0.0f; // 【修复】提前声明变量 / [Fix] Declare variable in advance

                        if (use_fast) {
                            light_noise = fast_sin(lx * 3.0f + cyclic_t) * fast_cos(ly * 3.0f - cyclic_t) * fast_sin(lz * 3.0f);
                            light_noise += fast_sin(lx * 6.0f - cyclic_t) * fast_cos(ly * 6.0f + cyclic_t) * fast_sin(lz * 6.0f) * 0.5f;
                            light_noise = light_noise * 0.5f + 0.5f;
                            
                            // 计算透光率 (光被挡住的概率) / Calculate light transmittance (probability of light being blocked)
                            light_transmittance = fast_exp(-fmaxf(light_noise - 0.4f, 0.0f) * 10.0f);
                        } else {
                            light_noise = std::sin(lx * 3.0f + cyclic_t) * std::cos(ly * 3.0f - cyclic_t) * std::sin(lz * 3.0f);
                            light_noise += std::sin(lx * 6.0f - cyclic_t) * std::cos(ly * 6.0f + cyclic_t) * std::sin(lz * 6.0f) * 0.5f;
                            light_noise = light_noise * 0.5f + 0.5f;
                            
                            light_transmittance = std::exp(-fmaxf(light_noise - 0.4f, 0.0f) * 10.0f);
                        }

                        // 3. 颜色映射：阳光是暖色，阴影是深蓝色/紫色 / 3. Color mapping: sunlight is warm, shadows are dark blue/purple
                        float c_r = 1.0f * light_transmittance + 0.2f;
                        float c_g = 0.9f * light_transmittance + 0.1f;
                        float c_b = 1.0f - light_transmittance * 0.5f; 

                        // 深度雾化效果：距离越远越偏向深空蓝 / Depth fog effect: the farther the distance, the more it leans towards deep space blue
                        float fog = fminf(dist * 0.1f, 1.0f);
                        c_r = c_r * (1.0f - fog) + 0.1f * fog;
                        c_g = c_g * (1.0f - fog) + 0.2f * fog;
                        c_b = c_b * (1.0f - fog) + 0.8f * fog;

                        // 4. 累加与吸收 / 4. Accumulation and absorption
                        float absorption = use_fast ? fast_exp(-density * 1.5f) : std::exp(-density * 1.5f);
                        
                        accum_r += c_r * density * transmittance;
                        accum_g += c_g * density * transmittance;
                        accum_b += c_b * density * transmittance;

                        transmittance *= absorption;
                        if (transmittance < 0.05f) break;
                    }
                }
                // 色调映射（HDR，让高光更亮） / Tone mapping (HDR, making highlights brighter)
                float r = 1.0f - (use_fast ? fast_exp(-accum_r * 1.5f) : std::exp(-accum_r * 1.5f));
                float g = 1.0f - (use_fast ? fast_exp(-accum_g * 1.5f) : std::exp(-accum_g * 1.5f));
                float b = 1.0f - (use_fast ? fast_exp(-accum_b * 1.5f) : std::exp(-accum_b * 1.5f));

                // 写入颜色 / Write color
                uint8_t r_c = (uint8_t)(fminf(fmaxf(r, 0.0f), 1.0f) * 255.0f);
                uint8_t g_c = (uint8_t)(fminf(fmaxf(g, 0.0f), 1.0f) * 255.0f);
                uint8_t b_c = (uint8_t)(fminf(fmaxf(b, 0.0f), 1.0f) * 255.0f);
                
                buffer[y * WIDTH + x] = (255 << 24) | (r_c << 16) | (g_c << 8) | b_c;
            }
        }
        SetEvent(thread_done_events[thread_id]);
    }
    return 0;
}

// 初始化线程池（保持不变） / Initialize thread pool (keep unchanged)
void init_thread_pool() {
    SYSTEM_INFO sysinfo; GetSystemInfo(&sysinfo);
    num_threads = sysinfo.dwNumberOfProcessors > 16 ? 16 : sysinfo.dwNumberOfProcessors;
    start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    quit_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    int rows_per_thread = HEIGHT / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_done_events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        thread_data[i].y_start = i * rows_per_thread;
        thread_data[i].y_end = (i == num_threads - 1) ? HEIGHT : (i + 1) * rows_per_thread;
        thread_handles[i] = (HANDLE)_beginthreadex(NULL, 0, worker_thread, (void*)(intptr_t)i, 0, NULL);
    }
}

// 渲染帧（保持不变） / Render frame (keep unchanged)
void render_field(HWND hwnd, float fps) {
    LARGE_INTEGER freq, start_t, end_t;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start_t);
    
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].t = time_val;
    }

    SetEvent(start_event);
    WaitForMultipleObjects(num_threads, thread_done_events, TRUE, INFINITE);
    ResetEvent(start_event);

    QueryPerformanceCounter(&end_t);
    calc_time = (float)(end_t.QuadPart - start_t.QuadPart) * 1000.0f / freq.QuadPart;

    SetDIBitsToDevice(memDC, 0, 0, WIDTH, HEIGHT, 0, 0, 0, HEIGHT, buffer, &bmi, DIB_RGB_COLORS);

    char text[256];
    const char* mode = use_fast ? "ZERO-BRANCH-MATH" : "SYSTEM MATH";
    sprintf(text, "%s | Calc Time: %.2f ms | FPS: %.1f | Cores: %d", mode, calc_time, fps, num_threads);
    
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, use_fast ? RGB(0, 255, 100) : RGB(255, 100, 100));
    TextOutA(memDC, 20, 20, text, strlen(text));
    
    SetTextColor(memDC, RGB(200, 200, 200));
    TextOutA(memDC, 20, HEIGHT - 40, "Press [SPACE] to switch Engine", 30);

    HDC hdc = GetDC(hwnd);
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

// 主函数（保持不变） / Main function (keep unchanged)
int main() {
    const char CLASS_NAME[] = "VolumetricDemo";
    WNDCLASS wc = {0}; wc.lpfnWndProc = WindowProc; wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME; RegisterClass(&wc);

    RECT rect = {0, 0, WIDTH, HEIGHT}; AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Zero-Branch Volumetric Raymarching", WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), NULL);

    HDC hdc = GetDC(hwnd); memDC = CreateCompatibleDC(hdc); ReleaseDC(hwnd, hdc);
    
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT; bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; bmi.bmiHeader.biCompression = BI_RGB;
    
    memBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
    oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    init_thread_pool(); 
    ShowWindow(hwnd, SW_SHOW); 
    UpdateWindow(hwnd);

    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency); QueryPerformanceCounter(&lastTime);

    MSG msg = {0}; float fps = 0.0f, fpsTimer = 0.0f; int frames = 0;

    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto cleanup;
            TranslateMessage(&msg); DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&currentTime);
        float dt = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        lastTime = currentTime; if (dt > 0.1f) dt = 0.1f; time_val += dt;

        frames++; fpsTimer += dt;
        if (fpsTimer >= 0.5f) { fps = frames / fpsTimer; frames = 0; fpsTimer = 0.0f; }
        
        render_field(hwnd, fps);
    }

cleanup:
    WaitForMultipleObjects(num_threads, thread_handles, TRUE, INFINITE);
    for (int i = 0; i < num_threads; ++i) CloseHandle(thread_handles[i]);
    SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
    return 0;
}