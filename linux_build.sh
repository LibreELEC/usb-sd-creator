#!/bin/sh

# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set -e

echo ""
if [ "$1" = "32" ]; then
  echo "Building 32 bit version..."
  QT_DIR=~/Qt6.2.2/6.2-static-32
else
  echo "Building 64 bit version..."
  QT_DIR=~/Qt6.2.2/6.2-static-64
fi

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
if [ "$1" = "32" ]; then
  mv LibreELEC.USB-SD.Creator.Linux-bit.bin LibreELEC.USB-SD.Creator.Linux-32bit.bin
  echo "    sudo ./LibreELEC.USB-SD.Creator.Linux-32bit.bin"
else
  mv LibreELEC.USB-SD.Creator.Linux-bit.bin LibreELEC.USB-SD.Creator.Linux-64bit.bin
  echo "    sudo ./LibreELEC.USB-SD.Creator.Linux-64bit.bin"
fi
echo ""
echo "Finished..."
echo ""

exit 0
