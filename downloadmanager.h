// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    ~DownloadManager();

    QNetworkReply* get(const QUrl& url);

signals:
    void downloadComplete(const QByteArray&);
    void partialData(const QByteArray, qlonglong total);
    void downloadProgress(qint64, qint64);
    void downloadError(const QString);

public slots:
    void cancelDownload();

private slots:
    void handleProgress(qint64,qint64);
    void handleGetFinished(QNetworkReply*);
    void handleReadyRead();

private:
    QNetworkAccessManager* manager;
    QNetworkReply* latestReply;

    enum {
        RESPONSE_OK = 200,
        RESPONSE_PARTIAL = 206,
        RESPONSE_FOUND = 302,
        RESPONSE_REDIRECT = 307,
        RESPONSE_BAD_REQUEST = 400,
        RESPONSE_NOT_FOUND = 404
    };
};

#endif // DOWNLOADMANAGER_H
