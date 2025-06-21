@echo off
setlocal


:: Clean previous build
rmdir /S /Q build 2>NUL

:: Configure the project
cmake -B build -S . 



echo.
echo ✅ Build complete.
pause