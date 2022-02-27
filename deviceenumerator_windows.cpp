// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#include "deviceenumerator_windows.h"

#include <windows.h>

#include "diskwriter_windows.h"

// Adapted from http://skilinium.com/blog/?p=134
QStringList DeviceEnumerator_windows::getRemovableDeviceNames() const
{
    QStringList names;
#ifdef WINDOWS_DUMMY_WRITE
    names << "dummy_image_device";
    return names;
#endif

    WCHAR *szDriveLetters;
    WCHAR szDriveInformation[1024];

    GetLogicalDriveStrings(1024, szDriveInformation);

    szDriveLetters = szDriveInformation;
    while (*szDriveLetters != '\0') {
        if (GetDriveType(szDriveLetters) == DRIVE_REMOVABLE)
            names << QString::fromWCharArray(szDriveLetters);

        szDriveLetters = &szDriveLetters[wcslen(szDriveLetters) + 1];
    }

    return names;
}

QStringList DeviceEnumerator_windows::getUserFriendlyNames(const QStringList &devices) const
{
    QStringList names;
#ifdef WINDOWS_DUMMY_WRITE
    Q_UNUSED(devices);
    names << "dummy_image_device [ local file ]";
    return names;
#endif

    foreach (const QString &dev, devices) {
        qint64 size = getSizeOfDevice(dev);
        QString label = getLabelOfDevice(dev);
        if (label.isEmpty())
            names << dev + " [" + sizeToHuman(size) + "]";
        else
            names << dev + " [" + label + ", " + sizeToHuman(size) + "]";
    }

    return names;
}

qint64 DeviceEnumerator_windows::getSizeOfDevice(const QString &device) const
{
#ifdef WINDOWS_DUMMY_WRITE
    Q_UNUSED(device);
    return std::numeric_limits<qint64>::max();
#endif

    HANDLE handle = DiskWriter_windows::getHandleOnDevice(device, GENERIC_READ);
    if (handle == INVALID_HANDLE_VALUE)
        return 0;

    DWORD junk;
    qint64 size = 0;
    GET_LENGTH_INFORMATION lenInfo;
    bool ok = DeviceIoControl(handle,
                              IOCTL_DISK_GET_LENGTH_INFO,
                              NULL, 0,
                              &lenInfo, sizeof(lenInfo),
                              &junk,
                              (LPOVERLAPPED) NULL);
    if (ok)
        size = lenInfo.Length.QuadPart;

    CloseHandle(handle);
    return size;
}

QString DeviceEnumerator_windows::getLabelOfDevice(const QString &device)
{
    TCHAR label[MAX_PATH + 1] = { 0 };

    if (GetVolumeInformation((const wchar_t *) device.utf16(), \
          label, ARRAYSIZE(label), \
          NULL, NULL, NULL, NULL, 0))
    {
        return QString::fromUtf16((const char16_t *) label);
    }

    return "";
}

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

// http://www.codeproject.com/Articles/259577/How-to-flush-a-storage-volumes-the-file-cache-lock
// EjectMediaByLetter.cpp by Uwe Sieber - www.uwe-sieber.de
// Simple demonstration how to flush, lock and dismount a volume and eject a media from a drive
// Works under W2K, XP, W2K3, Vista, Win7, Win8, not tested under Win9x
// you are free to use this code in your projects
int DeviceEnumerator_windows::loadEjectDrive(const QString &device, const loadEject action) const
{
    bool ForceEject = false;  // dismount and ejecting even we got no lock

    if (action == LOADEJECT_LOAD)
        qDebug() << "Loading device" << device;
    else
        qDebug() << "Ejecting device" << device;

    if (device.at(0) < 'A' || device.at(0) > 'Z' || device.at(1) != ':')
        return ERRL_INVALID_PARAM;

    // "\\.\X:"  -> to open the volume
    QString device1 = "\\\\.\\" + device.left(2);
    LPCWSTR LPCWS_device = (const wchar_t *) device1.utf16();

    int res;
    DWORD dwRet;

    // try to flush, write access required which only admins will get
    HANDLE hVolWrite = CreateFile(LPCWS_device, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVolWrite != INVALID_HANDLE_VALUE) {
        qDebug() << "Flushing cache...";
        res = FlushFileBuffers(hVolWrite);
        if (!res)
            qDebug() << "failed err=" << GetLastError();

        CloseHandle(hVolWrite);
    }

    HANDLE hVolRead = CreateFile(LPCWS_device, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hVolRead == INVALID_HANDLE_VALUE) {
        qDebug() << "error opening the volume for read access -> abort\n" << GetLastError();
        return ERRL_NO_VOLREAD;
    }

    if (action == LOADEJECT_EJECT) {
        // allowing (unlocking) eject, usually for CD/DVD only, but does not hurt (and returns TRUE) for other drives
        qDebug() << "Allowing eject...";
        PREVENT_MEDIA_REMOVAL pmr = {0}; // pmr.PreventMediaRemoval = FALSE;
        res = DeviceIoControl(hVolRead, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &dwRet, NULL);
        if (!res)
            qDebug() << "failed err=" << GetLastError();
    }

    // try to lock the volume, seems to flush too, maybe even with read access...
    qDebug() << "Locking volume...";
    int Locked = DeviceIoControl(hVolRead, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwRet, NULL);
    if (!Locked)
        qDebug() << "failed err=" << GetLastError();

    if (!Locked && !ForceEject)
        return ERRL_NO_LOCK;

    if (action == LOADEJECT_EJECT) {
        // dismount the file system if either we got a lock or we want to force it
        qDebug() << "Dismounting volume...";
        res = DeviceIoControl(hVolRead, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwRet, NULL);
        if (!res)
            qDebug() << "failed err=" << GetLastError();
    }

    if (action == LOADEJECT_EJECT) {
        qDebug() << "Ejecting media...";
        res = DeviceIoControl(hVolRead, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &dwRet, NULL);
        if (!res)
            qDebug() << "failed err=" << GetLastError();
    } else {
        qDebug() << "Loading media...";
        res = DeviceIoControl(hVolRead, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &dwRet, NULL);
        if (!res)
            qDebug() << "failed err=" << GetLastError();
    }

    if (Locked)
        DeviceIoControl(hVolRead, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwRet, NULL);

    CloseHandle(hVolRead);

    if (res) {
        qDebug() << "success";
        return ERRL_SUCCESS;
    }

    qDebug() << "no eject";
    return ERRL_NO_EJECT;
}

int DeviceEnumerator_windows::removeDrive(const QString &device) const
{
    Q_UNUSED(device);
    qDebug() << "unimplemented: DeviceEnumerator_windows::removeDrive";
    return 0;
}
