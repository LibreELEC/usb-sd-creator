////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
//      Copyright (C) 2013-2015 RasPlex project
//      Copyright (C) 2016-Present Team LibreELEC
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

#include "creator.h"
#include "ui_creator.h"

#include <QRegularExpression>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QPlainTextEdit>
#include <QStyleFactory>
#include <QDesktopServices>
#include <QMimeData>
#include <QProcess>
#include <QVersionNumber>
#include <QSignalBlocker>
#include <QApplication>

#if defined(Q_OS_WIN)
#include "diskwriter_windows.h"
#include "deviceenumerator_windows.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#include "diskwriter_udisks2.h"
#include "deviceenumerator_udisks2.h"
#elif defined(Q_OS_UNIX)
#include <unistd.h>
#include "diskwriter_unix.h"
#include "deviceenumerator_unix.h"
#endif

// force update notification dialog
//#define FORCE_UPDATE_NOTIFICATION "1.3"

const QString Creator::releasesUrl = "http://releases.libreelec.tv/";
const QString Creator::versionUrl = releasesUrl + "creator_version";
const QString Creator::helpUrl = "https://wiki.libreelec.tv/installation/create-media";
const int Creator::timerValue = 1500;  // msec

Creator::Creator(Privileges &privilegesArg, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Creator),
    manager(new DownloadManager(this)),
    state(STATE_IDLE),
    imageHash(QCryptographicHash::Sha256),
    settings(QSettings::IniFormat, QSettings::UserScope, "LibreELEC", "LibreELEC.USB-SD.Creator"),
    privileges(privilegesArg),
    deviceEjected("")
{
    restoreGeometry(settings.value("window/geometry").toByteArray());

    ui->setupUi(this);

#ifdef Q_OS_MACOS
    auto fontAbout = ui->labelAbout->font();
    fontAbout.setPointSize(fontAbout.pointSize() + 2);
    ui->labelAbout->setFont(fontAbout);
#endif

#if defined(Q_OS_WIN)
    diskWriter = new DiskWriter_windows();
    devEnumerator = new DeviceEnumerator_windows();
#elif defined(Q_OS_LINUX)
    diskWriter = new DiskWriter_udisks2();
    devEnumerator = new DeviceEnumerator_udisks2();
#elif defined(Q_OS_UNIX)
    diskWriter = new DiskWriter_unix();
    devEnumerator = new DeviceEnumerator_unix();
#endif
    diskWriterThread = new QThread(this);
    diskWriter->moveToThread(diskWriterThread);

    // must set before signals
    if (settings.value("preferred/imageshowall") == Qt::Checked)
        ui->imagesShowAll->setChecked(true);  // default unchecked

    // hide ? button
    this->setWindowFlags(this->windowFlags() & ~(Qt::WindowContextHelpButtonHint));
    // add minimize button on Windows
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    connect(diskWriterThread, SIGNAL(finished()),
            diskWriter, SLOT(deleteLater()));
    connect(this, SIGNAL(proceedToWriteImageToDevice(QString,QString,QString)),
            diskWriter, SLOT(writeImageToRemovableDevice(QString,QString,QString)));

    connect(diskWriter, SIGNAL(bytesWritten(int)),this, SLOT(handleWriteProgress(int)));
    connect(diskWriter, SIGNAL(syncing()), this, SLOT(writingSyncing()));
    connect(diskWriter, SIGNAL(finished()), this, SLOT(writingFinished()));
    connect(diskWriter, SIGNAL(error(QString)), this, SLOT(writingError(QString)));
    diskWriterThread->start();

    connect(ui->refreshRemovablesButton,SIGNAL(clicked()),
            this,SLOT(refreshRemovablesList()));

    connect(manager, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(handleDownloadProgress(qint64, qint64)));
    connect(manager, SIGNAL(downloadComplete(QByteArray)),
            this, SLOT(handleFinishedDownload(QByteArray)));
    connect(manager, SIGNAL(partialData(QByteArray,qlonglong)),
            this, SLOT(handlePartialData(QByteArray,qlonglong)));
    connect(manager, SIGNAL(downloadError(QString)),
            this, SLOT(handleDownloadError(QString)));

    connect(ui->downloadButton, SIGNAL(clicked()),
            this, SLOT(downloadButtonClicked()));
    connect(ui->loadButton, SIGNAL(clicked()),
            this, SLOT(getImageFileNameFromUser()));
    connect(ui->writeFlashButton, SIGNAL(clicked()),
            this, SLOT(writeFlashButtonClicked()));

    connect(ui->imagesShowAll, SIGNAL(stateChanged(int)),
            this, SLOT(projectImagesShowAllChanged(int)));
    connect(ui->projectSelectBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setProjectImages()));
    connect(ui->imageSelectBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(projectImagesChanged(QString)));

    connect(ui->removableDevicesComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(savePreferredRemovableDevice(int)));

    connect(ui->ejectUSB, SIGNAL(clicked()), this, SLOT(ejectUSB()));
    connect(ui->loadUSB, SIGNAL(clicked()), this, SLOT(loadUSB()));
    connect(ui->removeUSB, SIGNAL(clicked()), this, SLOT(removeUSB()));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->showAboutButton, SIGNAL(clicked()), this, SLOT(showAbout()));
    connect(ui->closeAboutButton, SIGNAL(clicked()), this, SLOT(closeAbout()));
    connect(ui->closeAppButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui->langButton,SIGNAL(clicked()), this, SLOT(languageChange()));

    refreshRemovablesList();

    // create a timer that refreshes the device list every 1.5 second
    // if there is any change then list is changed and current device removed
    timerId = startTimer(timerValue);

    // set Fusion style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    // and apply some changes to styles

#ifdef Q_OS_MACOS
    QFile fileStyle(":/qss/stylesheet_osx.qss");
#else
    QFile fileStyle(":/qss/stylesheet.qss");
