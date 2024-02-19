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

#ifdef Q_OS_MACOS
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
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *) override;
    void closeEvent(QCloseEvent *) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void changeEvent(QEvent *e) override;

signals:
    void proceedToWriteImageToDevice(const QString& image, const QString& device, const QString& deviceText);
    void error(const QString& message);

private slots:
    void httpsUrlHandler(const QUrl &url);
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
