@echo off
setlocal

echo [1/2] Building Release binary and running core tests...
call build.bat
if errorlevel 1 (
    echo Build failed! Aborting installer creation.
    exit /b 1
)

if not exist "dist" mkdir "dist"

echo [2/2] Packaging NSIS Installer...
set MAKENSIS="C:\Program Files (x86)\NSIS\makensis.exe"
if not exist %MAKENSIS% (
    where makensis >nul 2>&1
    if errorlevel 1 (
        echo ERROR: NSIS makensis.exe not found!
        exit /b 1
    )
    set MAKENSIS=makensis
)

%MAKENSIS% installer\cz3000_installer.nsi
if errorlevel 1 (
    echo NSIS installer build failed!
    exit /b 1
)

echo.
echo Installer created successfully in dist\CZ3000SysEx_Setup_v1.0.0.exe
endlocal