#endif
    if (fileStyle.open(QIODevice::ReadOnly | QIODevice::Text)) {
        this->setStyleSheet(QLatin1String(fileStyle.readAll()));
        this->ensurePolished();
        fileStyle.close();
    }

    setImageFileName("");
    ui->writeFlashButton->setEnabled(false);

    showLoadEject = false;  // disabled by default

#ifdef Q_OS_WIN
    // this option is only for me on Windows
    // disabled on Linux and OS X
    showLoadEject = settings.value("preferred/showloadeject", false).toBool();
    settings.setValue("preferred/showloadeject", showLoadEject);
    if (showLoadEject == false)
#endif
    {
        ui->labelEjectLoad->setVisible(false);
        ui->ejectUSB->setEnabled(false);
        ui->ejectUSB->setVisible(false);
        ui->loadUSB->setEnabled(false);
        ui->loadUSB->setVisible(false);
        ui->removeUSB->setEnabled(false);
        ui->removeUSB->setVisible(false);
    }

    setAcceptDrops(true);    // allow droping files on a window

    // singleShot fixes broken focus behavior if the message is shown on macOS
    QTimer::singleShot(0, this, &Creator::showRootMessageBox);

    // call web browser through our wrapper for Linux
    QDesktopServices::setUrlHandler("http", this, "httpsUrlHandler");
    QDesktopServices::setUrlHandler("https", this, "httpsUrlHandler");

    translator = new Translator(this, &settings);  // pass parent
    translator->fillLanguages(ui->menuLanguage, ui->langButton);

    retranslateUi();  // retranslate dynamic texts

    downloadVersionCheck();
}

bool Creator::showRootMessageBox()
{
#ifdef Q_OS_MACOS
    if (getuid() == 0)  // root == 0, real user != 0
        return false;

    QMessageBox msgBox(this);
    msgBox.setText(tr("Root privileges required to write image.\nRun application with sudo."));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    return true;
#else
    return false;
#endif
}

Creator::~Creator()
{
    if (imageFile.isOpen() && state == STATE_DOWNLOADING_IMAGE) {
        qDebug() << "Removing file" << imageFile.fileName();
        imageFile.remove();
    } else if (state == STATE_WRITING_IMAGE) {
        privileges.SetUser();    // back to user
    }

    delete ui;
    diskWriter->cancelWrite();
    diskWriterThread->quit();
    diskWriterThread->wait();
    delete diskWriterThread;
    delete devEnumerator;
    delete parserData;
}

void Creator::httpsUrlHandler(const QUrl &url)
{
    // on windows open web browser directly
    // for linux use a wrapper to set uid/gid correctly
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    QDesktopServices::openUrl(url);
#else
    qDebug() << "httpsUrlHandler called" << url;

    pid_t pid = fork();
    if (pid == 0) {
        // child process, set both real and effective uid/gid
        // because GTK+ applications check this and doesn't run
        setenv("DBUS_SESSION_BUS_ADDRESS", privileges.GetUserEnvDbusSession().toLatin1().data(), 1);
        setenv("LOGNAME", privileges.GetUserEnvLogname().toLatin1().data(), 1);
        privileges.SetRoot();       // no need to switch back
        privileges.SetRealUser();   // no need to switch back
        QDesktopServices::openUrl(QUrl(url));
        _exit(0);
    }

#if 0
    QString program = QCoreApplication::applicationFilePath();
    QStringList arguments = QStringList("--browser");

    // root is needed to start the process which
    // will be dropped back to user (both real and effective uid/gid)
    privileges.SetRoot();

    setenv("DBUS_SESSION_BUS_ADDRESS", privileges.GetUserEnvDbusSession().toLatin1().data(), 1);
    setenv("LOGNAME", privileges.GetUserEnvLogname().toLatin1().data(), 1);
    setenv("LE_URL_ADDRESS", url.toString().toLatin1().data(), 1);

    QProcess myProcess;
    myProcess.startDetached(program, arguments);
    myProcess.waitForStarted();
    myProcess.waitForFinished();

    privileges.SetUser();    // back to user
#endif

    qDebug() << "httpsUrlHandler done";
#endif
}

void Creator::setArgFile(QString file)
{
    if (file.isEmpty())
        return;

    QFileInfo checkFile(file);
    if (!checkFile.exists() || !checkFile.isFile())
        return;

    QFileInfo infoFile(file);
    file = infoFile.absoluteFilePath();
    setImageFileName(file);
}

void Creator::retranslateUi()
{
    // retranslate dynamic texts
    ui->labelVersion->setText(tr("Version: %1\nBuild date: %2").arg(QLatin1String{BUILD_VERSION}, QLatin1String{BUILD_DATE}));

    ui->labelAbout->setText(QString("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt; font-weight:600;\"><h2>&copy; LibreELEC %8</h2></span></p><p align=\"center\">%1<br/>%2</p><p align=\"center\">%3<br/><a href=\"https://github.com/LibreELEC/usb-sd-creator\"><span style=\" text-decoration: underline; color:#0000ff;\">https://github.com/LibreELEC/usb-sd-creator</span></a><br/></p><p align=\"center\">%4<br/>%5</p><p align=\"center\">%6<br/>%7 <br/><br/><a href=\"https://opencollective.com/libreelec/donate\"><img src=\":/icons/opencollective.png\"></a></p></body></html>") \
          .arg(tr("This software was created with love and released"))
          .arg(tr("under GPLv2, using earlier work from RasPlex."))
          .arg(tr("For license, credits and history, please read:"))
          .arg(tr("If you enjoy using LibreELEC please consider a"))
          .arg(tr("donation to support the project."))
          .arg(tr("Click the logo below to donate"))
          .arg(tr("using OpenCollective"))
          .arg(QLatin1String{COPYRIGHT_YEARS})
    );

    // orientation of the widget is reversed
    if (QApplication::isLeftToRight())
        ui->imagesShowAll->setLayoutDirection(Qt::RightToLeft);
    else
        ui->imagesShowAll->setLayoutDirection(Qt::LeftToRight);
}

