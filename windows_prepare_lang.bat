@echo off

REM SPDX-License-Identifier: GPL-2.0-or-later
REM Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

Setlocal EnableDelayedExpansion

set PATH=c:\Qt\6.2.2\mingw_64\bin;%PATH%
set PATH=c:\Qt\Tools\mingw900_64\bin;%PATH%
set PATH=c:\Qt\Static\6.2.2\bin;%PATH%

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
  for /f "tokens=*" %%f in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
    set name=%%~nf
    set lang=!name:~5,99!
    echo Converting !lang! from .ts to .po
    lconvert lang/lang-!lang!.ts -o lang/lang-!lang!.po
  )

	echo.
)

rem convert .po files back to .ts files
echo Converting files po -^> ts
for /f "tokens=*" %%f in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
  set name=%%~nf
  set lang=!name:~5,99!
  echo Converting !lang! from .po to .ts >NUL
  lconvert -locations relative lang/lang-!lang!.po -o lang/lang-!lang!.ts >NUL
)

rem update .ts files
echo Updating ts files
lupdate -verbose creator.pro >NUL

rem convert .ts files to .po files
echo Converting files ts -^> po
for /f "tokens=*" %%f in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
  set name=%%~nf
  set lang=!name:~5,99!
  echo Converting !lang! from .ts to .po >NUL
  lconvert lang/lang-!lang!.ts -o lang/lang-!lang!.po >NUL
)

rem create .qm files out of .ts files
echo Creating qm files
lrelease creator.pro >NUL

echo ====================================================================
echo.
