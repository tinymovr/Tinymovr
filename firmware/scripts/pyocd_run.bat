@echo off
REM PyOCD wrapper script for flexible discovery
REM Tries pyocd from PATH first, then falls back to project venv

REM Get the workspace root (parent of firmware\scripts)
set "SCRIPT_DIR=%~dp0"
pushd "%SCRIPT_DIR%\..\.."
set "WORKSPACE=%CD%"
popd

REM Try pyocd from PATH first
where pyocd >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    pyocd %*
    exit /b %ERRORLEVEL%
)

REM Try project venv
if exist "%WORKSPACE%\venv\Scripts\pyocd.exe" (
    "%WORKSPACE%\venv\Scripts\pyocd.exe" %*
    exit /b %ERRORLEVEL%
)

echo Error: pyocd not found.
echo Install with: pip install pyocd
echo Or create a virtual environment at %WORKSPACE%\venv with pyocd installed.
exit /b 1