void Creator::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        return;   // ignore Esc key for close
}

void Creator::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    settings.setValue("window/geometry", saveGeometry());
}

void Creator::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void Creator::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls()) {
        QString file = url.toLocalFile();
        QFileInfo infoFile(file);
        file = infoFile.absoluteFilePath();
        setImageFileName(file);
        reset();

        // hide selected project and image name
        ui->projectSelectBox->blockSignals(true);
        ui->imageSelectBox->blockSignals(true);

        ui->projectSelectBox->setCurrentIndex(-1);
        ui->imageSelectBox->setCurrentIndex(-1);

        ui->projectSelectBox->blockSignals(false);
        ui->imageSelectBox->blockSignals(false);

        // and disable download button
        ui->downloadButton->setEnabled(false);

        break;  // only first file
    }
}

void Creator::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    refreshRemovablesList();
}

void Creator::changeEvent(QEvent *e) {
    switch (e->type()) {
    case QEvent::ActivationChange:
        if (this->isActiveWindow()) {
            // got focus
            if (timerId == 0)
                timerId = startTimer(timerValue);
        } else {
            // lost focus
            if (timerId > 0) {
                killTimer(timerId);
                timerId = 0;
            }
        }

        break;
    case QEvent::LanguageChange:
        ui->retranslateUi(this);  // retranslate texts from .ui file
        retranslateUi();  // retranslate dynamic texts
        break;
    default:
        break;
    }
}

void Creator::ejectUSB()
{
    int idx = ui->removableDevicesComboBox->currentIndex();
    deviceEjected = ui->removableDevicesComboBox->itemData(idx).toString();
    if (deviceEjected.isNull())
        return;

    qDebug() << "ejectUSB" << deviceEjected;
    int rv = devEnumerator->loadEjectDrive(deviceEjected, DeviceEnumerator::LOADEJECT_EJECT);
    Q_UNUSED(rv);
}

void Creator::loadUSB()
{
    if (deviceEjected.isNull())
        return;

    qDebug() << "loadUSB" << deviceEjected;
    int rv = devEnumerator->loadEjectDrive(deviceEjected, DeviceEnumerator::LOADEJECT_LOAD);
    deviceEjected = "";   // init
    Q_UNUSED(rv);
}

void Creator::removeUSB()
{
    int idx = ui->removableDevicesComboBox->currentIndex();
    deviceEjected = ui->removableDevicesComboBox->itemData(idx).toString();
    if (deviceEjected.isNull())
        return;

    qDebug() << "removeUSB" << deviceEjected;
    int rv = devEnumerator->removeDrive(deviceEjected);
    Q_UNUSED(rv);
}

void Creator::showHelp()
{
    QDesktopServices::openUrl(QUrl(helpUrl));
}

void Creator::showAbout()
{
    ui->stackedWidget->setCurrentIndex(STACK_WIDGET_ABOUT);
}

void Creator::closeAbout()
{
    ui->stackedWidget->setCurrentIndex(STACK_WIDGET_MAIN);
}

void Creator::downloadProgressBarText(const QString &text = "")
{
    ui->downloadProgressBar->setFormat("   " + text);
    ui->downloadProgressBar->repaint();
    ui->downloadProgressBar->update();
    //qApp->processEvents();  // don't use this (signals lost and boooom)
}

void Creator::flashProgressBarText(const QString &text = "")
{
    ui->flashProgressBar->setFormat("   " + text);
    ui->flashProgressBar->repaint();
    ui->flashProgressBar->update();
    //qApp->processEvents();
}

void Creator::parseJsonAndSet(const QByteArray &data)
{
    //qDebug() << "JSON data:" << data;
    parserData = new JsonParser(data);

    // parse local file if exist
    QFile fileLocalReleases("releases-user.json");
    if (fileLocalReleases.open(QIODevice::ReadOnly | QIODevice::Text)) {
        parserData->addExtra(fileLocalReleases.readAll(), "User");
        fileLocalReleases.close();
    }

    ui->projectSelectBox->clear();

    QList<ProjectData> projectList = parserData->getProjectData();
    for (auto& project : projectList) {
        QString projectName = project.name;
        QString projectId = project.id;
        QString projectUrl = project.url;

        QVariantMap projectData;
        projectData.insert("id", projectId);
        projectData.insert("url", projectUrl);
        ui->projectSelectBox->insertItem(0, projectName, projectData);
        ui->projectSelectBox->setItemData(0, projectId, Qt::ToolTipRole);
    }

    QString previouslySelectedProject;
    previouslySelectedProject = settings.value("preferred/project").toString();

    // RPi2/3 is default project
    if (previouslySelectedProject.isEmpty())
        previouslySelectedProject = "Raspberry Pi 2 and 3";

    int idx = ui->projectSelectBox->findText(previouslySelectedProject,
                                             Qt::MatchFixedString);
    if (idx >= 0)
        ui->projectSelectBox->setCurrentIndex(idx);

    settings.setValue("preferred/project", ui->projectSelectBox->currentText());
    setProjectImages();
    resetProgressBars();  // it is affected with all downloads
}

