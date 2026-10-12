////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
//      Copyright (C) 2013-2015 RasPlex project
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

#ifndef DEVICEENUMERATOR_UNIX_H
#define DEVICEENUMERATOR_UNIX_H

#include "deviceenumerator.h"
#include <QStringList>

class DeviceEnumerator_unix : public DeviceEnumerator
{
public:
    QStringList getRemovableDeviceNames() const override;
    QStringList getUserFriendlyNames(const QStringList& devices) const override;
    bool unmountDevicePartitions(const QString &device) const override;
    qint64 getSizeOfDevice(const QString &device) const override;
    int loadEjectDrive(const QString &device, const loadEject action) const override
    {
        Q_UNUSED(device);
        Q_UNUSED(action);
        return 0;
    }
    int removeDrive(const QString &device) const override
    {
        Q_UNUSED(device);
        return 0;
    }

private:
    bool checkIsMounted(const QString& device) const;
    bool checkIfUSB(const QString& device) const;
    QStringList getDeviceNamesFromSysfs() const;
#if defined(Q_OS_LINUX)
    QStringList getPartitionsInfo(const QString &device) const;
    QString getFirstPartitionLabel(const QString& device) const;
    bool unmount(const QString& what) const;
#endif
};

#endif // DEVICEENUMERATOR_UNIX_H
