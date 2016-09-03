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

chmod -R 755 dmg_osx

echo ""
echo "Creating .qm files"
/Users/$USER/Qt5.6.1/5.6/clang_64/bin/lrelease creator.pro

echo ""
echo "Running qmake..."
/Users/$USER/Qt5.6.1/5.6/clang_64/bin/qmake

echo ""
echo "Building..."
make -j4

# to decompile
#    osadecompile main.scpt >main.scpt.txt
echo ""
echo "Running osacompile..."
mkdir -p dmg_osx/template.app/Contents/Resources/Scripts
osacompile -t osas -o dmg_osx/template.app/Contents/Resources/Scripts/main.scpt dmg_osx/main.scpt.txt

echo ""
echo "Running macdeployqt..."
/Users/$USER/Qt5.6.1/5.6/clang_64/bin/macdeployqt "LibreELEC USB-SD Creator.app"

echo ""
echo "Copying template files over..."
cp -r dmg_osx/template.app/* "LibreELEC USB-SD Creator.app"

echo ""
echo "  To run application directly type"
echo "    sudo \"LibreELEC USB-SD Creator.app/Contents/MacOS/LibreELEC USB-SD Creator\""
echo ""
echo "  Or run osx_make_dmg.sh script to create final .dmg file..."
echo ""
echo "Finished..."
echo ""

exit 0