void Creator::setProjectImages()
{
    downloadProgressBarText();
    //ui->fileNameLabel->setText("");

    // last selected is preferred
    settings.setValue("preferred/project", ui->projectSelectBox->currentText());

    QString previouslySelectedImage;
    if (ui->imageSelectBox->count() == 0)
        previouslySelectedImage = settings.value("preferred/image").toString();
    else
        previouslySelectedImage = ui->imageSelectBox->currentText();

    {
        const QSignalBlocker blocker{ui->imageSelectBox};
        ui->imageSelectBox->clear();
    }

    QList<ProjectData> projectList = parserData->getProjectData();
    for (auto& project : projectList) {
        QString projectName = project.name;

        // show images only for selected project
        if (projectName != ui->projectSelectBox->currentText())
            continue;

        QString lastVersionNum;

        QList<QVariantMap> releases = project.images;
        for (QList<QVariantMap>::const_iterator it = releases.constBegin();
             it != releases.constEnd();
             it++)
        {
            QString imageName = (*it)["name"].toString();
            QString imageChecksum = (*it)["sha256"].toString();
            QString imageSize = (*it)["size"].toString();

            QString versionNum;
            QRegularExpression versionNumRegExp = QRegularExpression("-([0-9]+\\.[0-9]+\\.[0-9]+).*\\.img\\.gz");
            QRegularExpressionMatch versionNumMatch = versionNumRegExp.match(imageName);
            if (versionNumMatch.hasMatch())
                versionNum = versionNumMatch.captured(1);

            // if we don't show all images, break after the version number changes
            // note that multiple latest image numbers are possible with hardware variations
            // e.g LibreELEC-A64.arm-9.95.4-orangepi-win.img.gz or LibreELEC-A64.arm-9.95.4-pine64-lts.img.g
            if (!ui->imagesShowAll->isChecked() && !lastVersionNum.isEmpty() && lastVersionNum != versionNum)
                break;

            int size = imageSize.toInt();
            if (size < 1024) {
                imageSize = QString::number(size) + " B";
            } else if (size < 1024*1024) {
                size /= 1024;
                imageSize = QString::number(size) + " kB";
            } else {
                size /= 1024*1024;
                imageSize = QString::number(size) + " MB";
            }

            // LibreELEC-RPi2.arm-7.90.002.img.gz
            // LibreELEC-TinkerBoard.arm-8.90.015-rk3288.img.gz
            QRegularExpression regExp = QRegularExpression(".+-[0-9]+\\.(9[05])\\.[0-9]+.*\\.img\\.gz");
            QRegularExpressionMatch match = regExp.match(imageName);
            QStringList regExpVal = match.capturedTexts();
            QString alphaBetaNumber;

            alphaBetaNumber = tr("[Stable]");
            if (regExpVal.count() == 2) {
                if (regExpVal.at(1) == "90")
                    alphaBetaNumber = tr("[Alpha]");
                else if (regExpVal.at(1) == "95")
                    alphaBetaNumber = tr("[Beta]");
            }

            if (! ui->imagesShowAll->isChecked()) {
                // check value (number 90 or 95)
                if (alphaBetaNumber != tr("[Stable]"))
                  continue;    // skip testing images
            }

            checksumMap[imageName] = imageChecksum;
            ui->imageSelectBox->insertItem(0, imageName + ", " + imageSize, imageName);
            ui->imageSelectBox->setItemData(0, alphaBetaNumber + " " + releasesUrl + imageName, Qt::ToolTipRole);

            lastVersionNum = versionNum;
        }
    }

    int idx = ui->imageSelectBox->findText(previouslySelectedImage,
                                           Qt::MatchFixedString);
    if (idx >= 0)
        ui->imageSelectBox->setCurrentIndex(idx);

    savePreferredImage(ui->imageSelectBox->currentText());

    reset();
    downloadProgressBarText();
}

void Creator::projectImagesShowAllChanged(int state)
{
    settings.setValue("preferred/imageshowall", state);
    setProjectImages();
}

void Creator::projectImagesChanged(const QString& version)
{
    downloadProgressBarText();
    ui->fileNameLabel->setText("");
    savePreferredImage(version);

    // in case user file was selected then both project and image was empty
    // try setting project
    QString previouslySelectedProject;
    previouslySelectedProject = settings.value("preferred/project").toString();

    int idx = ui->projectSelectBox->findText(previouslySelectedProject,
                                             Qt::MatchFixedString);
    if (idx >= 0)
        ui->projectSelectBox->setCurrentIndex(idx);


}

void Creator::reset(const QString& message)
{
    bytesDownloaded = 0;
    bytesLast = 0;

    if (imageFile.isOpen())
        imageFile.close();

    ui->imagesShowAll->setEnabled(true);
    ui->projectSelectBox->blockSignals(false);
    ui->projectSelectBox->setEnabled(true);

    ui->imageSelectBox->blockSignals(false);
    ui->imageSelectBox->setEnabled(true);

    ui->downloadButton->setEnabled(true);
    ui->downloadButton->setText(tr("&Download"));

    ui->loadButton->setEnabled(true);

    ui->refreshRemovablesButton->setEnabled(true);
    ui->removableDevicesComboBox->setEnabled(true);

    int idx = ui->removableDevicesComboBox->currentIndex();
    QString destination = ui->removableDevicesComboBox->itemData(idx).toString();
    QString file = ui->fileNameLabel->text();

    if (file.isFilled()) {
        QFileInfo checkFile(file);
        if (!checkFile.exists() || !checkFile.isFile())
            file = "";
        else {
            QFileInfo infoFile(file);
            file = infoFile.absoluteFilePath();
            ui->fileNameLabel->setText(file);
            setImageFileName(file);
        }
    }

    if (destination.isNull() == false && file.isFilled())
        ui->writeFlashButton->setEnabled(true);
    else
        ui->writeFlashButton->setEnabled(false);

    ui->writeFlashButton->setText(tr("&Write"));

    if (message.isNull() == false) {
        if (state == STATE_DOWNLOADING_IMAGE) {
            ;
        } else if (state == STATE_WRITING_IMAGE) {
            flashProgressBarText(message);
        }
    }

    state = STATE_IDLE;

    // TBD - USB eject/load/remove
}

void Creator::resetProgressBars()
{
    ui->downloadProgressBar->setValue(0);
    ui->flashProgressBar->setValue(0);
    downloadProgressBarText();
    flashProgressBarText();
}

void Creator::savePreferredImage(const QString& version)
{
    settings.setValue("preferred/image", version);
}

void Creator::savePreferredRemovableDevice(int idx)
{
    if (idx < 0 )
        return;

    settings.setValue("preferred/removableDevice", ui->removableDevicesComboBox->itemData(idx).toString());
    flashProgressBarText("");
}

void Creator::languageChange()
{
    // menu has padding around
    ui->menuLanguage->exec(ui->langButton->mapToGlobal(QPoint(0, 0)));
}

