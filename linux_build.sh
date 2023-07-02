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
echo "Building 64 bit version..."
QT_DIR=~/Qt6.2.2/6.2-static-64

export PATH=$QT_DIR/bin:$QT_DIR/../6.2.2/gcc_64/bin:$PATH

echo ""
echo "Creating .qm files"
lrelease creator.pro

echo ""
echo "Running qmake..."
qmake

echo ""
echo "Building..."
make -j4

echo ""
echo "  To run application directly type"
echo "    sudo ./LibreELEC.USB-SD.Creator.Linux.bin"
echo ""
echo "Finished..."
echo ""

exit 0
