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

bool ReadImageName(const QJsonObject& imageObject, int projectIndex, QList<QVariantMap>& imagesList, QList<ProjectData>& projectList)
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
            projectList[projectIndex].images.append(std::move(imageProps));

        return true;
    }

    return false;
}

} //anonymous namespace

void JsonParser::parseAndSet(const QByteArray &data, const QString label)
{
    //qDebug() << "parseAndSet data:" << data;
    const QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    const auto jsonObject = jsonDocument.object();

    // get project versions (LibreElec 7.0, 8.0, ...)
    for (auto itProjectVersions = jsonObject.begin(); itProjectVersions != jsonObject.end(); ++itProjectVersions)
    {
        const auto project = itProjectVersions.key();
        const auto projectObj = itProjectVersions.value().toObject();
        const auto projectUrl = projectObj["url"].toString();

        // get projects (imx6, Wetek, ...)
        const auto projectVersionsNode = projectObj["project"].toObject();
        for (auto itProjects = projectVersionsNode.begin(); itProjects != projectVersionsNode.end(); ++itProjects)
        {
            const auto projectId = itProjects.key();
            const auto releasesNode = itProjects.value().toObject();
            auto projectName = releasesNode["displayName"].toString();

            // skip Virtual
            if (projectId == "Virtual.x86_64")
                continue;

            if (!label.isEmpty())
                projectName += QString{" - %1"}.arg(label);

            QVariantMap projectData;
            projectData.insert("name", projectName);
            projectData.insert("id", projectId);
            projectData.insert("url", projectUrl);

            // get releases
            for (auto itReleases = releasesNode.begin(); itReleases != releasesNode.end(); ++itReleases)
            {
                QList<QVariantMap> imagesList;
                ProjectData projectCheck;
                projectCheck.name = projectName;
                int projectIndex = projectList.indexOf(projectCheck);

                const auto releaseNode = itReleases.value().toObject();
                for (auto itReleaseItems = releaseNode.begin(); itReleaseItems != releaseNode.end(); ++itReleaseItems)
                {
                    const auto releaseItemsNode = itReleaseItems.value().toObject();

                    const auto itFile = releaseItemsNode.find("file");
                    if (itFile != releaseItemsNode.end())
                        ReadImageName(itFile.value().toObject(), projectIndex, imagesList, projectList);

                    const auto itImage = releaseItemsNode.find("image");
                    if (itImage != releaseItemsNode.end())
                        ReadImageName(itImage.value().toObject(), projectIndex, imagesList, projectList);

                    const auto itUbootsNode = releaseItemsNode.find("uboot");
                    if (itUbootsNode != releaseItemsNode.end())
                    {
                        QJsonArray ubootsNode = itUbootsNode.value().toArray();
                        for (QJsonValue uboot : ubootsNode)
                        {
                            ReadImageName(uboot.toObject(), projectIndex, imagesList, projectList);
                        }
                    }
                }

                if (projectIndex < 0)
                {
                  // new project
                  ProjectData projectData(projectName, projectId, projectUrl, imagesList);
                  projectList.append(projectData);
                }
            }
        }
    }

    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseSensitive);

    std::sort(projectList.begin(), projectList.end(),
              [&collator](const ProjectData &proj1, const ProjectData &proj2)
         {return collator.compare(proj1.name, proj2.name) > 0;});

    for (auto& project : projectList)
    {
        auto& images = project.images;
        std::sort(images.begin(), images.end(), compareVersion);
        std::reverse(images.begin(), images.end());
    }
}

QList<ProjectData> JsonParser::getProjectData() const
{
    return projectList;
}
