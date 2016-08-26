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

del release\LibreELEC.USB-SD.Creator.Win32.exe > nul 2>&1

if not exist lang/lang-en_GB.ts (
	rem create new .ts files
	lupdate -verbose creator.pro
)

if not exist lang/lang-en_GB.po (
	rem convert .ts files to new .po files
	lconvert lang/lang-en_GB.ts -o lang/lang-en_GB.po
	lconvert lang/lang-de_DE.ts -o lang/lang-de_DE.po
	lconvert lang/lang-nl_NL.ts -o lang/lang-nl_NL.po
)

rem convert .po files back to .ts files
lconvert -locations relative lang/lang-de_DE.po -o lang/lang-de_DE.ts
lconvert -locations relative lang/lang-nl_NL.po -o lang/lang-nl_NL.ts

rem update .ts files
lupdate -verbose creator.pro

rem convert .ts files to .po files
lconvert lang/lang-en_GB.ts -o lang/lang-en_GB.po
lconvert lang/lang-de_DE.ts -o lang/lang-de_DE.po
lconvert lang/lang-nl_NL.ts -o lang/lang-nl_NL.po

rem create .qm files out of .ts files
lrelease creator.pro

qmake.exe
mingw32-make -j4 release
