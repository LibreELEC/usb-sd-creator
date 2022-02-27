@echo off

REM # SPDX-License-Identifier: GPL-2.0-or-later
REM # Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set /p USER=Enter Transifex username:
set /p PW=Enter Transifex password:
set TRANSIFEX_USER="%USER%:%PW%"
echo %TRANSIFEX_USER%

REM check if login works
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/en >NUL 2>NUL
if errorlevel 1 (
  echo Can't login at Transifex
  exit /b
)
pause

rmdir /s /q lang_dl 2>NUL
mkdir lang_dl 2>NUL

copy lang\lang-en_GB.po lang\lang-en_GB.po_orig

del lang\lang-*.po 2>NUL
del lang\lang-*.qm 2>NUL

move lang\lang-en_GB.po_orig lang\lang-en_GB.po

echo downloading Transifex files
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ast_ES/?file -o lang_dl\lang-ast_ES.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/bg/?file     -o lang_dl\lang-bg_BG.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/cs/?file     -o lang_dl\lang-cs_CZ.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/da/?file     -o lang_dl\lang-da_DK.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/de/?file     -o lang_dl\lang-de_DE.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/el/?file     -o lang_dl\lang-el_GR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/es/?file     -o lang_dl\lang-es_ES.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fa/?file     -o lang_dl\lang-fa_IR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fi/?file     -o lang_dl\lang-fi_FI.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fr/?file     -o lang_dl\lang-fr_FR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/he/?file     -o lang_dl\lang-he_IL.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hr/?file     -o lang_dl\lang-hr_HR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hu/?file     -o lang_dl\lang-hu_HU.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/it/?file     -o lang_dl\lang-it_IT.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/lt_LT/?file  -o lang_dl\lang-lt_LT.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/nl/?file     -o lang_dl\lang-nl_NL.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/no/?file     -o lang_dl\lang-no_NO.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pl/?file     -o lang_dl\lang-pl_PL.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt/?file     -o lang_dl\lang-pt_PT.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt_BR/?file  -o lang_dl\lang-pt_BR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ro/?file     -o lang_dl\lang-ro_RO.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ru/?file     -o lang_dl\lang-ru_RU.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sk/?file     -o lang_dl\lang-sk_SK.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sl/?file     -o lang_dl\lang-sl_SI.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sr_RS/?file  -o lang_dl\lang-sr_RS.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sv/?file     -o lang_dl\lang-sv_SE.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/tr/?file     -o lang_dl\lang-tr_TR.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/uk/?file     -o lang_dl\lang-uk_UA.po
curl -sS -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/zh/?file     -o lang_dl\lang-zh_CN.po

echo remove all email addresses from langfiles
for /f "delims=|" %%f in ('dir /b lang_dl') do powershell -Command "& {(Get-Content lang_dl\%%f) -replace '<.*@.*>', '<email-removed>' | Set-Content lang_dl\%%f}"
 
xcopy /y/q lang_dl\lang-*.po lang\

echo.
echo Checking email address in .po files...
echo ------------------------------------------------------
findstr /s @ lang_dl/*
