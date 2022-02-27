// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

#include "zlib.h"

class DiskWriter : public QObject
{
    Q_OBJECT

public:
    DiskWriter(QObject *parent = 0) : QObject(parent) {}
    virtual ~DiskWriter() {}

private:
    virtual void writeUncompressedImage(const QString &filename, const QString& device);
    virtual void writeGzCompressedImage(const QString &filename, const QString& device);
    virtual void writeZipCompressedImage(const QString &filename, const QString& device);
    virtual int zipRead(gzFile src, z_streamp stream, QByteArray &buf, QByteArray &bufZip);

public slots:
    void cancelWrite();
    virtual void writeImageToRemovableDevice(const QString &filename, const QString& device);

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
