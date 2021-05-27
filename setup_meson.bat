@echo off
cd %~dp0
echo Creating debug configuration...
meson setup debug
echo Creating release configuration...
meson setup release --buildtype=release
echo Done!
pause