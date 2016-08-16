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

#ifdef Q_OS_UNIX
#include "privileges_unix.h"
#else
#include "privileges.h"
#endif

#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDebug>

// show debug output always
//#define ALWAYS_DEBUG_OUTPUT

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
    if (! app.arguments().contains("--debug"))
        qInstallMessageHandler(noMessageOutput);
#endif

    qDebug() << "App data:" << BUILD_VERSION ", " BUILD_DATE;

    Privileges privileges = Privileges();
    privileges.Whoami();

#if 0
#ifndef Q_OS_WIN
    if (app.arguments().size() == 2 && app.arguments().at(1) == "--browser") {
        QString url = getenv("LE_URL_ADDRESS");
        qDebug() << "url" << url;

        if (! url.trimmed().isEmpty()) {
            privileges.SetRealUser();   // no need to switch back
            QDesktopServices::openUrl(QUrl(url));
        }

        qDebug() << "Exit browser instance";
        exit(0);  // process stays with 'return app.exec();'
    }
#endif
#endif

    // skip program filename
    for (int i=1; i<app.arguments().size(); i++) {
        QString file = app.arguments().at(i);
        QFileInfo checkFile(file);

        if (checkFile.exists() && checkFile.isFile()) {
            argFile = file;
            break;
        }
    }

    privileges.SetUser();
    privileges.Whoami();

    Creator win(privileges, 0);
    win.setArgFile(argFile);
    win.setWindowTitle("LibreELEC USB-SD Creator");
    win.show();

    return app.exec();
}
