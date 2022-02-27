@echo off

REM SPDX-License-Identifier: GPL-2.0-or-later
REM Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set PATH=c:\Qt\Static\6.2.2\bin;%PATH%
set PATH=c:\Qt\Tools\mingw900_64\bin;%PATH%

if exist Makefile (
  mingw32-make distclean
  rmdir .generated_files
  rmdir debug
  del log > nul 2>&1
  del lang\*.qm > nul 2>&1
)

del object_script.* > nul 2>&1
del creator.pro.user > nul 2>&1

del release\LibreELEC.USB-SD.Creator.Win32.exe > nul 2>&1
del release\dummy_image_device > nul 2>&1
