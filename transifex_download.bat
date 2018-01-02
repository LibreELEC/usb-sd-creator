@echo off
rem ################################################################################
rem #      This file is part of LibreELEC - https://libreelec.tv
rem #      Copyright (C) 2016-present Team LibreELEC
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

set TRANSIFEX_USER="username:password"

set PATH=c:\Qt\Qt5.6.1-static\bin;%PATH%
set PATH=c:\Qt\Qt5.6.1\Tools\mingw492_32\bin;%PATH%

rmdir /s /q lang_dl 2>NUL
mkdir lang_dl 2>NUL

copy lang\lang-en_GB.po lang\lang-en_GB.po_orig

del lang\lang-*.po 2>NUL
del lang\lang-*.qm 2>NUL

move lang\lang-en_GB.po_orig lang\lang-en_GB.po

c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ast_ES/?file -o lang_dl\lang-ast_ES.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/bg/?file     -o lang_dl\lang-bg_BG.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/zh/?file     -o lang_dl\lang-zh_CN.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hr/?file     -o lang_dl\lang-hr_HR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/cs/?file     -o lang_dl\lang-cs_CZ.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/da/?file     -o lang_dl\lang-da_DK.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/nl/?file     -o lang_dl\lang-nl_NL.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fi/?file     -o lang_dl\lang-fi_FI.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fr/?file     -o lang_dl\lang-fr_FR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/de/?file     -o lang_dl\lang-de_DE.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/el/?file     -o lang_dl\lang-el_GR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hu/?file     -o lang_dl\lang-hu_HU.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/it/?file     -o lang_dl\lang-it_IT.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/lt_LT/?file  -o lang_dl\lang-lt_LT.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/no/?file     -o lang_dl\lang-no_NO.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fa/?file     -o lang_dl\lang-fa_IR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pl/?file     -o lang_dl\lang-pl_PL.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt/?file     -o lang_dl\lang-pt_PT.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt_BR/?file  -o lang_dl\lang-pt_BR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ru/?file     -o lang_dl\lang-ru_RU.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sk/?file     -o lang_dl\lang-sk_SK.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sl/?file     -o lang_dl\lang-sl_SI.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/es/?file     -o lang_dl\lang-es_ES.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sv/?file     -o lang_dl\lang-sv_SE.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/tr/?file     -o lang_dl\lang-tr_TR.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/uk/?file     -o lang_dl\lang-uk_UA.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sr_RS/?file  -o lang_dl\lang-sr_RS.po
c:\programi\curl -k -X GET -L --user %TRANSIFEX_USER% https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/he/?file     -o lang_dl\lang-he_IL.po

sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-ast_ES.po >lang\lang-ast_ES.po
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-bg_BG.po  >lang\lang-bg_BG.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-zh_CN.po  >lang\lang-zh_CN.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-hr_HR.po  >lang\lang-hr_HR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-cs_CZ.po  >lang\lang-cs_CZ.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-da_DK.po  >lang\lang-da_DK.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-nl_NL.po  >lang\lang-nl_NL.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-fi_FI.po  >lang\lang-fi_FI.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-fr_FR.po  >lang\lang-fr_FR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-de_DE.po  >lang\lang-de_DE.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-el_GR.po  >lang\lang-el_GR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-hu_HU.po  >lang\lang-hu_HU.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-it_IT.po  >lang\lang-it_IT.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-lt_LT.po  >lang\lang-lt_LT.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-no_NO.po  >lang\lang-no_NO.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-fa_IR.po  >lang\lang-fa_IR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-pl_PL.po  >lang\lang-pl_PL.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-pt_PT.po  >lang\lang-pt_PT.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-pt_BR.po  >lang\lang-pt_BR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-ru_RU.po  >lang\lang-ru_RU.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-sk_SK.po  >lang\lang-sk_SK.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-sl_SI.po  >lang\lang-sl_SI.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-es_ES.po  >lang\lang-es_ES.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-sv_SE.po  >lang\lang-sv_SE.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-tr_TR.po  >lang\lang-tr_TR.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-uk_UA.po  >lang\lang-uk_UA.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-sr_RS.po  >lang\lang-sr_RS.po 
sed "s/<.*@.*>/<email-removed>/" lang_dl\lang-he_IL.po  >lang\lang-he_IL.po 

copy lang\lang-*.po lang_dl

dir lang\lang-*.po

echo Should be 29 files: 28 translations + en_GB

echo.
echo Checking email address in .po files...
echo ------------------------------------------------------
findstr.exe /s @ lang_dl/*
echo ------------------------------------------------------
