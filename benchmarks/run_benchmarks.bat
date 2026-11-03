@echo off
chcp 65001 >nul
echo 正在使用 GCC 编译性能测试 (开启 -O3 极限优化)...
g++ -O3 -march=native -fopenmp bench_math.cpp -o bench_math.exe -static
g++ -O3 -march=native -fopenmp bench_particle.cpp -o bench_particle.exe -static
g++ -O3 -march=native -fopenmp bench_next_ops.cpp -o bench_next_ops.exe -static
g++ -O3 -march=native -fopenmp bench_atan2.cpp -o bench_atan2.exe -static

echo.
echo ===================================================
echo [Test 1] Pure Math Operators (sin, cos, tanh)
echo ===================================================
bench_math.exe

echo.
echo ===================================================
echo [Test 2] 100K Particle Mandala Physics
echo ===================================================
bench_particle.exe

echo.
echo ===================================================
echo [Test 3] Next Operations (erf, exp, log)
echo ===================================================
bench_next_ops.exe

echo.
echo ===================================================
echo [Test 4] atan2
echo ===================================================
bench_atan2.exe

pause