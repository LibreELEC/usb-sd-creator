////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
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

#include "translator.h"

#include <QDebug>
#include <QFile>
#include <QDir>

Translator::Translator(QObject *parent, QSettings *set) :
    QObject(parent),
    settings(set)
{
    qtranslator = new QTranslator();
}

Translator::~Translator()
{
    delete qtranslator;
}

void Translator::fillLanguages(QMenu *menuPtr, QPushButton *langBtnPtr)
{
    menu = menuPtr;
    langBtn = langBtnPtr;

    QStringList qmFiles;
    // languages from resources
    qmFiles << QDir(":/lang").entryList(QStringList("*.qm"));
    // languages from a local disk (mostly for testing purposes)
    qmFiles << QDir(".").entryList(QStringList("*.qm"));

    // add menu entry for all the files
    foreach (const QString &qmFile, qmFiles) {
        QRegExp regExp = QRegExp("lang-(.*)\\.qm");
        regExp.indexIn(qmFile);
        QString locale = regExp.capturedTexts().at(1);

        QIcon icon;
        QString iconName = "flag-" + locale + ".png";
        if (QFile::exists(":/lang/" + iconName))
            icon = QIcon(":/lang/" + iconName);
        else if (QFile::exists(iconName))
            icon = QIcon(iconName);
        else
            icon = QIcon(":/lang/flag-empty.png");

        QString lang = QLocale(locale).nativeLanguageName();
        lang = lang.left(1).toUpper() + lang.mid(1);  // capitalize first letter

        lang.replace("British English", "English");   // nicer
        lang.replace("American English", "English");

        QAction *action = new QAction(lang, menu);
        action->setIcon(icon);
        action->setData(locale);
        menu->addAction(action);
    }

    connect(menu, SIGNAL(triggered(QAction*)), SLOT(languageAction(QAction*)));

    QString locale = settings->value("preferred/lang").toString();

    // set first time locale from the system
    if (locale.isEmpty())
        locale = QLocale::system().name();

    // check for file in resources and on disk
    if (QFile::exists(":/lang/lang-" + locale + ".qm") == false &&
        QFile::exists("lang-" + locale + ".qm") == false)
            locale = "en_GB";   // default locale

    for (int i=0; i<menu->actions().count(); i++) {
        if (locale == menu->actions().at(i)->data()) {
            langBtn->setIcon(menu->actions().at(i)->icon());
            languageAction(menu->actions().at(i));
            break;
        }
    }
}

void Translator::languageAction(QAction *action)
{
    QString locale = action->data().toString();
    settings->setValue("preferred/lang", locale);

    langBtn->setIcon(action->icon());

    if (qtranslator->isFilled())
        qApp->removeTranslator(qtranslator);

    if (QFile::exists(":/lang/lang-" + locale + ".qm"))
        qtranslator->load(":/lang/lang-" + locale + ".qm");
    else
        qtranslator->load("lang-" + locale + ".qm");

    if (qtranslator->isFilled())
        qApp->installTranslator(qtranslator);

}
