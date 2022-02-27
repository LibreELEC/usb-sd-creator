@echo off

REM SPDX-License-Identifier: GPL-2.0-or-later
REM Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set PATH=c:\Qt\Static\6.2.2\bin;%PATH%
set PATH=c:\Qt\Tools\mingw900_64\bin;%PATH%

del release\LibreELEC.USB-SD.Creator.Win32.exe > nul 2>&1

call windows_prepare_lang.bat

qmake.exe
mingw32-make -j4 release
