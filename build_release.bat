@echo off
setlocal
where cmake >nul 2>nul || (echo CMake not found & exit /b 1)
cmake -S . -B build -A Win32
if errorlevel 1 exit /b 1
cmake --build build --config Release
if errorlevel 1 exit /b 1
copy /Y build\Release\HECA_GateOccupancy.dll .\HECA_GateOccupancy.dll
if errorlevel 1 exit /b 1
echo.
echo SUCCESS: HECA_GateOccupancy.dll created in this folder.
pause
