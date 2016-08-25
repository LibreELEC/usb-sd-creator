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

    foreach(const QString &qmFile, QDir(":/lang").entryList(QStringList("*.qm"))) {
        QRegExp regExp = QRegExp("lang-(.*)\\.qm");
        regExp.indexIn(qmFile);
        QString locale = regExp.capturedTexts().at(1);
        QIcon icon(":/lang/flag-" + locale + ".png");
        QString lang = QLocale(locale).nativeLanguageName();
        lang.replace("British English", "English");   // nicer
        langBox->addItem(icon, lang, locale);
    }

    connect(langBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(langBoxChanged(int)));

    int idx = settings->value("preferred/lang", "-1").toInt();
    if (idx == -1 || idx > langBox->count()) {
        // not saved yet or error, try system locale
        QString locale = QLocale::system().name();

        if (QFile::exists(":/lang/lang-" + locale + ".qm") == false)
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

    if (qtranslator->isEmpty() == false)
        qApp->removeTranslator(qtranslator);

    qtranslator->load(":/lang/lang-" + locale + ".qm");
    qApp->installTranslator(qtranslator);
}
