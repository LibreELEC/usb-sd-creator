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

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCollator>
#include <QRegularExpression>
#include <algorithm>

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
    QList<QAction *> actions;
    foreach (const QString &qmFile, qmFiles) {
        QRegularExpression regExp = QRegularExpression("lang-(.*)\\.qm");
        QRegularExpressionMatch match = regExp.match(qmFile);
        QString locale = match.captured(1);

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
        QString langEn = QLocale::languageToString(QLocale(locale).language());

        // make names nicer
        lang.replace("British English", "English UK");
        lang.replace("American English", "English US");
        lang.replace("Portugu" + QString::fromUtf8("\xc3\xaa") + "s europeu", \
          "Portugu" + QString::fromUtf8("\xc3\xaa"));
        lang.replace("Espa" + QString::fromUtf8("\xc3\xb1") + "ol de Espa" + QString::fromUtf8("\xc3\xb1") + "a", \
          "Espa" + QString::fromUtf8("\xc3\xb1") + "ol");

        langEn.replace("NorwegianBokmal", "Norwegian");

        QAction *action = new QAction(langEn + " / " + lang, menu);
        action->setCheckable(true);
        action->setIcon(icon);
        action->setData(locale);

        actions << action;
    }

    // sort actions by country name
    QCollator collator;
    collator.setNumericMode(false);
    collator.setCaseSensitivity(Qt::CaseSensitive);

    std::sort(actions.begin(), actions.end(),
      [&collator](const QAction *act1, const QAction *act2)
         {return collator.compare(act1->text(), act2->text()) < 0;}
    );

    menu->addActions(actions);  // add to menu

    connect(menu, &QMenu::triggered, this, &Translator::languageAction);

    QString locale = settings->value("preferred/lang").toString();

    // set first time locale from the system
    if (locale.isEmpty() && QLocale::system().uiLanguages().count() >= 1) {
        locale = QLocale::system().uiLanguages().at(0);
        locale.replace("-", "_");
    }

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

    QApplication::setLayoutDirection(QLocale(locale).textDirection());
}

void Translator::languageAction(QAction *action)
{
    QString locale = action->data().toString();
    settings->setValue("preferred/lang", locale);

    langBtn->setIcon(action->icon());

    if (qtranslator->isFilled())
        qApp->removeTranslator(qtranslator);

    bool loaded = false;
    if (QFile::exists(":/lang/lang-" + locale + ".qm"))
        loaded = qtranslator->load(":/lang/lang-" + locale + ".qm");
    else
        loaded = qtranslator->load("lang-" + locale + ".qm");

    if (loaded && qtranslator->isFilled())
        qApp->installTranslator(qtranslator);

    // clear checked status
    for (int i=0; i<menu->actions().count(); i++)
        menu->actions().at(i)->setChecked(false);

    // set checked and tooltip for current one
    action->setChecked(true);
    langBtn->setToolTip(action->text());

    QApplication::setLayoutDirection(QLocale(locale).textDirection());
}
