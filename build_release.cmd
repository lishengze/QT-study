@echo off
title qmake and nmake build prompt
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
set QTDIR=C:\Qt\Qt5.7.1\5.7\\msvc2015_64
set PATH=%VCINSTALLDIR%\bin;%QTDIR%\bin;C:\Program Files\7-Zip;%PATH%
::shadow build
cd ..
set file = build-release-chart-dev
if NOT exist %file% ( mkdir build-release-chart-dev
cd build-release-chart-dev
call "%VCINSTALLDIR%\vcvarsall.bat" amd64
qmake ../chart-dev/layout.pro -spec win32-msvc2015 "CONFIG+=release" "CONFIG+=qml_release"
nmake
cd release
windeployqt layout.exe
layout.exe

