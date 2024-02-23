#!/usr/bin/env bash

brew install ninja

cd ..
for module in base tools ; do
  archive="$module.7z"
  curl -L -o "$archive" "https://download.qt.io/online/qtsdkrepository/mac_x64/desktop/qt6_662/qt.qt6.662.clang_64/6.6.2-0-202402121131qt$module-MacOS-MacOS_13-Clang-MacOS-MacOS_13-X86_64-ARM64.7z"
  7z x "$archive" '-xr!*.dSYM'
done
echo "CMAKE_PREFIX_PATH=$PWD/$(ls -1 | fgrep 6.)/macos" >> $GITHUB_ENV

if [ "$MACOS_ASC_API_KEY" ]; then
  ascApiKey='ascApiKey.p8'
  echo "$MACOS_ASC_API_KEY" > "$ascApiKey"
  echo "ASC_API_KEY_PATH=$PWD/$ascApiKey" >> $GITHUB_ENV
fi

if [ "$MACOS_CODE_SIGN_KEY_BASE64" ]; then
  codesignKey='codesign.p12'
  echo "$MACOS_CODE_SIGN_KEY_BASE64" | base64 --decode > "$codesignKey"
  security import "$codesignKey" -f pkcs12 -P "" -T "$(which codesign)"
  echo "CODE_SIGN_IDENTITY=Developer ID Application: Kodi Foundation" >> $GITHUB_ENV
fi
