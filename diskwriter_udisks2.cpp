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

#include "diskwriter_udisks2.h"

namespace
{
constexpr auto UDISKS2_SERVICE{"org.freedesktop.UDisks2"};
constexpr auto UDISKS2_BLOCK_INTERFACE{"org.freedesktop.UDisks2.Block"};
}

DiskWriter_udisks2::DiskWriter_udisks2(QObject *parent) :
    DiskWriter(parent),
    connection(QDBusConnection::systemBus())
{
    isCancelled = false;
}

DiskWriter_udisks2::~DiskWriter_udisks2()
{
    if (isOpen())
        close();
}

bool DiskWriter_udisks2::open(const QString& device)
{
    QDBusInterface blockInterface{UDISKS2_SERVICE, device, UDISKS2_BLOCK_INTERFACE, connection};

    if (!blockInterface.isValid())
        return false;

    QMap<QString, QVariant> options{};
    QDBusReply<QDBusUnixFileDescriptor> reply = blockInterface.call("OpenDevice", "w", options);

    if (!reply.isValid())
    {
        return false;
    }

    fd = reply.value();

    return fd.isValid();
}

void DiskWriter_udisks2::close()
{
    ::close(fd.fileDescriptor());
}

void DiskWriter_udisks2::sync()
{
    fsync(fd.fileDescriptor());
}

bool DiskWriter_udisks2::isOpen()
{
    return fd.isValid();
}

bool DiskWriter_udisks2::write(const QByteArray &data)
{
    return ::write(fd.fileDescriptor(), data.data(), data.size()) == data.size();
}

