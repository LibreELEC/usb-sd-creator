#!/bin/bash

# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2019-present Team LibreELEC (https://libreelec.tv)

if [ -f ~/.transifex ]; then
  . ~/.transifex
else
  TRANSIFEX_USER="user:pass"
fi

cp lang/lang-en_GB.po lang/lang-en_GB.po_orig
rm lang/lang-*.po
rm lang/lang-*.qm
mv lang/lang-en_GB.po_orig lang/lang-en_GB.po

curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ast_ES/?file -o lang/lang-ast_ES.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/bg/?file     -o lang/lang-bg_BG.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/cs/?file     -o lang/lang-cs_CZ.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/da/?file     -o lang/lang-da_DK.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/de/?file     -o lang/lang-de_DE.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/el/?file     -o lang/lang-el_GR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/es/?file     -o lang/lang-es_ES.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fa/?file     -o lang/lang-fa_IR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fi/?file     -o lang/lang-fi_FI.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/fr/?file     -o lang/lang-fr_FR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/he/?file     -o lang/lang-he_IL.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hr/?file     -o lang/lang-hr_HR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/hu/?file     -o lang/lang-hu_HU.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/it/?file     -o lang/lang-it_IT.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ko/?file     -o lang/lang-ko_KR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/lt_LT/?file  -o lang/lang-lt_LT.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/nl/?file     -o lang/lang-nl_NL.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/no/?file     -o lang/lang-no_NO.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pl/?file     -o lang/lang-pl_PL.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt/?file     -o lang/lang-pt_PT.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/pt_BR/?file  -o lang/lang-pt_BR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ro/?file     -o lang/lang-ro_RO.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/ru/?file     -o lang/lang-ru_RU.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sk/?file     -o lang/lang-sk_SK.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sl/?file     -o lang/lang-sl_SI.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sr_RS/?file  -o lang/lang-sr_RS.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/sv/?file     -o lang/lang-sv_SE.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/tr/?file     -o lang/lang-tr_TR.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/uk/?file     -o lang/lang-uk_UA.po
curl -k -X GET -L --user $TRANSIFEX_USER https://www.transifex.com/api/2/project/libreelec-usb-sd-creator/resource/stringspo/translation/zh/?file     -o lang/lang-zh_CN.po

for file in lang/*.po; do
  sed -i "s/<.*@.*>/<email-removed>/" $file
done
