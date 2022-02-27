// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef PRIVILEGES_H
#define PRIVILEGES_H

class Privileges {
public:
    Privileges() {}
    void SetRoot() {}
    void SetUser() {}
    void SetRealUser() {}
    void Whoami() {}
    void SaveUserEnv(pid_t) {}
    QString GetUserEnvDbusSession()
    {
        return QString();
    }
    QString GetUserEnvLogname()
    {
        return QString();
    }
};

#endif // PRIVILEGES_H
