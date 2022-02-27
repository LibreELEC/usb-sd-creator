// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QObject>
#include <QMenu>
#include <QPushButton>
#include <QSettings>

// useful macro
#define isFilled()  isEmpty() == false

class Translator : public QObject
{
    Q_OBJECT

public:
    Translator(QObject *parent = 0, QSettings *set = 0);
    ~Translator();
    void fillLanguages(QMenu *menuPtr, QPushButton *langBtnPtr);

private:
    QMenu *menu;
    QPushButton *langBtn;
    QSettings *settings;
    QTranslator *qtranslator;

protected slots:
    void languageAction(QAction *action);
};

#endif // TRANSLATOR_H
