////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
//      Copyright (C) 2016 Team LibreELEC
//
//  LibreELEC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 2 of the License, or
//  (at your option) any later version.
//
//  LibreELEC is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with LibreELEC.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "diskwriter.h"
#include <QFile>
#include <QtDBus>

class DiskWriter_udisks2 : public DiskWriter
{
    Q_OBJECT
public:
    explicit DiskWriter_udisks2(QObject *parent = 0);
    ~DiskWriter_udisks2();

private:
    QDBusUnixFileDescriptor fd;
    QDBusConnection connection;

    bool open(const QString &device) override;
    void close() override;
    void sync() override;
    bool isOpen() override;
    bool write(const QByteArray &data) override;
};
