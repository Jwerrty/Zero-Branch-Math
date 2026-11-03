@echo off
chcp 65001 >nul
setlocal
echo ===================================================
echo Compiling Zero-Branch-Math Aurora Demo...
echo ===================================================

g++ -O3 -march=native -ffast-math  demo_aurora.cpp -o demo_aurora.exe -lgdi32 -luser32 -static -static-libgcc -static-libstdc++ -I../../include/cpu

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Compilation failed. Please check if GCC is installed.
    pause
    exit /b
)

echo.
echo [SUCCESS] Compilation succeeded! Launching...
echo.
demo_aurora.exe
pause
endlocal