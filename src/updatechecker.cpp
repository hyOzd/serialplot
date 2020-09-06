/*
  Copyright © 2020 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QRegularExpression>
#include <algorithm>
#include <functional>

#include "updatechecker.h"

const char UPDATES_INFO_URL[] = "https://serialplot.ozderya.net/downloads/updates.json";

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent), nam(this)
{
    activeReply = NULL;

    connect(&nam, &QNetworkAccessManager::finished,
            this, &UpdateChecker::onReqFinished);
}

bool UpdateChecker::isChecking() const
{
    return activeReply != NULL && !activeReply->isFinished();
}

void UpdateChecker::checkUpdate()
{
    if (isChecking()) return;

    auto req = QNetworkRequest(QUrl(UPDATES_INFO_URL));
    activeReply = nam.get(req);
}

void UpdateChecker::cancelCheck()
{
    if (activeReply != NULL) activeReply->abort();
}

void UpdateChecker::onReqFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        emit checkFailed(QString("Network error: ") + reply->errorString());
    }
    else
    {
        QJsonParseError error;
        auto data = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError)
        {
            emit checkFailed(QString("JSon parsing error: ") + error.errorString());
        }
        else
        {
            bool updateFound;
            VersionNumber newVersion;
            QString link;

            if (!findUpdate(data, updateFound, newVersion, link))
            {
                emit checkFailed("Data parsing error.");
                qCritical() << "Parsing the update info file failed:";
                qCritical() << data;
            }
            else
            {
                if (updateFound)
                {
                    emit checkFinished(
                        true, newVersion.toString(), link);
                }
                else
                {
                    emit checkFinished(false, "", "");
                }
            }
        }
    }
    reply->deleteLater();
    activeReply = NULL;
}

bool UpdateChecker::findUpdate(const QJsonDocument& data, bool& foundUpdate,
                               VersionNumber& foundVersion, QString& fileLink) const
{
    /* Data is expected to be in this format:
----
{
  "latest" : {
    "linux" : "0.11",
    "windows" : "0.11",
  },
  "releases" : {
    "0.11" : {
      "files" : {
        "linux" : "https://serialplot.ozderya.net/downloads/serialplot_v0.11.appimage",
        "windows" : "https://serialplot.ozderya.net/downloads/serialplot_v0.11.exe",
      }
    }
  }
}
----
    */

    // release type to look
#if defined(Q_OS_WIN)

#if defined(Q_PROCESSOR_X86_64)
    const char release_type[] = "windows";
#elif defined(Q_PROCESSOR_X86_32)
    const char release_type[] = "windows_32";
#else
#error Unknown architecture for update file detection.
#endif

#else  // assume linux

#if defined(Q_PROCESSOR_X86_64)
    const char release_type[] = "linux";
#elif defined(Q_PROCESSOR_X86_32)
    const char release_type[] = "linux_32";
#elif defined(Q_PROCESSOR_ARM)
    const char release_type[] = "arm";
#else
#error Unknown architecture for update file detection.
#endif

#endif

    foundUpdate = false;

    if (!data.isObject())
    {
        qCritical("JSON data invalid.");
        return false;
    }

    // find latest version for this release type
    auto latest = data.object().value("latest");
    if (latest == QJsonValue::Undefined || !latest.isObject())
    {
        qCritical("JSON data \"latest\" field is missing.");
        return false;
    }

    auto latest_rel = latest.toObject().value(release_type);
    if (latest_rel == QJsonValue::Undefined || !latest_rel.isString())
    {
        qDebug() << "No update found for " << release_type;
        return true;
    }

    VersionNumber latest_vn;
    if (!VersionNumber::extract(latest_rel.toString(), latest_vn))
    {
        qCritical() << "Failed to parse version number: " << latest_vn.toString();
        return false;
    }

    if (!(latest_vn > CurrentVersion))
    {
        qDebug() << "No update.";
        return true;
    }

    // find the file link
    auto link = data.object().value("releases").toObject()[latest_rel.toString()]\
                .toObject().value("files").toObject().value(release_type);
    if (link == QJsonValue::Undefined || !link.isString())
    {
        qCritical() << "Link not found!";
        return false;
    }

    fileLink = link.toString();
    foundVersion = latest_vn;
    qDebug() << "New update:" << latest_vn.toString() << fileLink;
    foundUpdate = true;
    return true;
}
