@echo off
setlocal

set "WINDOWS_PROJECT=%~dp0."
set "WINDOWS_PROJECT=%WINDOWS_PROJECT:\=/%"
set "WSL_DRIVE="
for %%D in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do if /i "%~d0"=="%%D:" set "WSL_DRIVE=%%D"
if not defined WSL_DRIVE (
    echo Could not determine the project drive for WSL.
    exit /b 1
)
set "WSL_PROJECT=/mnt/%WSL_DRIVE%%WINDOWS_PROJECT:~2%"

echo Opening QMK WSL shell. Run sBuild in that shell for each firmware build.
wsl.exe -d QMK -- bash "%WSL_PROJECT%/sBuild.sh" --shell "%WSL_PROJECT%"
if errorlevel 1 (
    echo QMK WSL shell exited with an error.
    exit /b 1
)