void Creator::disableControls(const int which)
{
    ui->imagesShowAll->setEnabled(false);
    ui->projectSelectBox->setEnabled(false);
    ui->projectSelectBox->blockSignals(true);
    ui->imageSelectBox->setEnabled(false);
    ui->imageSelectBox->blockSignals(true);
    ui->refreshRemovablesButton->setEnabled(false);
    ui->removableDevicesComboBox->setEnabled(false);

    if (which == DISABLE_CONTROL_DOWNLOAD) {
        ui->writeFlashButton->setEnabled(false);
    } else {
        ui->downloadButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
    }

    // TBD - USB eject/load/remove
}

bool Creator::isChecksumValid(const QString checksumSha256)
{
    checksum = checksumMap[selectedImage];

    if (checksumSha256.isFilled() && checksumSha256 == checksum)
        return true;  // checksum calculated at download stage

    QByteArray referenceSum, downloadSum;
    QCryptographicHash c(QCryptographicHash::Sha256);

    // calculate the sha256 sum of the downloaded file
    imageFile.open(QFile::ReadOnly);
    while (!imageFile.atEnd())
        c.addData(imageFile.read(4096));

    downloadSum = c.result().toHex();

    imageFile.close();

    qDebug() << selectedImage << checksum;

    if (checksum.isEmpty() || downloadSum != checksum.toUtf8())
        return false;

    return true;
}

// From http://www.gamedev.net/topic/591402-gzip-uncompressed-file-size/
// Might not be portable!
unsigned int Creator::getUncompressedImageSize()
{
    FILE *file;
    unsigned int len;
    unsigned char bufSize[4];
    unsigned int fileSize;

#if defined(_WIN32)
    // toStdString internally converts filename to utf8, which
    // windows does not support for fileaccess
    // so use unchanged 16 Bit unicode here (QChar is 16 Bit)
    file = _wfopen((const wchar_t *)imageFile.fileName().utf16(), L"rb");
#else
    file = fopen(imageFile.fileName().toStdString().c_str(), "rb");
#endif
    if (file == NULL)
    {
        emit error("Couldn't open " + imageFile.fileName());
        return 0;
    }

    if (imageFile.fileName().endsWith(".gz")) {
        if (fseek(file, -4, SEEK_END) != 0)
            return 0;

        len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
        if (len != 4) {
            fclose(file);
            return 0;
        }

        fileSize = (unsigned int) ((bufSize[3] << 24) | (bufSize[2] << 16) | (bufSize[1] << 8) | bufSize[0]);
        qDebug() << "Uncompressed gz file size:" << fileSize;
    } else if (imageFile.fileName().endsWith(".zip")) {
        // first check uncompressed size from header
        if (fseek(file, 22, SEEK_SET) != 0)
            return 0;

        len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
        if (len != 4) {
            fclose(file);
            return 0;
        }

        fileSize = (unsigned int) ((bufSize[3] << 24) | (bufSize[2] << 16) | (bufSize[1] << 8) | bufSize[0]);

        // check general-purpose flags
        if (fseek(file, 6, SEEK_SET) != 0)
            return 0;

        len = fread(&bufSize[0], sizeof(unsigned char), 2, file);
        if (len != 2) {
            fclose(file);
            return 0;
        }

        qDebug() << "fileSize" << fileSize << "general-purpose flag" << (bufSize[0] & 0x08);
        if (fileSize == 0 && (bufSize[0] & 0x08) != 0) {
            // get size from structure immediately after the
            // compressed data (at the end of the file)

            // get End of central directory record (EOCD)
            long off;
            for (off = 0;; off++) {
                qDebug() << "off:" << off;
                if (fseek(file, -22 - off, SEEK_END) == -1)
                    break;  // error

                len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
                if (len != 4) {
                    fclose(file);
                    return 0;
                }

                // check End of central directory signature = 0x06054b50
                if (bufSize[3] == 0x06 && bufSize[2] == 0x05 && \
                    bufSize[1] == 0x4b && bufSize[0] == 0x50)
                {
                    qDebug() << "found End of central directory signature = 0x06054b50";
                    break;  // exit loop
                }
            } // for

            off = 16 - 4;  // 4 B already read

            // Offset of start of central directory, relative to start of archive
            if (fseek(file, off, SEEK_CUR) == -1) {
                fclose(file);
                return 0;
            }

            len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
            if (len != 4) {
                fclose(file);
                return 0;
            }

            // calculate offset
            off = (long) ((bufSize[3] << 24) | (bufSize[2] << 16) | (bufSize[1] << 8) | bufSize[0]);

            if (fseek(file, off, SEEK_SET) == -1) {
                fclose(file);
                return 0;
            }

            len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
            if (len != 4) {
                fclose(file);
                return 0;
            }

            // check Central directory file header signature = 0x02014b50
            if (bufSize[3] == 0x02 && bufSize[2] == 0x01 && bufSize[1] == 0x4b && bufSize[0] == 0x50) {
                qDebug() << "found Central directory file header signature = 0x02014b50";
                off = 24 - 4;  // 4 B already read
            }

            if (fseek(file, off, SEEK_CUR) == -1) {
                fclose(file);
                return 0;
            }

            len = fread(&bufSize[0], sizeof(unsigned char), 4, file);
            if (len != 4) {
                fclose(file);
                return 0;
            }

            // Uncompressed size
            fileSize = (unsigned int) ((bufSize[3] << 24) | (bufSize[2] << 16) | (bufSize[1] << 8) | bufSize[0]);

            if (fileSize == 0) {
                qDebug() << "fileSize unknown - set 512 MB";
                fileSize = 512 * 1024 * 1024;   // test
            }
        } // fileSize == 0

        qDebug() << "Uncompressed zip file size:" << fileSize;
    } else {
        fseek(file, 0L, SEEK_END);
        fileSize = ftell(file);
        qDebug() << "Regular file size:" << fileSize;
    }

    fclose(file);
    return fileSize;
}

