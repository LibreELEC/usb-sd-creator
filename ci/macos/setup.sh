#!/usr/bin/env bash
set -e

brew install ninja p7zip

qtVersion='6.11.1'

if command -v pipx >/dev/null 2>&1; then
  pipx install aqtinstall
  AQT="aqt"
elif command -v python3 >/dev/null 2>&1; then
  python3 -m pip install --break-system-packages aqtinstall 2>/dev/null || python3 -m pip install aqtinstall
  AQT="python3 -m aqt"
else
  pip install aqtinstall
  AQT="aqt"
fi

cd ..
$AQT install-qt mac desktop "$qtVersion" clang_64 --outputdir "$PWD" --archives qtbase qtdeclarative qttools

qtDir=$(find "$PWD/$qtVersion" -maxdepth 2 -type d \( -name "macos" -o -name "clang_64" \) 2>/dev/null | head -n 1)
if [ -z "$qtDir" ]; then
  qtDir="$PWD/$qtVersion/macos"
fi

echo "CMAKE_PREFIX_PATH=$qtDir" >> "$GITHUB_ENV"
if [ -d "$qtDir/bin" ]; then
  echo "$qtDir/bin" >> "$GITHUB_PATH"
fi

if [ "$MACOS_ASC_API_KEY" ]; then
  ascApiKey='ascApiKey.p8'
  echo "$MACOS_ASC_API_KEY" > "$ascApiKey"
  echo "ASC_API_KEY_PATH=$PWD/$ascApiKey" >> $GITHUB_ENV
fi

if [[ "$MACOS_CODE_SIGN_KEY_BASE64" && "$MACOS_KEYCHAIN_PASSWORD" ]]; then
  codesignKey='codesign.p12'
  echo "$MACOS_CODE_SIGN_KEY_BASE64" | base64 --decode > "$codesignKey"
  echo "CODE_SIGN_IDENTITY=Developer ID Application: Kodi Foundation" >> $GITHUB_ENV

  keychainPath='build.keychain'
  security create-keychain -p "$MACOS_KEYCHAIN_PASSWORD" "$keychainPath"
  security unlock-keychain -p "$MACOS_KEYCHAIN_PASSWORD" "$keychainPath"
  security default-keychain -s "$keychainPath"
  security import "$codesignKey" -f pkcs12 -P "" -T "$(which codesign)" "$keychainPath"
  security set-key-partition-list -S apple-tool:,apple: -s -k "$MACOS_KEYCHAIN_PASSWORD" "$keychainPath"
fi
