// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#include "diskwriter.h"

#ifdef WINDOWS_DUMMY_WRITE
  #include "windows.h"
#endif

#include <QCoreApplication>
#include <QDebug>

void DiskWriter::cancelWrite()
{
    isCancelled = true;
}

void DiskWriter::writeImageToRemovableDevice(const QString &filename, const QString &device)
{
    if (!open(device)) {
        emit error("Couldn't open " + device);
        return;
    }

    isCancelled = false;

    if (filename.endsWith(".gz"))
        writeGzCompressedImage(filename, device);
    else if (filename.endsWith(".zip"))
        writeZipCompressedImage(filename, device);
    else
        writeUncompressedImage(filename, device);

    if (isCancelled)
        emit bytesWritten(0);
    else
        emit finished();
}

void DiskWriter::writeGzCompressedImage(const QString &filename, const QString& device)
{
    int read;
    QByteArray buf(512*1024*sizeof(char), 0);

    // Open source
#if defined(_WIN32)
    // toStdString internally converts filename to utf8, which
    // windows does not support for fileaccess
    // so use unchanged 16 Bit unicode here
    gzFile src = gzopen_w((const wchar_t *)filename.utf16(), "rb");
#else
    gzFile src = gzopen(filename.toStdString().c_str(), "rb");
#endif
    if (src == NULL) {
        emit error("Couldn't open " + filename);
        this->close();
        return;
    }

    if (gzbuffer(src, buf.size()) != 0) {
        emit error("Failed to set gz buffer size");
        gzclose_r(src);
        this->close();
        return;
    }

    while (isCancelled == false) {
        read = gzread(src, buf.data(), buf.size());
        if (read == 0)
            break;  // all read
        else if (read < 0) {
            emit error("Failed to read from " + filename);
            gzclose_r(src);
            this->close();
            return;
        }

        // write data exactly as read from image and nothing more
        if (read < buf.size())
            buf.truncate(read);

        if (this->write(buf) == false) {
            emit error("Failed to write to " + device + "!");
            gzclose(src);
            this->close();
            return;
        }

        this->sync();
        emit bytesWritten(gztell(src));
    } // while

    emit syncing();
    gzclose_r(src);
    this->sync();
    this->close();
}

void DiskWriter::writeUncompressedImage(const QString &filename, const QString& device)
{
    // if input file is not in gzip format then
    // gzread reads directly from the file
    writeGzCompressedImage(filename, device);
}

// zip parts from zipcat.c -- inflate a single-file PKZIP archive to stdout
// by Sam Hocevar <sam@zoy.org>
void DiskWriter::writeZipCompressedImage(const QString &filename, const QString& device)
{
    int read;
    uint8_t buf4[4];
    QByteArray bufOut(512*1024*sizeof(char), 0);
    QByteArray bufZip(512*1024*sizeof(char), 0);
    unsigned int skipSize = 0;
    z_stream stream;
    off_t bytesRead = 0;

    // Open source
#if defined(_WIN32)
    // toStdString internally converts filename to utf8, which
    // windows does not support for fileaccess
    // so use unchanged 16 Bit unicode here
    gzFile src = gzopen_w((const wchar_t *)filename.utf16(), "rb");
#else
    gzFile src = gzopen(filename.toStdString().c_str(), "rb");
#endif
    if (src == NULL) {
        emit error("Couldn't open " + filename);
        this->close();
        return;
    }

    // Parse ZIP file (check header signature)
    read = gzread(src, buf4, 4);
    if (memcmp(buf4, "PK\3\4", 4) != 0) {
        emit error("Not ZIP file " + filename);
        gzclose_r(src);
        this->close();
        return ;
    }

    // https://en.wikipedia.org/wiki/Zip_(file_format)
    // go to start of first file
    gzseek(src, 22, SEEK_CUR);

    read = gzread(src, buf4, 2); // Uncompressed size
    if (read <= 0) {
        emit error("Failed to get filename size");
        gzclose_r(src);
        this->close();
        return ;
    }

    skipSize += (uint16_t) buf4[0] | ((uint16_t) buf4[1] << 8);

    read = gzread(src, buf4, 2); // Extra field size
    if (read <= 0) {
        emit error("Failed to get extra field size");
        gzclose_r(src);
        this->close();
        return ;
    }

    skipSize += (uint16_t) buf4[0] | ((uint16_t) buf4[1] << 8);

    gzseek(src, skipSize, SEEK_CUR);

    // Initialize inflate stream
    stream.total_out = 0;
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    stream.next_in = NULL;
    stream.avail_in = 0;

    if (inflateInit2(&stream, -MAX_WBITS) != Z_OK) {
        emit error("Failed to initialize decompression stream.");
        gzclose_r(src);
        this->close();
        return ;
    }

    while (isCancelled == false) {
        read = zipRead(src, &stream, bufOut, bufZip);
        if (read == 0)
            break;  // all read
        else if (read < 0) {
            emit error("Failed to read from " + filename);
            gzclose_r(src);
            this->close();
            return;
        }

        // write data exactly as read from image and nothing more
        if (read < bufOut.size())
            bufOut.truncate(read);

        if (this->write(bufOut) == false) {
            emit error("Failed to write to " + device + "!");
            gzclose(src);
            this->close();
            return;
        }

        this->sync();
        bytesRead += read;
        emit bytesWritten(bytesRead);
    } // while

    emit syncing();
    gzclose_r(src);
    this->sync();
    this->close();
}

int DiskWriter::zipRead(gzFile src, z_streamp stream, QByteArray &bufOut, QByteArray &bufZip)
{
    unsigned int total = 0;

    if (bufOut.size() == 0 || bufZip.size() == 0)
        return 0;

    stream->next_out = (Bytef *) bufOut.data();
    stream->avail_out = bufOut.size();

    while (stream->avail_out > 0) {
        unsigned int tmp;
        int ret = 0;

        if (stream->avail_in == 0 && !gzeof(src)) {
            int read = gzread(src, bufZip.data(), bufZip.size());
            if (read < 0)
                return -1;

            stream->next_in = (Bytef *) bufZip.data();
            stream->avail_in = read;
        }

        tmp = stream->total_out;
        ret = inflate(stream, Z_SYNC_FLUSH);
        total += stream->total_out - tmp;

        if (ret == Z_STREAM_END)
            return total;

        if (ret != Z_OK)
            return ret;
    }

    return total;
}