void Creator::setImageFileName(QString filename)
{
    if (imageFile.isOpen()) {
        qDebug() << "Tried to change filename while imageFile was open!";
        return;
    }

    imageFile.setFileName(filename);

    if (filename.endsWith(".temp"))
        filename = filename.left(filename.lastIndexOf("."));

    ui->fileNameLabel->setText(filename);
}

QString Creator::getDefaultSaveDir()
{
    static QString defaultDir;
    if (defaultDir.isEmpty()) {
        defaultDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        if (defaultDir.isEmpty())
            defaultDir = QDir::homePath();
    }

    return defaultDir;
}

void Creator::handleDownloadError(const QString message)
{
    qDebug() << "Something went wrong with download:" << message;
    downloadProgressBarText(message);

    if (state == STATE_GET_VERSION)
        downloadReleases();
}

void Creator::handleFinishedDownload(const QByteArray &data)
{
    switch (state) {
    case STATE_GET_VERSION:
        state = STATE_IDLE;
#ifdef FORCE_UPDATE_NOTIFICATION
        checkNewVersion(FORCE_UPDATE_NOTIFICATION);
#else
        checkNewVersion(data);
#endif
        downloadReleases();
        break;

    case STATE_GET_RELEASES:
        parseJsonAndSet(data);
        ui->downloadButton->setEnabled(true);
        state = STATE_IDLE;
        break;

    case STATE_DOWNLOADING_IMAGE:
        // whole data at once (no partial)
        if (bytesDownloaded == 0) {
            downloadProgressBarText(tr("Download complete, syncing file..."));
            qApp->processEvents();  // fix this
            handlePartialData(data, data.size());
        }

        resetProgressBars();

        imageFile.close();
        downloadProgressBarText(tr("Download complete, verifying checksum..."));

        if (isChecksumValid(imageHash.result().toHex()))
            downloadProgressBarText(tr("Download complete, checksum ok."));
        else
            downloadProgressBarText(tr("Download complete, checksum not ok."));

        // rename file
        if (imageFile.fileName().endsWith(".temp")) {
            QString newFileName = imageFile.fileName().left(imageFile.fileName().lastIndexOf("."));

            bool success = imageFile.rename(newFileName);
            if (success)
              qDebug() << "rename ok";
            else
              qDebug() << "rename error";
        }

        delete averageSpeed;
        reset();
        state = STATE_IDLE;

        QApplication::beep();
        QApplication::alert(this, 5000);
        break;

    default:
        qDebug() << "handleFinishedDownload default";
        break;
    }
}

void Creator::handlePartialData(const QByteArray &data, qlonglong total)
{
    Q_UNUSED(total);

    if (state != STATE_DOWNLOADING_IMAGE) {
        // what to do in this case?
        qDebug() << "handlePartialData: got unexpected data!";
        return;
    }

    imageFile.write(data);
    imageHash.addData(data);
    bytesDownloaded += data.size();
}

void Creator::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (state != STATE_DOWNLOADING_IMAGE)
        return;

    if (bytesTotal < 10000)
        return;   // skip json file (to be fixed)

    // Update progress bar
    ui->downloadProgressBar->setMaximum(bytesTotal);
    ui->downloadProgressBar->setValue(bytesReceived);

    // calculate current download speed
    double speed;
    int elapsedTime = speedTime.elapsed();
    if (elapsedTime <= 0)
        elapsedTime = 1;  // don't delete by zero

    speed = (bytesReceived - bytesLast) * 1000.0 / elapsedTime;
    averageSpeed->AddValue(speed);
    speed = averageSpeed->AverageValue();

    double remainingTime = (bytesTotal - bytesReceived) / speed;  // in seconds
    bytesLast = bytesReceived;

    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";

        if (speed > 1000)
            speed = 0;
    }

    QString speedText = QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit);
    QString timeText = QString::number(remainingTime, 'f', 0);
    int percentage = ((double) bytesReceived) / bytesTotal * 100;

    downloadProgressBarText(tr("%1 seconds remaining - %2% at %3").arg(timeText, QString::number(percentage), speedText));

    speedTime.restart();   // start again to get current speed
}

void Creator::downloadVersionCheck()
{
    state = STATE_GET_VERSION;
    ui->downloadButton->setEnabled(false);
    disableControls(DISABLE_CONTROL_DOWNLOAD);

    QUrl url(versionUrl);
    manager->get(url);
}

void Creator::downloadReleases()
{
    state = STATE_GET_RELEASES;
    ui->downloadButton->setEnabled(false);
    disableControls(DISABLE_CONTROL_DOWNLOAD);

    QUrl url(releasesUrl + "releases.json");
    manager->get(url);
}

