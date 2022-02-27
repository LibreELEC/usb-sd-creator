@echo off

REM SPDX-License-Identifier: GPL-2.0-or-later
REM Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

set PATH=c:\Qt\Static\6.2.2\bin;%PATH%
set PATH=c:\Qt\Tools\mingw900_64\bin;%PATH%
set PATH=c:\Qt\Tools\QtCreator\bin;%PATH%

start qtcreator.exe creator.pro
