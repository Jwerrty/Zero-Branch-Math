#include <windows.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <process.h>

#include "zero_branch_math.h"

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int MAX_THREADS = 32;

uint32_t* buffer = nullptr;
BITMAPINFO bmi = {0};
HDC memDC = nullptr;
HBITMAP memBitmap = nullptr;
HBITMAP oldBitmap = nullptr;

bool use_fast = false;
float time_val = 0.0f;
float calc_time = 0.0f;

struct ThreadData {
    int y_start;
    int y_end;
    float cx;
    float cy;
    float time_val;
};

int num_threads = 16;
HANDLE thread_handles[MAX_THREADS];
HANDLE thread_done_events[MAX_THREADS];
HANDLE start_event = NULL;
HANDLE quit_event = NULL;
ThreadData thread_data[MAX_THREADS];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) { 
        SetEvent(quit_event);
        PostQuitMessage(0); 
        return 0; 
    }
    else if (uMsg == WM_KEYDOWN && wParam == VK_SPACE) { use_fast = !use_fast; }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

unsigned __stdcall worker_thread(void* p) {
    int thread_id = (int)(intptr_t)p;
    
    while (true) {
        HANDLE events[2] = { start_event, quit_event };
        DWORD wait_res = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        
        if (wait_res == WAIT_OBJECT_0 + 1) {
            break;
        }

        ThreadData* data = &thread_data[thread_id];
        float cx = data->cx;
        float cy = data->cy;
        float t = data->time_val;

        for (int y = data->y_start; y < data->y_end; ++y) {
            float dy = (y - cy) / 350.0f;
            float dy2 = dy * dy;
            
            if (use_fast) {
                for (int x = 0; x < WIDTH; ++x) {
                    float dx = (x - cx) / 350.0f;
                    float dx2 = dx * dx;
                    float r2 = dx2 + dy2;
                    float r = sqrtf(r2);

                    float clover_sdf = r2 * r - 1.2f * fabsf(dx2 - dy2) - 0.15f;
                    float distort = fast_sin(dx * 6.0f + t) * fast_cos(dy * 6.0f - t);
                    float final_sdf = clover_sdf + distort * 0.05f;
                    float edge = fast_erf(final_sdf * 15.0f); 

                    float intensity = (1.0f - edge) * 0.5f;
                    float color_r = fast_erf(intensity * 4.0f - 1.0f);
                    float color_g = fast_sin(intensity * 3.14f);
                    float color_b = fast_cos(intensity * 3.14f);

                    uint8_t r_c = (uint8_t)(fmaxf(0.0f, color_r) * 255.0f);
                    uint8_t g_c = (uint8_t)(fmaxf(0.0f, color_g) * 255.0f);
                    uint8_t b_c = (uint8_t)(fmaxf(0.0f, color_b) * 255.0f);
                    buffer[y * WIDTH + x] = (255 << 24) | (r_c << 16) | (g_c << 8) | b_c;
                }
            } else {
                for (int x = 0; x < WIDTH; ++x) {
                    float dx = (x - cx) / 350.0f;
                    float dx2 = dx * dx;
                    float r2 = dx2 + dy2;
                    float r = std::sqrt(r2);

                    float clover_sdf = r2 * r - 1.2f * std::abs(dx2 - dy2) - 0.15f;
                    float distort = std::sin(dx * 6.0f + t) * std::cos(dy * 6.0f - t);
                    float final_sdf = clover_sdf + distort * 0.05f;
                    float edge = std::erf(final_sdf * 15.0f); 

                    float intensity = (1.0f - edge) * 0.5f;
                    float color_r = std::erf(intensity * 4.0f - 1.0f);
                    float color_g = std::sin(intensity * 3.14f);
                    float color_b = std::cos(intensity * 3.14f);

                    uint8_t r_c = (uint8_t)(std::max(0.0f, color_r) * 255.0f);
                    uint8_t g_c = (uint8_t)(std::max(0.0f, color_g) * 255.0f);
                    uint8_t b_c = (uint8_t)(std::max(0.0f, color_b) * 255.0f);
                    buffer[y * WIDTH + x] = (255 << 24) | (r_c << 16) | (g_c << 8) | b_c;
                }
            }
        }
        
        SetEvent(thread_done_events[thread_id]);
    }
    return 0;
}

void init_thread_pool() {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    num_threads = sysinfo.dwNumberOfProcessors;
    if (num_threads > 16) num_threads = 16;

    start_event = CreateEvent(NULL, TRUE, FALSE, NULL); // 手动重置
    quit_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    int rows_per_thread = HEIGHT / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        thread_done_events[i] = CreateEvent(NULL, FALSE, FALSE, NULL); // 自动重置
        
        thread_data[i].y_start = i * rows_per_thread;
        thread_data[i].y_end = (i == num_threads - 1) ? HEIGHT : (i + 1) * rows_per_thread;
        
        thread_handles[i] = (HANDLE)_beginthreadex(NULL, 0, worker_thread, (void*)(intptr_t)i, 0, NULL);
    }
}

void render_field(HWND hwnd, float fps) {
    LARGE_INTEGER freq, start_t, end_t;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start_t);
    
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].cx = WIDTH * 0.5f;
        thread_data[i].cy = HEIGHT * 0.5f;
        thread_data[i].time_val = time_val;
    }

    SetEvent(start_event);

    WaitForMultipleObjects(num_threads, thread_done_events, TRUE, INFINITE);

    ResetEvent(start_event);

    QueryPerformanceCounter(&end_t);
    calc_time = (float)(end_t.QuadPart - start_t.QuadPart) * 1000.0f / freq.QuadPart;

    SetDIBitsToDevice(memDC, 0, 0, WIDTH, HEIGHT, 0, 0, 0, HEIGHT, buffer, &bmi, DIB_RGB_COLORS);

    char text[256];
    const char* mode = use_fast ? "ALL Zero-Branch-Math" : "ALL SYSTEM MATH";
    sprintf(text, "%s | Calc Time: %.2f ms | FPS: %.1f | Cores: %d", mode, calc_time, fps, num_threads);
    
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, use_fast ? RGB(0, 255, 100) : RGB(255, 100, 100));
    TextOutA(memDC, 20, 20, text, strlen(text));

    SetTextColor(memDC, RGB(255, 255, 255));
    TextOutA(memDC, 20, HEIGHT - 40, "Press [SPACE] to switch algorithm", 33);

    HDC hdc = GetDC(hwnd);
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

int main() {
    const char CLASS_NAME[] = "CloverDemo";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    RECT rect = {0, 0, WIDTH, HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Zero-Branch-Math Clover SDF Demo",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

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

    init_thread_pool();

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    MSG msg = {0};
    float fps = 0.0f, fpsTimer = 0.0f;
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

        frames++;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) { fps = frames / fpsTimer; frames = 0; fpsTimer = 0.0f; }

        render_field(hwnd, fps);
    }

cleanup:
    WaitForMultipleObjects(num_threads, thread_handles, TRUE, INFINITE);
    for (int i = 0; i < num_threads; ++i) CloseHandle(thread_handles[i]);
    
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
    return 0;
}