@echo off

Setlocal EnableDelayedExpansion

set PATH=c:\Qt\Qt5.6.1\5.6\mingw49_32\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1\Tools\mingw492_32\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1-static\bin;%PATH%

rem ------------------------------
rem Creating a Newline variable
rem (the two blank lines are required!)
set LF=^


rem ------------------------------
set var1=
set var2=
set var3=

set cnt1=1
for /f "tokens=*" %%f in ('dir /b "lang\*.png" ^| %SYSTEMROOT%\system32\find.exe /v "empty"') do (
  set name=%%~nf
  set lang=!name:~5,99!
  rem echo Converting !lang!

  if !cnt1! EQU 1 (
    set var1=TRANSLATIONS += lang/lang-!lang!.ts \!LF!
  ) else (
    set var1=!var1!                lang/lang-!lang!.ts \!LF!
    set var2=!var2!!LF!
    set var3=!var3!!LF!
  )

  set var2=!var2!         ^<file^>lang/flag-!lang!.png^</file^>
  set var3=!var3!         ^<file^>lang/lang-!lang!.qm^</file^>

  set /a cnt1+=1
)

echo.
echo for file creator.pro!LF!
echo !var1!
echo for file resources.qrc!LF!
echo !var2!
echo !var3!
echo.
