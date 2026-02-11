@echo off
setlocal enabledelayedexpansion

set "CLANG_PATH=bin\clang.exe"

for %%f in (*.cpp *.c) do (
    "%CLANG_PATH%" "%%f" -o "%%~nf.exe"
    
    if %errorlevel% equ 0 (
        echo [OK] Created: %%~nf.exe
    ) else (
        echo [FAIL] Error compiling %%f
    )
)

echo Done!
pause
