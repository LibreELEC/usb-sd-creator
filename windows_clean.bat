@echo off
rem ################################################################################
rem #      This file is part of LibreELEC - http://www.libreelec.tv
rem #      Copyright (C) 2016 Team LibreELEC
rem #
rem #  LibreELEC is free software: you can redistribute it and/or modify
rem #  it under the terms of the GNU General Public License as published by
rem #  the Free Software Foundation, either version 2 of the License, or
rem #  (at your option) any later version.
rem #
rem #  LibreELEC is distributed in the hope that it will be useful,
rem #  but WITHOUT ANY WARRANTY; without even the implied warranty of
rem #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem #  GNU General Public License for more details.
rem #
rem #  You should have received a copy of the GNU General Public License
rem #  along with LibreELEC.  If not, see <http://www.gnu.org/licenses/>.
rem ################################################################################

set PATH=c:\Qt\Qt5.6.1-static\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1\Tools\mingw492_32\bin;%PATH%

if exist Makefile (
  mingw32-make distclean
  rmdir .generated_files
  rmdir debug
  rm -f log
)

del object_script.* > nul 2>&1
del creator.pro.user > nul 2>&1

del release\LibreELEC.USB-SD.Creator.Win32.exe > nul 2>&1
del release\dummy_image_device > nul 2>&1
