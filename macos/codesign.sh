#!/usr/bin/env bash

# working directory - .app/Contents

if [ -z "$CODE_SIGN_IDENTITY" ] ; then
  echo 'skip signing - no code sign identity provided in CODE_SIGN_IDENTITY'
  exit 0
fi

function sign {
  codesign --verbose=4 --force --timestamp --options=runtime --sign "$CODE_SIGN_IDENTITY" "$1"
}


echo 'sign frameworks'
for framework in Frameworks/*.framework ; do
  sign "$framework"
done

echo 'sign dylibs'
for lib in $(find PlugIns -type f -iname '*.dylib') ; do
  sign "$lib"
done

echo 'sign app bundle'
sign ..
