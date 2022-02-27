// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef DEVICEENUMERATOR_H
#define DEVICEENUMERATOR_H

#include <QStringList>

// interface class
class DeviceEnumerator
{
public:
    enum loadEject {
      LOADEJECT_LOAD = 0,
      LOADEJECT_EJECT
    };

    virtual ~DeviceEnumerator() {}
    virtual QStringList getRemovableDeviceNames() const = 0;
    virtual QStringList getUserFriendlyNames(const QStringList& devices) const = 0;
    virtual bool unmountDevicePartitions(const QString &device) const = 0;
    virtual qint64 getSizeOfDevice(const QString &device) const = 0;
    virtual int loadEjectDrive(const QString &device, const loadEject action) const = 0;
    virtual int removeDrive(const QString &device) const = 0;

    QString sizeToHuman(const qint64 size) const
    {
        const QStringList suffixes = QStringList() << " B" << " kB" << " MB" << " GB" << " TB";

        if (size <= 0)
            return "0 MB";

        int idx = 0;
        qreal fSize = size;
        while ((fSize > 1024.0) && ((idx + 1) < suffixes.size())) {
            ++idx;
            fSize /= 1024.0;
        }

        return QString::number(fSize, 'f', 1) + suffixes[idx];
    }
};

#endif // DEVICEENUMERATOR_H
