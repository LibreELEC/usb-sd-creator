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

#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

#include <zlib.h>

class DiskWriter : public QObject
{
    Q_OBJECT

public:
    DiskWriter(QObject *parent = 0) : QObject(parent) {}
    virtual ~DiskWriter() {}

private:
    virtual void writeUncompressedImage(const QString &filename, const QString& device, const QString& deviceText);
    virtual void writeGzCompressedImage(const QString &filename, const QString& device, const QString& deviceText);
    virtual void writeZipCompressedImage(const QString &filename, const QString& device, const QString& deviceText);
    virtual int zipRead(gzFile src, z_streamp stream, QByteArray &buf, QByteArray &bufZip);

public slots:
    void cancelWrite();
    virtual void writeImageToRemovableDevice(const QString &filename, const QString& device, const QString& deviceText);

signals:
    void bytesWritten(int);
    void syncing();
    void finished();
    void error(const QString& message);

protected:
    volatile bool isCancelled;

    virtual bool open(const QString& device) = 0;
    virtual void close() = 0;
    virtual void sync() = 0;
    virtual bool isOpen() = 0;
    virtual bool write(const QByteArray &data) = 0;
};

#endif // DISKWRITER_H
