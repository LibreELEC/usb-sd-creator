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

void Translator::fillLanguages(QComboBox *box)
{
    langBox = box;

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
        lang.replace("British English", "English");   // nicer
        lang.replace("American English", "English");
        langBox->addItem(icon, lang, locale);
    }

    connect(langBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(langBoxChanged(int)));

    int idx = settings->value("preferred/lang", "-1").toInt();
    if (idx == -1 || idx > langBox->count()) {
        // not saved yet or error, try system locale
        QString locale = QLocale::system().name();

        // check for file in resources and on disk
        if (QFile::exists(":/lang/lang-" + locale + ".qm") == false ||
            QFile::exists("lang-" + locale + ".qm") == false)
                locale = "en_GB";   // default locale

        idx = langBox->findData(locale, Qt::UserRole, Qt::MatchFixedString);
    }

    langBox->setCurrentIndex(idx);
    langBoxChanged(idx);
}

void Translator::langBoxChanged(int idx)
{
    settings->setValue("preferred/lang", idx);
    QString locale = langBox->itemData(idx).toString();

    if (qtranslator->isFilled())
        qApp->removeTranslator(qtranslator);

    if (QFile::exists(":/lang/lang-" + locale + ".qm"))
        qtranslator->load(":/lang/lang-" + locale + ".qm");
    else
        qtranslator->load("lang-" + locale + ".qm");

    if (qtranslator->isFilled())
        qApp->installTranslator(qtranslator);
}
