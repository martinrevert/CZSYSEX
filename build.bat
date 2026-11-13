@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "CMAKE_EXE=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

if not exist build mkdir build
cd build

"%CMAKE_EXE%" .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

"%CMAKE_EXE%" --build . --config Release
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

echo Build finished successfully!
Release\test_sysex.exe

