////////////////////////////////////////////////////////////////////////////////
//      This file is part of LibreELEC - http://www.libreelec.tv
//      Copyright (C) 2024 Team LibreELEC
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

#include "askpass.h"

#include <QApplication>
#include <QInputDialog>

#include <iostream>

namespace sudo
{
void askpass()
{
    const auto passwordPrompt = QObject::tr("%1 requires admin permissions.\n\nPlease enter your password to allow this.",
                                            "arg is app name");

    auto getAdminPasswordDlg = new QInputDialog;
    getAdminPasswordDlg->setAttribute(Qt::WA_DeleteOnClose);
    getAdminPasswordDlg->setInputMode(QInputDialog::TextInput);
    getAdminPasswordDlg->setTextEchoMode(QLineEdit::Password);
    getAdminPasswordDlg->setLabelText(passwordPrompt.arg(qApp->applicationDisplayName()));
    getAdminPasswordDlg->show();

    QObject::connect(getAdminPasswordDlg, &QInputDialog::accepted, [=]{
        const auto enteredPassword = getAdminPasswordDlg->textValue();
        std::cout << qUtf8Printable(enteredPassword);
        std::cout.flush();
        qApp->quit();
    });
}
}
