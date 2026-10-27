@echo off
setlocal

REM ============================================================
REM  Fix UE 5.6 UnrealBuildTool "NETSDK1004" error
REM  (project.assets.json not found -> NuGet restore missing)
REM  Run this as Administrator.
REM ============================================================

set "ENGINE=D:\UE_5.6"
set "UBT=%ENGINE%\Engine\Source\Programs\UnrealBuildTool"

echo === [1/3] Installed .NET SDKs ===
dotnet --list-sdks

echo.
echo === [2/3] Pin .NET 8.0.420 via Engine\Source\global.json ===
> "%ENGINE%\Engine\Source\global.json" echo {"sdk":{"version":"8.0.420"}}
type "%ENGINE%\Engine\Source\global.json"

echo.
echo === [3/3] Restore UnrealBuildTool NuGet packages ===
cd /d "%UBT%"
dotnet restore UnrealBuildTool.csproj --force
if errorlevel 1 (
    echo First restore failed, cleaning obj and retrying...
    rmdir /s /q "%UBT%\obj" 2>nul
    dotnet restore UnrealBuildTool.csproj
)

echo.
echo ============================================================
echo  Done. Reopen Aura.sln and rebuild.
echo  If the error persists, install Visual Studio 2022 (17.x) --
echo  UE 5.6.1 does not support Visual Studio 2026 (v18).
echo ============================================================
pause
