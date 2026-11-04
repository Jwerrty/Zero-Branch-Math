@echo off
chcp 65001 >nul
echo ===================================================
echo Compiling Volumetric Raymarching Demo... / 正在编译体积光线步进演示...
echo ===================================================
g++ -O3 -march=native -ffast-math  demo_volumetric.cpp -o demo_volumetric.exe -lgdi32 -luser32 -static -static-libgcc -static-libstdc++ -I../../include/cpu

if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed. Check if GCC is in PATH. / [错误] 编译失败。请检查 GCC 是否在环境变量 PATH 中。
    pause
    exit /b
)

echo [SUCCESS] Compilation succeeded! Launching... / [成功] 编译成功！正在启动...
demo_volumetric.exe
pause