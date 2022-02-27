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

call windows_build_paths.bat

del release\LibreELEC.USB-SD.Creator.Win32.exe > nul 2>&1

call windows_build_prepare_lang.bat

qmake.exe
mingw32-make -j4 release

call windows_build_upx.bat
