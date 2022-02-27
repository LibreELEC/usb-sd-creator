#!/bin/sh

# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set -e

echo ""
echo "Start building..."

USER=$(whoami)
QBIN="/Users/$USER/Qt/6.2.2-static/bin"

chmod -R 755 dmg_osx

echo ""
echo "Creating .qm files"
$QBIN/lrelease creator.pro

echo ""
echo "Running qmake..."
$QBIN/qmake

echo ""
echo "Building..."
make -j$(sysctl -n hw.ncpu)

# to decompile
#    osadecompile main.scpt >main.scpt.txt
echo ""
echo "Running osacompile..."
mkdir -p dmg_osx/template.app/Contents/Resources/Scripts
osacompile -t osas -o dmg_osx/template.app/Contents/Resources/Scripts/main.scpt dmg_osx/main.scpt.txt

echo ""
echo "Running macdeployqt..."
$QBIN/macdeployqt "LibreELEC USB-SD Creator.app"

echo ""
echo "Copying template files over..."
cp -r dmg_osx/template.app/* "LibreELEC USB-SD Creator.app"

echo ""
echo "Signing..."
xattr -cr "LibreELEC USB-SD Creator.app"
codesign --deep -v -fs "LibreELEC" "LibreELEC USB-SD Creator.app"
codesign -vvvv "LibreELEC USB-SD Creator.app"

echo ""
echo "Finished..."
echo ""

exit 0
