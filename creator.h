// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef CREATOR_H
#define CREATOR_H

#include <QCryptographicHash>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QFile>
#include <QStandardPaths>
#include <QElapsedTimer>

#include "downloadmanager.h"
#include "jsonparser.h"
#include "movingaverage.h"
#include "translator.h"

#ifdef Q_OS_UNIX
#include "privileges_unix.h"
#else
#include "privileges.h"
#endif

// useful macro
#define isFilled()  isEmpty() == false

class QThread;
class DiskWriter;
class DeviceEnumerator;

namespace Ui {
class Creator;
}

class Creator : public QDialog
{
    Q_OBJECT

public:
    explicit Creator(Privileges &priv, QWidget *parent = 0);
    ~Creator();
    void setArgFile(QString argFile);
    void retranslateUi();
    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    Ui::Creator *ui;
    DownloadManager* manager;
    Translator *translator;

    void parseAndSetLinks(const QByteArray &data);
    void saveAndUpdateProgress(QNetworkReply *reply);
    void disableControls(const int which);
    bool isChecksumValid(const QString);

    QByteArray rangeByteArray(qlonglong first, qlonglong last);
    QNetworkRequest createRequest(QUrl &url, qlonglong first, qlonglong last);
    unsigned int getUncompressedImageSize();
    void setImageFileName(QString filename);
    QString getDefaultSaveDir();
    bool showRootMessageBox();

    enum {
        DISABLE_CONTROL_DOWNLOAD,
        DISABLE_CONTROL_WRITE
    };
    enum {
        RESPONSE_OK = 200,
        RESPONSE_PARTIAL = 206,
        RESPONSE_FOUND = 302,
        RESPONSE_REDIRECT = 307,
        RESPONSE_BAD_REQUEST = 400,
        RESPONSE_NOT_FOUND = 404
    };
    enum {
        STATE_IDLE,
        STATE_GET_VERSION,
        STATE_GET_RELEASES,
        STATE_DOWNLOADING_IMAGE,
        STATE_WRITING_IMAGE
    } state;
    enum {
        STACK_WIDGET_MAIN = 0,
        STACK_WIDGET_ABOUT = 1
    };

    int timerId;
    QTimer *devicesTimer;
    qlonglong bytesDownloaded;
    QString imageFileName;
    QCryptographicHash imageHash;
    QFile imageFile;
    QString downloadUrl;
    QString downloadFileSize;
    QString checksum;
    QString selectedImage;
    QMap<QString, QString> checksumMap;
    DiskWriter *diskWriter;
    QThread* diskWriterThread;
    DeviceEnumerator* devEnumerator;
    static const int timerValue;
    static const QString releasesUrl;
    static const QString versionUrl;
    static const QString helpUrl;
    JsonParser *parserData;
    QSettings settings;
    QElapsedTimer speedTime;
    qlonglong bytesLast;
    MovingAverage *averageSpeed;
    unsigned int uncompressedImageSize;
    Privileges privileges;
    QString deviceEjected;
    bool showLoadEject;

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void proceedToWriteImageToDevice(const QString& image, const QString& device);
    void error(const QString& message);

private slots:
    void httpsUrlHandler(const QUrl &url);
    void changeEvent(QEvent * e);
    void ejectUSB();
    void loadUSB();
    void removeUSB();
    void showHelp();
    void showAbout();
    void closeAbout();

    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleFinishedDownload(const QByteArray& data);
    void handlePartialData(const QByteArray& data, qlonglong total);
    void handleDownloadError(const QString);
    void downloadVersionCheck();
    void checkNewVersion(const QString &version);
    void downloadReleases();
    void parseJsonAndSet(const QByteArray &data);
    void setProjectImages();
    void projectImagesShowAllChanged(int state);
    void projectImagesChanged(const QString& version);
    void refreshRemovablesList();
    void downloadButtonClicked();
    void getImageFileNameFromUser();
    void writeFlashButtonClicked();
    void writingSyncing();
    void writingFinished();
    void writingError(QString);
    void reset(const QString& message = "");
    void resetProgressBars();
    void savePreferredImage(const QString &version);
    void savePreferredRemovableDevice(int idx);
    void languageChange();
    void downloadProgressBarText(const QString &text);
    void flashProgressBarText(const QString &text);
    void handleWriteProgress(int written);
};

#endif // CREATOR_H
