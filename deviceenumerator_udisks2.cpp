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

#include "deviceenumerator_udisks2.h"

namespace
{
constexpr auto UDISKS2_SERVICE{"org.freedesktop.UDisks2"};
constexpr auto UDISKS2_MANAGER_PATH{"/org/freedesktop/UDisks2/Manager"};
constexpr auto UDISKS2_MANAGER_INTERFACE{"org.freedesktop.UDisks2.Manager"};

constexpr auto UDISKS2_BLOCK_INTERFACE{"org.freedesktop.UDisks2.Block"};
constexpr auto UDISKS2_DRIVE_INTERFACE{"org.freedesktop.UDisks2.Drive"};
constexpr auto UDISKS2_PARTITIONTABLE_INTERFACE{"org.freedesktop.UDisks2.PartitionTable"};
constexpr auto UDISKS2_FILESYSTEM_INTERFACE{"org.freedesktop.UDisks2.Filesystem"};

constexpr auto DBUS_PROPERTIES_INTERFACE{"org.freedesktop.DBus.Properties"};
}

DeviceEnumerator_udisks2::DeviceEnumerator_udisks2() :
    connection(QDBusConnection::systemBus())
{}

QStringList DeviceEnumerator_udisks2::getRemovableDeviceNames() const
{
    QDBusInterface managerInterface{UDISKS2_SERVICE, UDISKS2_MANAGER_PATH, UDISKS2_MANAGER_INTERFACE, connection};

    if (!managerInterface.isValid())
        return {};

    QMap<QString, QVariant> options{};

    QDBusReply<QList<QDBusObjectPath>> reply = managerInterface.call("GetBlockDevices", options);

    if (!reply.isValid())
        return {};

    QStringList deviceNames;

    for (const auto& blockDevice : reply.value())
    {
        QDBusInterface blockDevicePropertiesInterface{UDISKS2_SERVICE, blockDevice.path(), DBUS_PROPERTIES_INTERFACE, connection};

        if (!blockDevicePropertiesInterface.isValid())
            continue;

        QDBusReply<QVariant> replyType = blockDevicePropertiesInterface.call("Get", UDISKS2_PARTITIONTABLE_INTERFACE, "Type");

        if (!replyType.isValid())
            continue;

        QDBusReply<QVariant> replyDrive = blockDevicePropertiesInterface.call("Get", UDISKS2_BLOCK_INTERFACE, "Drive");

        if (!replyDrive.isValid())
            continue;

        QDBusInterface drivePropertiesInterface{UDISKS2_SERVICE, replyDrive.value().value<QDBusObjectPath>().path(), DBUS_PROPERTIES_INTERFACE, connection};

        if (!drivePropertiesInterface.isValid())
            continue;

        QDBusReply<QVariant> replyRemovable = drivePropertiesInterface.call("Get", UDISKS2_DRIVE_INTERFACE, "Removable");

        if (!replyRemovable.isValid())
            continue;

        if (replyRemovable.value().toBool())
            deviceNames.append(blockDevice.path());
    }

    return deviceNames;
}

QStringList DeviceEnumerator_udisks2::getUserFriendlyNames(const QStringList &devices) const
{
    QStringList friendlyNames;

    for (const auto& device : devices)
    {
        QDBusInterface blockPropertiesInterface{UDISKS2_SERVICE, device, DBUS_PROPERTIES_INTERFACE, connection};

        if (!blockPropertiesInterface.isValid())
            continue;

        QDBusReply<QVariant> replyDrive = blockPropertiesInterface.call("Get", UDISKS2_BLOCK_INTERFACE, "Drive");

        if (!replyDrive.isValid())
            continue;

        QDBusInterface drivePropertiesInterface{UDISKS2_SERVICE, replyDrive.value().value<QDBusObjectPath>().path(), DBUS_PROPERTIES_INTERFACE, connection};

        if (!drivePropertiesInterface.isValid())
            continue;

        QDBusReply<QVariant> replyVendor = drivePropertiesInterface.call("Get", UDISKS2_DRIVE_INTERFACE, "Vendor");

        if (!replyVendor.isValid())
            continue;

        QDBusReply<QVariant> replyModel = drivePropertiesInterface.call("Get", UDISKS2_DRIVE_INTERFACE,"Model");

        if (!replyModel.isValid())
            continue;

        qint64 size = getSizeOfDevice(device);

        QString name = QString("%1 %2 [%3]").arg(replyVendor.value().toString(), replyModel.value().toString(), sizeToHuman(size)).trimmed();
        friendlyNames.append(name);
    }

    return friendlyNames;
}

bool DeviceEnumerator_udisks2::unmountDevicePartitions(const QString &device) const
{
    QDBusInterface blockPropertiesInterface{UDISKS2_SERVICE, device, DBUS_PROPERTIES_INTERFACE, connection};

    if (!blockPropertiesInterface.isValid())
        return false;

    QDBusReply<QVariant> replyPartitions = blockPropertiesInterface.call("Get", UDISKS2_PARTITIONTABLE_INTERFACE, "Partitions");

    if (!replyPartitions.isValid())
        return false;

    QDBusArgument dbusArgs = replyPartitions.value().value<QDBusArgument>();
    QList<QDBusObjectPath> partitions;
    dbusArgs >> partitions;

    QMap<QDBusObjectPath, bool> partitionsUnmounted;

    for (const auto& partition : partitions)
    {
        partitionsUnmounted[partition] = false;

        QDBusInterface partitionPropertiesInterface{UDISKS2_SERVICE, partition.path(), DBUS_PROPERTIES_INTERFACE, connection};

        if (!partitionPropertiesInterface.isValid())
            continue;

        QDBusReply<QVariant> replyMountPoints = partitionPropertiesInterface.call("Get", UDISKS2_FILESYSTEM_INTERFACE, "MountPoints");

        if (!replyMountPoints.isValid())
            continue;

        QDBusArgument dbusArgs = replyMountPoints.value().value<QDBusArgument>();
        QByteArrayList mountPoints;
        dbusArgs >> mountPoints;

        if (mountPoints.empty())
        {
            partitionsUnmounted[partition] = true;
            continue;
        }
        else
        {
            QDBusInterface filesystemInterface{UDISKS2_SERVICE, partition.path(), UDISKS2_FILESYSTEM_INTERFACE, connection};

            if (!filesystemInterface.isValid())
                continue;

            QMap<QString, QVariant> options{};

            QDBusReply<void> reply = filesystemInterface.call("Unmount", options);

            partitionsUnmounted[partition] = reply.isValid();
        }
    }

    return std::all_of(partitionsUnmounted.keyValueBegin(), partitionsUnmounted.keyValueEnd(), [](const auto& it) {
        return it.second;
    });
}

qint64 DeviceEnumerator_udisks2::getSizeOfDevice(const QString& device) const
{
    QDBusInterface blockPropertiesInterface{UDISKS2_SERVICE, device, DBUS_PROPERTIES_INTERFACE, connection};

    if (!blockPropertiesInterface.isValid())
        return 0;

    QDBusReply<QVariant> replySize = blockPropertiesInterface.call("Get", UDISKS2_BLOCK_INTERFACE, "Size");

    if (!replySize.isValid())
        return 0;

    return replySize.value().toULongLong();
}

int DeviceEnumerator_udisks2::loadEjectDrive(const QString &device, const loadEject action) const
{
    return 0;
}

int DeviceEnumerator_udisks2::removeDrive(const QString &device) const
{
    return 0;
}
