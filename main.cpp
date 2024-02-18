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

#include "creator.h"
#include "version.h"

#ifdef Q_OS_MAC
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
