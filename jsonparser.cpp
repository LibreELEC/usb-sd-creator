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

#include "jsonparser.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStandardPaths>
#include <QCollator>
#include <QVersionNumber>
#include <algorithm>

bool compareVersion(const QVariantMap &imageMap1, const QVariantMap &imageMap2)
{
    // must compare only version not whole string
    // name-8.0.2.img.gz < name-8.0.2.1.img.gz
    // LibreELEC-WeTek_Hub.aarch64-8.0.2.1.img.gz
    QRegExp regExp = QRegExp(".*-([0-9]+.*)\\.img\\.gz");
    regExp.indexIn(imageMap1["name"].toString());
    QStringList versionStr1 = regExp.capturedTexts();
    regExp.indexIn(imageMap2["name"].toString());
    QStringList versionStr2 = regExp.capturedTexts();

    if (versionStr1.count() != 2 || versionStr2.count() != 2)
        return false;   // some error

    int versionCmp = QVersionNumber::compare(
              QVersionNumber::fromString(versionStr1.at(1)),
              QVersionNumber::fromString(versionStr2.at(1)));

    if (versionCmp < 0)
        return false;
    else
        return true;
}

JsonParser::JsonParser(const QByteArray &data)
{
    parseAndSet(data, "");
}

void JsonParser::addExtra(const QByteArray &data, const QString label)
{
    parseAndSet(data, label);
}

void JsonParser::parseAndSet(const QByteArray &data, const QString label)
{
    //qDebug() << "parseAndSet data:" << data;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonDocument.object();

    // get project versions (7.0, 8.0, ...)
    for (QJsonObject::Iterator itProjectVersions  = jsonObject.begin();
                               itProjectVersions != jsonObject.end();
                               itProjectVersions++)
    {
        QString project = itProjectVersions.key();
        QString projectUrl = itProjectVersions.value().toObject()["url"].toString();

        // get projects (imx6, Wetek, ...)
        QJsonObject val = itProjectVersions.value().toObject()["project"].toObject();
        for (QJsonObject::Iterator itProjects  = val.begin();
                                   itProjects != val.end();
                                   itProjects++)
        {
            QString projectId = itProjects.key();
            QString projectName = itProjects.value().toObject()["displayName"].toString();

            // skip Virtual
            if (projectId == "Virtual.x86_64")
                continue;

            if (label != "")
                projectName = projectName + " - " + label;

            QVariantMap projectData;
            projectData.insert("name", projectName);
            projectData.insert("id", projectId);
            projectData.insert("url", projectUrl);

            // get releases
            QJsonObject val1 = itProjects.value().toObject();
            for (QJsonObject::Iterator itReleasesNode  = val1.begin();
                                       itReleasesNode != val1.end();
                                       itReleasesNode++)
            {
                QList<QVariantMap> images;
                JsonData projectCheck;
                projectCheck.name = projectName;
                int projectIx = dataList.indexOf(projectCheck);

                QJsonObject val2 = itReleasesNode.value().toObject();
                for (QJsonObject::Iterator itReleases  = val2.begin();
                                           itReleases != val2.end();
                                           itReleases++)
                {
                    QJsonObject projectReleasesList = itReleases.value().toObject();
                    for (QJsonObject::Iterator itImageFile  = projectReleasesList.begin();
                                               itImageFile != projectReleasesList.end();
                                               itImageFile++)
                    {
                        QString imageName = itImageFile.value().toObject().toVariantMap()["name"].toString();

                        if (! imageName.endsWith(".img.gz"))
                            continue;   // we want to see only image files

                        if (projectIx < 0) {
                            // new project
                            images.append(itImageFile.value().toObject().toVariantMap());
                        } else {
                            // old project
                            dataList[projectIx].images.append(itImageFile.value().toObject().toVariantMap());
                        }
                    }
                }


                if (projectIx < 0) {
                  // new project
                  JsonData projectData(projectName, projectId, projectUrl, images);
                  dataList.append(projectData);
                }
            }
        }
    }

    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseSensitive);

    std::sort(dataList.begin(), dataList.end(),
              [&collator](const JsonData &proj1, const JsonData &proj2)
         {return collator.compare(proj1.name, proj2.name) > 0;});

    for (int ix = 0; ix < dataList.size(); ix++)
        std::sort(dataList[ix].images.begin(), dataList[ix].images.end(), compareVersion);
}

QList<JsonData> JsonParser::getJsonData() const
{
    return dataList;
}
