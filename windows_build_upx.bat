@echo off

REM # SPDX-License-Identifier: GPL-2.0-or-later
REM # Copyright (C) 2022-present Team LibreELEC (https://libreelec.tv)

set UPX_VERSION=3.96

if exist tools\upx-%UPX_VERSION%-win64\upx.exe (
  echo Found UPX version %UPX_VERSION%
) else (
  mkdir tools\

  echo Downloading UPX version %UPX_VERSION%
  curl -sS -L https://github.com/upx/upx/releases/download/v%UPX_VERSION%/upx-%UPX_VERSION%-win64.zip -o tools\upx-%UPX_VERSION%-win64.zip

  echo Extracting upx-%UPX_VERSION%-win64.zip
  powershell -Command "& {Expand-Archive tools\upx-%UPX_VERSION%-win64.zip -DestinationPath tools\}"
)

echo Compressing the binary
tools\upx-%UPX_VERSION%-win64\upx.exe -9 -k -f --lzma "release\LibreELEC.USB-SD.Creator.Win32.exe"

echo build finished
