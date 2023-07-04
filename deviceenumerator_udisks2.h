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

#include "deviceenumerator.h"

#include <QStringList>
#include <QtDBus>

class DeviceEnumerator_udisks2 : public DeviceEnumerator
{
public:
    DeviceEnumerator_udisks2();
    ~DeviceEnumerator_udisks2() = default;

    QStringList getRemovableDeviceNames() const override;
    QStringList getUserFriendlyNames(const QStringList& devices) const override;
    bool unmountDevicePartitions(const QString &device) const override;
    qint64 getSizeOfDevice(const QString &device) const override;
    int loadEjectDrive(const QString &device, const loadEject action) const override;
    int removeDrive(const QString &device) const override;

private:
    QDBusConnection connection;
};
