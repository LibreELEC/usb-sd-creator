#!/bin/sh
################################################################################
#      This file is part of LibreELEC - http://www.libreelec.tv
#      Copyright (C) 2016 Team LibreELEC
#
#  LibreELEC is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  LibreELEC is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with LibreELEC.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

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
