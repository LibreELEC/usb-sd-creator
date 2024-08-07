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

#ifdef Q_OS_MACOS
#include "privileges_unix.h"
#include "macos/askpass.h"

#include <QLatin1String>
#include <QProcess>
#else
#include "privileges.h"
#endif

#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>
#include <QProxyStyle>
#include <QNetworkProxy>
#include <QDebug>

#ifndef ALWAYS_DEBUG_OUTPUT
void noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    Q_UNUSED(msg);
}
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WINDOWS
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");
#endif
    QApplication app(argc, argv);

    const auto cmdArgs = app.arguments();

#ifndef ALWAYS_DEBUG_OUTPUT
    if (cmdArgs.contains("--debug") == false)
        qInstallMessageHandler(noMessageOutput);
#endif

#ifdef Q_OS_MACOS
    // special GUI when executing as a "get admin password" program
    if (qEnvironmentVariableIsSet(sudo::AskPassEnvVar.data()))
    {
        sudo::askpass();
        return app.exec();
    }

    // If not running with root privileges, relaunch executable with sudo.
    const QLatin1String elevatedParam{"--elevated"};
    if (!cmdArgs.contains(elevatedParam) && getuid() != 0)
    {
        const auto executablePath = QCoreApplication::applicationFilePath();

        // Apple Script "do shell script ... with administrator privileges" can't be used here:
        // GUI app launched like that doesn't have all GUI capabilities, like accepting file drops
        QProcess sudoProc;
        sudoProc.setProgram(QLatin1String{"sudo"});
        sudoProc.setArguments(QStringList{QLatin1String{"--askpass"}, executablePath, elevatedParam} + cmdArgs.mid(1));

        QProcessEnvironment sudoEnv;
        sudoEnv.insert(sudo::AskPassEnvVar, executablePath);
        sudoProc.setProcessEnvironment(sudoEnv);

        if (sudoProc.startDetached())
        {
            return 0;
        }
        else
        {
            qDebug() << "Unable to start elevated process for " << executablePath;
        }
    }
#endif

    qDebug() << "App data: Version:" << BUILD_VERSION << ", Build date:" << BUILD_DATE;

    const QLatin1String httpProxyParam{"--http-proxy"};
    const auto httpProxyParamIndex = cmdArgs.indexOf(httpProxyParam);
    if (httpProxyParamIndex != -1)
    {
        const auto httpProxy = cmdArgs.at(httpProxyParamIndex + 1);
        const auto proxyHostPort = httpProxy.split(':');

        QNetworkProxy proxy{QNetworkProxy::HttpProxy, proxyHostPort[0]};
        if (proxyHostPort.size() > 1)
            proxy.setPort(proxyHostPort[1].toUShort());
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if (!cmdArgs.contains("--no-proxy")) {
        QNetworkProxyQuery npq(QUrl("http://releases.libreelec.tv/"));
        QList<QNetworkProxy> listOfProxies = QNetworkProxyFactory::systemProxyForQuery(npq);
        if (listOfProxies.size()) {
            QNetworkProxy::setApplicationProxy(listOfProxies[0]);
            qDebug() << "Using" << listOfProxies[0];
        }
    }

    Privileges privileges;
    privileges.Whoami();

    QString argFile;
    // skip program filename
    for (int i = 1; i < cmdArgs.size(); i++) {
        QString file = cmdArgs.at(i);
        QFileInfo checkFile(file);

        if (checkFile.exists() && checkFile.isFile()) {
            argFile = file;
            break;
        }
    }

#ifndef Q_OS_MACOS
    privileges.SetUser();
    privileges.Whoami();
#endif

    Creator win(privileges);
    win.setArgFile(argFile);
    win.show();

    return app.exec();
}
