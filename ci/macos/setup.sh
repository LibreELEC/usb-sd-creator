#!/usr/bin/env bash

brew install ninja

cd ..
for module in base tools ; do
  archive="$module.7z"
  curl -L -o "$archive" "https://download.qt.io/online/qtsdkrepository/mac_x64/desktop/qt6_662/qt.qt6.662.clang_64/6.6.2-0-202402121131qt$module-MacOS-MacOS_13-Clang-MacOS-MacOS_13-X86_64-ARM64.7z"
  7z x "$archive" '-xr!*.dSYM'
done
echo "CMAKE_PREFIX_PATH=$PWD/$(ls -1 | fgrep 6.)/macos" >> $GITHUB_ENV
