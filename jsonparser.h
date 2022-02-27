// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2016-present Team LibreELEC (https://libreelec.tv)

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QDebug>

class ProjectData
{
  public:
    ProjectData() {}

    ProjectData(QString name, QString id, QString url, QList<QMap<QString, QVariant>> &images)
    {
         addData(name, id, url, images);
    }

    void addData(QString name, QString id, QString url, QList<QMap<QString, QVariant>> &images)
    {
        ProjectData::name = name;
        ProjectData::id = id;
        ProjectData::url = url;
        ProjectData::images = images;
    }

    bool operator== (const ProjectData &data) const
    {
        if (data.name == this->name)
            return true;

        return false;
    }

    QString name;
    QString id;
    QString url;
    QList<QVariantMap> images;
};

class JsonParser
{
public:
    JsonParser() {}
    JsonParser(const QByteArray &data);
    void addExtra(const QByteArray &data, const QString label);
    void parseAndSet(const QByteArray &data, const QString label);
    QList<ProjectData> getProjectData() const;

private:
    QList<ProjectData> projectList;
};

#endif // JSONPARSER_H
