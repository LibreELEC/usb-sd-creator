#!/bin/sh

# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

echo ""
echo "Start cleaning..."
echo ""

USER=$(whoami)

if [ -f Makefile ]; then
  make distclean
  rmdir .generated_files
  rm -f creator.pro.user
fi

rm -f lang/*.qm
rm -f log
rm -f *.dmg
rm -f local_image_file

echo ""
echo "Finished."
echo ""
