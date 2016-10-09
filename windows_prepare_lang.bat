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

Setlocal EnableDelayedExpansion

set PATH=c:\Qt\Qt5.6.1\5.6\mingw49_32\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1\Tools\mingw492_32\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1-static\bin;%PATH%

echo.
echo ====================================================================

if not exist lang/lang-en_GB.ts (
  rem create new .ts files
  echo.
  echo Creating new .ts files
  lupdate -verbose creator.pro
  echo.
)

if not exist lang/lang-en_GB.po (
  rem convert .ts files to new .po files
  echo.
  for /f "tokens=*" %%i in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
    set name=%%i
    set lang=!name:~5,5!
    echo Converting !lang! from .ts to .po
    lconvert lang/lang-!lang!.ts -o lang/lang-!lang!.po
  )

	echo.
)

rem convert .po files back to .ts files
echo Converting files po -^> ts
for /f "tokens=*" %%i in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
  set name=%%i
  set lang=!name:~5,5!
  echo Converting !lang! from .po to .ts >NUL
  lconvert -locations relative lang/lang-!lang!.po -o lang/lang-!lang!.ts >NUL
)

rem update .ts files
echo Updating ts files
lupdate -verbose creator.pro >NUL

rem convert .ts files to .po files
echo Converting files ts -^> po
for /f "tokens=*" %%i in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
  set name=%%i
  set lang=!name:~5,5!
  echo Converting !lang! from .ts to .po >NUL
  lconvert lang/lang-!lang!.ts -o lang/lang-!lang!.po >NUL
)

rem create .qm files out of .ts files
echo Creating qm files
lrelease creator.pro >NUL

echo ====================================================================
echo.
