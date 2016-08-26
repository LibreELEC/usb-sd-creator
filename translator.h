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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QObject>
#include <QComboBox>
#include <QSettings>

// useful macro
#define isFilled()  isEmpty() == false

class Translator : public QObject
{
    Q_OBJECT

public:
    Translator(QObject *parent = 0, QSettings *set = 0);
    ~Translator();
    void fillLanguages(QComboBox *box);

signals:
    void languageChanged(const QString &language);  // standard Qt signal

private:
    QComboBox *langBox;
    QSettings *settings;
    QTranslator *qtranslator;

protected slots:
    void langBoxChanged(int idx);
};

#endif // TRANSLATOR_H
