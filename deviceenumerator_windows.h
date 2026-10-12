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

#ifndef DEVICEENUMERATOR_WINDOWS_H
#define DEVICEENUMERATOR_WINDOWS_H

#include "deviceenumerator.h"

#include <QStringList>
#include <QDebug>

class DeviceEnumerator_windows : public DeviceEnumerator
{
public:
    QStringList getRemovableDeviceNames() const override;
    QStringList getUserFriendlyNames(const QStringList& devices) const override;
    bool unmountDevicePartitions(const QString &device) const override {
        Q_UNUSED(device);
        return true;
    }
    qint64 getSizeOfDevice(const QString &device) const override;
    int loadEjectDrive(const QString &device, const loadEject action) const override;
    int removeDrive(const QString &device) const override;

private:
    static QString getLabelOfDevice(const QString &device);

    // returned errorlevels
    enum {
      ERRL_SUCCESS = 0,
      ERRL_INVALID_PARAM,
      ERRL_NO_VOLREAD,
      ERRL_NO_LOCK,
      ERRL_NO_EJECT
    };
};

#endif // DEVICEENUMERATOR_WINDOWS_H