void Creator::checkNewVersion(const QString &verNewStr)
{
    QVersionNumber qVersionNew = QVersionNumber::fromString(verNewStr);
    QVersionNumber qVersionOld = QVersionNumber::fromString(BUILD_VERSION);


    int QVersionCompare = QVersionNumber::compare(qVersionNew, qVersionOld);
    qDebug() << "QVersionCompare" << QVersionCompare;

    if (QVersionCompare <= 0) {
        qDebug() << "no new version";
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Update Notification"));
#ifdef Q_OS_MACOS
    QAbstractButton *visitButton = msgBox.addButton(tr("&Visit Website"), QMessageBox::NoRole);
    msgBox.addButton(tr("&Close"), QMessageBox::YesRole);
#else
    QAbstractButton *visitButton = msgBox.addButton(tr("&Visit Website"), QMessageBox::YesRole);
    msgBox.addButton(tr("&Close"), QMessageBox::NoRole);
#endif
    QString verHtml = "<font color=\"blue\">" + verNewStr + "</font>";
    QString msg = tr("LibreELEC USB-SD Creator %1 is available.").arg(verHtml);
    msgBox.setText("<p align='center' style='margin-right:30px'><br>" + msg + "<br></p>");

    msgBox.exec();
    if (msgBox.clickedButton() == visitButton)
      QDesktopServices::openUrl(QUrl(helpUrl));
}

void Creator::downloadButtonClicked()
{
    if (state == STATE_DOWNLOADING_IMAGE) {
        state = STATE_IDLE;
        // cancel download
        manager->cancelDownload();

        // remove temp file
        if (imageFile.isOpen()) {
            qDebug() << "Removing file" << imageFile.fileName();
            imageFile.remove();
        }

        resetProgressBars();
        downloadProgressBarText(tr("Download canceled."));
        reset();
        return;
    }

    // start download
    state = STATE_DOWNLOADING_IMAGE;
    disableControls(DISABLE_CONTROL_DOWNLOAD);

    QString imageName = ui->imageSelectBox->currentText();
    selectedImage = imageName.section(',', 0, 0);   // remove size
    qDebug() << "selectedImage" << selectedImage;

    QString projectUrl = ui->projectSelectBox->itemData(ui->projectSelectBox->currentIndex()).toMap()["url"].toString();
    if (projectUrl == "")
        projectUrl = releasesUrl;

    QUrl url = projectUrl + selectedImage;

    qDebug() << "Downloading" << url;

    QString saveDir = settings.value("preferred/savedir", getDefaultSaveDir()).toString();

    qDebug() << "saveDir" << saveDir;
    qDebug() << "Whole path" << saveDir + '/' + selectedImage;

    saveDir = QFileDialog::getExistingDirectory(this,
                tr("Directory to store image file"),
                saveDir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (saveDir.isEmpty() || selectedImage.isEmpty()) {
        reset();
        return;
    }

    QFile fileTest(saveDir + "/" + selectedImage);
    if (fileTest.exists()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setText(tr("File \n%1/%2\nalready exist.").arg(saveDir).arg(selectedImage));
        msgBox.setInformativeText(tr("Do you want to overwrite?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret != QMessageBox::Yes) {
            downloadProgressBarText(tr("File already exists."));
            reset();
            return;
        }
    }

    qDebug() << "Downloading to" << saveDir + " " + selectedImage;

    if (imageFile.isOpen())
        imageFile.close();

    // remove both files
    QFile fileToRemove(saveDir + "/" + selectedImage);
    qDebug() << "Removing" << fileToRemove.fileName();
    fileToRemove.remove();
    fileToRemove.setFileName(fileToRemove.fileName() + ".temp");
    qDebug() << "Removing" << fileToRemove.fileName();
    fileToRemove.remove();

    setImageFileName(saveDir + "/" + selectedImage + ".temp");

    if (!imageFile.open(QFile::WriteOnly | QFile::Truncate)) {
        downloadProgressBarText(tr("Failed to open file for writing!"));
        reset();
        return;
    }

    imageHash.reset();
    settings.setValue("preferred/savedir", saveDir);
    savePreferredImage(selectedImage);

    manager->get(url);
    speedTime.start();
    averageSpeed = new MovingAverage(50);

    ui->downloadButton->setText(tr("Cance&l"));
}

void Creator::getImageFileNameFromUser()
{
    //reset();
    //ui->fileNameLabel->setText("Loading file...");

    QString loadDir = settings.value("preferred/savedir", getDefaultSaveDir()).toString();
    // load from previous folder if exist
    if (ui->fileNameLabel->text().isFilled()) {
        QDir curDir = QFileInfo(ui->fileNameLabel->text()).absoluteDir();
        qDebug() << "curDir" << curDir;

        if (QDir(curDir.absolutePath()).exists())
            loadDir = curDir.absolutePath();
        else
            loadDir = settings.value("preferred/savedir", getDefaultSaveDir()).toString();

        qDebug() << "loadDir" << loadDir;
    }

    QString filename = QFileDialog::getOpenFileName(this,
                        tr("Open image file"),
                        loadDir,
                        tr("Compressed gz image (*img.gz);;Compressed zip image (*img.zip);;Uncompressed image (*.img);;All files (*.*)"));

    if (filename.isEmpty()) {
        //ui->fileNameLabel->setText("");
        return;
    }

    downloadProgressBarText();
    setImageFileName(filename);

    int idx = ui->removableDevicesComboBox->currentIndex();
    QString destination = ui->removableDevicesComboBox->itemData(idx).toString();
    if (destination.isNull() == false && ui->fileNameLabel->text().isFilled())
        ui->writeFlashButton->setEnabled(true);
    else
        ui->writeFlashButton->setEnabled(false);

    // hide selected project and image name
    ui->projectSelectBox->blockSignals(true);
    ui->imageSelectBox->blockSignals(true);

    ui->projectSelectBox->setCurrentIndex(-1);
    ui->imageSelectBox->setCurrentIndex(-1);

    ui->projectSelectBox->blockSignals(false);
    ui->imageSelectBox->blockSignals(false);

    // and disable download button
    ui->downloadButton->setEnabled(false);
}

void Creator::writeFlashButtonClicked()
{
    if (showRootMessageBox() == true)
      return;

    if (state == STATE_WRITING_IMAGE) {
        state = STATE_IDLE;
        // cancel flashing
        privileges.SetUser();
        resetProgressBars();
        flashProgressBarText(tr("Writing canceled."));
        reset();
        diskWriter->cancelWrite();
        return;
    }

    disableControls(DISABLE_CONTROL_WRITE);

    int idx = ui->removableDevicesComboBox->currentIndex();
    QString destination = ui->removableDevicesComboBox->itemData(idx).toString();

    if (destination.isNull()) {
        qDebug() << "destination is not set";
        reset();
        return;
    }

    // check that both data points to same device (just in case)
    QString destinationText = ui->removableDevicesComboBox->itemText(idx);
#ifndef Q_OS_LINUX
    if (destinationText.startsWith(destination) == false) {
        qDebug() << "destination critical error";
        reset();
        return;
    }
#endif

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Confirm write"));
    msgBox.setText(tr("Selected device: %1\n"
                      "Are you sure you want to write the image?\n\n"
                      "Your USB-SD device will be wiped!").arg(destinationText));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Yes) {
        reset();
        return;
    }

    // unmount partitions (on Linux only)
    privileges.SetRoot();    // root need for opening a device
    bool unmounted = devEnumerator->unmountDevicePartitions(destination);
    qint64 deviceSize = devEnumerator->getSizeOfDevice(destination);
    privileges.SetUser();    // back to user
    if (unmounted == false) {
        flashProgressBarText(tr("Cannot unmount partititons on device %1").arg(destinationText));
        reset();
        return;
    }

    uncompressedImageSize = getUncompressedImageSize();
    ui->flashProgressBar->setValue(0);
    ui->flashProgressBar->setMaximum(uncompressedImageSize);

    qDebug() << "deviceSize" << deviceSize << "uncompressedImageSize" << uncompressedImageSize;
    if (uncompressedImageSize > deviceSize) {
        QString uncompressedSizeStr = devEnumerator->sizeToHuman(uncompressedImageSize);
        QString deviceSizeStr = devEnumerator->sizeToHuman(deviceSize);
        flashProgressBarText(tr("Not enough space on %1 [%2 < %3]").arg(destinationText).arg(deviceSizeStr).arg(uncompressedSizeStr));
        reset();
        return;
    }

    // DiskWriter will re-open the image file.
    if (imageFile.isOpen())
        imageFile.close();

    state = STATE_WRITING_IMAGE;
    privileges.SetRoot();    // root need for opening a device

    ui->writeFlashButton->setText(tr("Cance&l"));
    emit proceedToWriteImageToDevice(imageFile.fileName(), destination, destinationText);

    speedTime.start();
    averageSpeed = new MovingAverage(20);
    bytesLast = 0;
}

void Creator::writingSyncing()
{
    qDebug() << "writingSyncing";
    if (state == STATE_WRITING_IMAGE)
        flashProgressBarText(tr("Syncing file system..."));
}

void Creator::writingFinished()
{
    qDebug() << "writingFinished";

    /* if error happened leave it visible */
    if (state != STATE_IDLE) {
        privileges.SetUser();    // back to user
        reset();
        resetProgressBars();
        flashProgressBarText(tr("Writing done!"));
        delete averageSpeed;
        state = STATE_IDLE;
    }

    QApplication::beep();
    QApplication::alert(this, 5000);
    refreshRemovablesList();
}

void Creator::writingError(QString message)
{
    qDebug() << "Writing error:" << message;
    privileges.SetUser();    // back to user
    reset(tr("Error: %1").arg(message));
    delete averageSpeed;
    state = STATE_IDLE;

    QApplication::beep();
    QApplication::alert(this, 5000);
}

void Creator::refreshRemovablesList()
{
    // timer is always running but don't enumerate when writing image
    if (state == STATE_WRITING_IMAGE)
        return;

    //qDebug() << "Refreshing removable devices list";
    QVariant previouslySelectedDevice;
    previouslySelectedDevice = settings.value("preferred/removableDevice");

#ifndef Q_OS_MACOS
    privileges.SetRoot();    // root need for opening a device
#endif
    QStringList devNames = devEnumerator->getRemovableDeviceNames();
    QStringList friendlyNames = devEnumerator->getUserFriendlyNames(devNames);
#ifndef Q_OS_MACOS
    privileges.SetUser();    // back to user
#endif

    // check for changes
    if (devNames.size() == ui->removableDevicesComboBox->count()) {
        // same number, check values too
        bool sameDevices = true;
        for (int i = 0; i < devNames.size(); i++) {
            if (friendlyNames[i].compare(ui->removableDevicesComboBox->itemText(i)) != 0 ||
                devNames[i].compare(ui->removableDevicesComboBox->itemData(i).toString()) != 0) {
                sameDevices = false;
                break;
            }
        }   // for

        if (sameDevices)
            return;
    }

    // disable saving settings
    ui->removableDevicesComboBox->blockSignals(true);
    ui->removableDevicesComboBox->clear();

    for (int i = 0; i < devNames.size(); i++) {
        // add only real drives (not empty readers)
        if (friendlyNames[i].compare(devNames[i]) != 0)
            ui->removableDevicesComboBox->addItem(friendlyNames[i], devNames[i]);
    }

    int idx = ui->removableDevicesComboBox->findData(previouslySelectedDevice,
                                                     Qt::UserRole,
                                                     Qt::MatchFixedString);
    if (idx >= 0)
        ui->removableDevicesComboBox->setCurrentIndex(idx);
    else
        ui->removableDevicesComboBox->setCurrentIndex(0);  // first one

    // enable saving settings
    ui->removableDevicesComboBox->blockSignals(false);

    idx = ui->removableDevicesComboBox->currentIndex();
    QString destination = ui->removableDevicesComboBox->itemData(idx).toString();
    if (destination.isNull() == false && ui->fileNameLabel->text().isFilled())
        ui->writeFlashButton->setEnabled(true);
    else
        ui->writeFlashButton->setEnabled(false);
}

void Creator::handleWriteProgress(int written)
{
    if (state != STATE_WRITING_IMAGE)
        return;

    int elapsedTime = speedTime.elapsed();
    if (elapsedTime < 100)
        return;  // at least 100 msec interval

    ui->flashProgressBar->setValue(written);

    // calculate current write speed
    double speed = (written - bytesLast) * 1000.0 / elapsedTime;
    averageSpeed->AddValue(speed);
    speed = averageSpeed->AverageValue();

    double remainingTime = (uncompressedImageSize - written) / speed;  // in seconds
    bytesLast = written;

    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    QString speedText = QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit);
    QString timeText = QString::number(remainingTime, 'f', 0);
    int percentage = ((double) written) / uncompressedImageSize * 100;

    flashProgressBarText(tr("%1 seconds remaining - %2% at %3").arg(timeText, QString::number(percentage), speedText));

    speedTime.restart();   // start again to get current speed
}
