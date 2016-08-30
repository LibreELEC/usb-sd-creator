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
#include <QProxyStyle>
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
    if (! app.arguments().contains("--debug"))
        qInstallMessageHandler(noMessageOutput);
#endif

    qDebug() << "App data: Version:" << BUILD_VERSION ", Build date: " BUILD_DATE;

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

    privileges.SetUser();
    privileges.Whoami();

    Creator win(privileges, 0);
    win.setArgFile(argFile);
    win.show();

    return app.exec();
}
