@echo off
chcp 65001 >nul
setlocal
echo ===================================================
echo Compiling Extreme Edge-Case Demo...
echo ===================================================

g++ -O3 -march=native  test_operator.cpp -o extreme_edge_case.exe -lgdi32 -luser32 -static -static-libgcc -static-libstdc++ -I../../include

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Compilation failed. Please check if GCC is installed.
    pause
    exit /b
)

echo.
echo [SUCCESS] Compilation succeeded! Launching...
echo.
extreme_edge_case.exe
pause
endlocal