#!/bin/sh

# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

echo ""
echo "  Start cleaning..."
rm -f LibreELEC.USB-SD.Creator.Linux-*bit.bin

if [ -f Makefile ]; then
  make distclean
  rmdir .generated_files
  rm -f creator.pro.user
  rm -f log
  rm -f lang/*.qm
fi

echo ""
echo "  Finished."
echo ""
