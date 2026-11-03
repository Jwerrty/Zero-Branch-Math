@echo off
echo Compiling 100K Particle Mandala Demo...
g++ -O3 -march=native -fopenmp -I../../include/cpu demo_win32.cpp -o demo_win32.exe -lgdi32 -luser32 -static
if %errorlevel% neq 0 (
    echo Compilation failed. Please check if GCC is installed and configured in the environment variables.
) else (
    echo Compilation succeeded! Launching...
    demo_win32.exe
)
pause