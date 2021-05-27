@echo off
setlocal

cd %~dp0/%1
@rem Combine all args from %2 onwards
for /f "tokens=1,* delims= " %%a in ("%*") do set more=%%b
meson compile %more%
cd ..
endlocal
@echo on