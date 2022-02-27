// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef PRIVILEGES_H
#define PRIVILEGES_H

#include <pwd.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <grp.h>

#include <QFile>
#include <QDebug>

class Privileges
{
public:
    Privileges();
    void SetRoot();
    void SetUser();
    void SetRealUser();
    void Whoami();
    void SaveUserEnv(pid_t);
    QString GetUserEnvDbusSession() {
        return envDbus;
    }
    QString GetUserEnvLogname() {
        return envLogname;
    }

private:
    uid_t userUidFromPid(pid_t pid, pid_t *ppid);

    QString envDbus;
    QString envLogname;
    uid_t orig_uid;
    gid_t orig_gid;
    int   orig_ngroups;
    gid_t orig_groups[NGROUPS_MAX];
    uid_t new_uid;
    gid_t new_gid;

    enum {
        USER_ROOT,
        USER_NORMAL
    } origUser;  // which user selected
};

#endif // PRIVILEGES_H
