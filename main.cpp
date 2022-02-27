// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2013-2015 RasPlex project
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#include "creator.h"
#include "version.h"

#ifdef Q_OS_UNIX
#include "privileges_unix.h"
#else
#include "privileges.h"
#endif

#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>
#include <QProcess>
#include <QProxyStyle>
#include <QNetworkProxy>
#include <QDebug>

// show debug output always
//#define ALWAYS_DEBUG_OUTPUT

#ifdef Q_OS_MAC
class MacFontStyle : public QProxyStyle
{
protected:
    void polish(QWidget *w)
    {
        //QMenu* mn = dynamic_cast<QMenu*>(w);
        //if (!mn && !w->testAttribute(Qt::WA_MacNormalSize))
        if (!w->testAttribute(Qt::WA_MacNormalSize))
            w->setAttribute(Qt::WA_MacSmallSize);
    }
};
#endif

void noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    Q_UNUSED(msg);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString argFile = "";

#ifdef Q_OS_MAC
    // prevents the font size from appearing overly large on OSX
    app.setStyle(new MacFontStyle);
#endif

#ifndef ALWAYS_DEBUG_OUTPUT
    if (app.arguments().contains("--debug") == false)
        qInstallMessageHandler(noMessageOutput);
#endif

#ifdef Q_OS_MACOS
    // If not running with root privileges, relaunch executable with sudo.
    if (getuid() != 0 && app.arguments().contains("--elevated") == false)
    {
        QString askPassCommand = QCoreApplication::applicationDirPath() + "/askPass.js";

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("SUDO_ASKPASS", askPassCommand);

        QProcess myProcess;
        myProcess.setProcessEnvironment(env);
        myProcess.setProgram("sudo");
        myProcess.setArguments(QStringList()
            << "-A"
            << QCoreApplication::applicationFilePath()
            << "--elevated");
        bool success = myProcess.startDetached();

        if (success)
        {
            return 0;
        }
        else
        {
            qDebug() << "Unable to start elevated process for " << QCoreApplication::applicationFilePath();
        }
    }
#endif

    qDebug() << "App data: Version:" << BUILD_VERSION ", Build date: " BUILD_DATE;

    if (app.arguments().contains("--no-proxy") == false) {
        QNetworkProxyQuery npq(QUrl("http://releases.libreelec.tv/"));
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
        if (listOfProxies.size()) {
            QNetworkProxy::setApplicationProxy(listOfProxies[0]);
            qDebug() << "Using" << listOfProxies[0];
        }
    }

    Privileges privileges = Privileges();
    privileges.Whoami();

    // skip program filename
    for (int i=1; i<app.arguments().size(); i++) {
        QString file = app.arguments().at(i);
        QFileInfo checkFile(file);

        if (checkFile.exists() && checkFile.isFile()) {
            argFile = file;
            break;
        }
    }

#ifndef Q_OS_MACOS
    privileges.SetUser();
#endif
    privileges.Whoami();

    Creator win(privileges, 0);
    win.setArgFile(argFile);
    win.show();

    return app.exec();
}
