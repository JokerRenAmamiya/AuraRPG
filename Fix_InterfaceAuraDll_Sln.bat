@echo off
setlocal

REM ============================================================
REM  Restore the InterfaceAuraDll project into Aura.sln after
REM  UnrealBuildTool regenerates the solution.
REM  See the header of Fix_InterfaceAuraDll_Sln.ps1 for why.
REM  The local execution policy is AllSigned, so this wrapper
REM  calls PowerShell with -ExecutionPolicy Bypass.
REM ============================================================

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Fix_InterfaceAuraDll_Sln.ps1"
set "RC=%ERRORLEVEL%"

echo.
if "%RC%"=="0" (
    echo Done. You can reopen Aura.sln.
) else (
    echo Failed, exit code %RC%.
)
pause
exit /b %RC%
