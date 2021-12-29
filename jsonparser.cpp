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

#include <QDebug>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStandardPaths>
#include <QCollator>
#include <QRegularExpression>
#include <QVersionNumber>
#include <algorithm>

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

bool compareVersion(const QVariantMap &imageMap1, const QVariantMap &imageMap2)
{
    QString versionStr1;
    QString versionStr2;

    // must compare only version not whole string
    // name-8.0.2.img.gz < name-8.0.2.1.img.gz
    // LibreELEC-WeTek_Hub.aarch64-8.0.2.1.img.gz
    // LibreELEC-A64.arm-9.95.5-pine64-plus.img.gz
    QRegularExpression regExp = QRegularExpression("-([0-9]+\\.[0-9]+\\.[0-9]+).*\\.img\\.gz");

    QRegularExpressionMatch match = regExp.match(imageMap1["name"].toString());
    if (match.hasMatch())
        versionStr1 = match.captured(1);

    QRegularExpressionMatch match2 = regExp.match(imageMap2["name"].toString());
    if (match2.hasMatch())
        versionStr2 = match2.captured(1);

    if (versionStr1.isEmpty() || versionStr2.isEmpty())
        return false; // some error

    int versionCmp = QVersionNumber::compare(
              QVersionNumber::fromString(versionStr1),
              QVersionNumber::fromString(versionStr2));

    if (versionCmp < 0)
        return true;
    else
        return false;
}

JsonParser::JsonParser(const QByteArray &data)
{
    parseAndSet(data, "");
}

void JsonParser::addExtra(const QByteArray &data, const QString label)
{
    parseAndSet(data, label);
}

namespace {

bool ReadImageName(const QJsonObject& imageObject, int projectIndex, QList<QVariantMap>& imagesList, QList<ProjectData>& dataList)
{
    QVariantMap imageProps = imageObject.toVariantMap();
    QString imageName = imageProps["name"].toString();

    if (imageName.endsWith(".img.gz"))
    {
        // QTextStream& textStream = qStdOut();
        // textStream << "Image: " << imageName << " - " << imageProps["sha256"].toString() << " - " << imageProps["size"].toString() << "\n";
        // textStream.flush();

        //We need to add the full map to the list as we'll need the name, sha256 and the size.
        if (projectIndex < 0) // new project
            imagesList.append(std::move(imageProps));
        else // existing project
            dataList[projectIndex].images.append(std::move(imageProps));

        return true;
    }

    return false;
}

} //anonymous namespace

void JsonParser::parseAndSet(const QByteArray &data, const QString label)
{
    //qDebug() << "parseAndSet data:" << data;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonDocument.object();

    int imageCount = 0;

    // get project versions (LibreElec 7.0, 8.0, ...)
    for (auto itProjectVersions = jsonObject.begin(); itProjectVersions != jsonObject.end(); itProjectVersions++)
    {
        QString project = itProjectVersions.key();
        QString projectUrl = itProjectVersions.value().toObject()["url"].toString();

        // get projects (imx6, Wetek, ...)
        QJsonObject projectVersionsNode = itProjectVersions.value().toObject()["project"].toObject();
        for (auto itProjects = projectVersionsNode.begin(); itProjects != projectVersionsNode.end(); itProjects++)
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
            QJsonObject releasesNode = itProjects.value().toObject();
            for (auto itReleases = releasesNode.begin(); itReleases != releasesNode.end(); itReleases++)
            {
                QList<QVariantMap> imagesList;
                ProjectData projectCheck;
                projectCheck.name = projectName;
                int projectIndex = dataList.indexOf(projectCheck);

                QJsonObject releaseNode = itReleases.value().toObject();
                for (auto itReleaseItems = releaseNode.begin(); itReleaseItems != releaseNode.end(); itReleaseItems++)
                {
                    QJsonObject releaseItemsNode = itReleaseItems.value().toObject();

                    QJsonObject::Iterator itFile = releaseItemsNode.find("file");
                    if (ReadImageName(itFile.value().toObject(), projectIndex, imagesList, dataList))
                        imageCount++;

                    QJsonObject::Iterator itImage = releaseItemsNode.find("image");
                    if (ReadImageName(itImage.value().toObject(), projectIndex, imagesList, dataList))
                        imageCount++;

                    QJsonObject::Iterator itUbootsNode = releaseItemsNode.find("uboot");
                    QJsonArray ubootsNode = itUbootsNode.value().toArray();
                    for (QJsonValue uboot : ubootsNode)
                    {
                        if (ReadImageName(uboot.toObject(), projectIndex, imagesList, dataList))
                            imageCount++;
                    }
                }

                if (projectIndex < 0)
                {
                  // new project
                  ProjectData projectData(projectName, projectId, projectUrl, imagesList);
                  dataList.append(projectData);
                }
            }
        }
    }

    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseSensitive);

    std::sort(dataList.begin(), dataList.end(),
              [&collator](const ProjectData &proj1, const ProjectData &proj2)
         {return collator.compare(proj1.name, proj2.name) > 0;});

    for (auto& project : dataList)
    {
        auto& images = project.images;
        std::sort(images.begin(), images.end(), compareVersion);
        std::reverse(images.begin(), images.end());
    }
}

QList<ProjectData> JsonParser::getProjectData() const
{
    return dataList;
}
