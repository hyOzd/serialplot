/*
  Copyright © 2017 Hasan Yavuz Özderya

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

// This link returns the list of downloads in JSON format. Note that we only use
// the first page because results are sorted new to old.
const char BB_DOWNLOADS_URL[] = "https://api.bitbucket.org/2.0/repositories/hyozd/serialplot/downloads?fields=values.name,values.links.self.href";

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent), nam(this)
{
    connect(&nam, &QNetworkAccessManager::finished,
            this, &UpdateChecker::onReqFinished);
}

void UpdateChecker::checkUpdate()
{
    auto req = QNetworkRequest(QUrl(BB_DOWNLOADS_URL));
    nam.get(req);
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
            QList<FileInfo> files;
            if (!parseData(data, files))
            {
                // TODO: emit detailed data contents for logging
                emit checkFailed("Data parsing error.");
            }
            else
            {
                FileInfo updateFile;
                if (findUpdate(files, updateFile))
                {
                    emit checkFinished(
                        true, updateFile.version.toString(), updateFile.link);
                }
                else
                {
                    emit checkFinished(false, "", "");
                }
            }
        }
    }
    reply->deleteLater();
}

bool UpdateChecker::parseData(const QJsonDocument& data, QList<FileInfo>& files) const
{
    /* Data is expected to be in this form:

    {
       "values": [
       {
         "name": "serialplot-0.9.1-x86_64.AppImage",
         "links": {
           "self": {
             "href": "https://api.bitbucket.org/2.0/repositories/hyOzd/serialplot/downloads/serialplot-0.9.1-x86_64.AppImage"
            }
          }
       }, ... ]
    }
    */

    if (!data.isObject()) return false;

    auto values = data.object()["values"];
    if (values == QJsonValue::Undefined || !values.isArray()) return false;

    for (auto value : values.toArray())
    {
        if (!value.isObject()) return false;

        auto name = value.toObject().value("name");
        if (name.isUndefined() || !name.isString())
             return false;

        auto links = value.toObject().value("links");
        if (links.isUndefined() || !links.isObject())
            return false;

        auto self = links.toObject().value("self");
        if (self.isUndefined() || !self.isObject())
            return false;

        auto href = self.toObject().value("href");
        if (href.isUndefined() || !href.isString())
            return false;

        FileInfo finfo;
        finfo.name = name.toString();
        finfo.link = href.toString();
        finfo.hasVersion = VersionNumber::extract(name.toString(), finfo.version);

        if (finfo.name.contains("amd64") ||
            finfo.name.contains("x86_64") ||
            finfo.name.contains("win64"))
        {
            finfo.arch = FileArch::amd64;
        }
        else if (finfo.name.contains("win32") ||
                 finfo.name.contains("i386"))
        {
            finfo.arch = FileArch::i386;
        }
        else
        {
            finfo.arch = FileArch::unknown;
        }

        files += finfo;
    }

    return true;
}

bool UpdateChecker::findUpdate(const QList<FileInfo>& files, FileInfo& foundFile) const
{
    QList<FileInfo> fflist;

    // filter the file list according to extension and version number
    for (int i = 0; i < files.length(); i++)
    {
        // file type to look
#if defined(Q_OS_WIN)
        const char ext[] = ".exe";
#else  // of course linux
        const char ext[] = ".appimage";
#endif

        // file architecture to look
#if defined(Q_PROCESSOR_X86_64)
        const FileArch arch = FileArch::amd64;
#elif defined(Q_PROCESSOR_X86_32)
        const FileArch arch = FileArch::i386;
#elif defined(Q_PROCESSOR_ARM)
        const FileArch arch = FileArch::arm;
#else
        #error Unknown architecture for update file detection.
#endif

        // filter the file list
        auto file = files[i];
        if (file.name.contains(ext, Qt::CaseInsensitive) &&
            file.arch == arch &&
            file.hasVersion && file.version > CurrentVersion)
        {
            fflist += file;
        }
    }

    // sort and find most up to date file
    if (!fflist.empty())
    {
        std::sort(fflist.begin(), fflist.end(),
                  [](const FileInfo& a, const FileInfo& b)
                  {
                      return a.version > b.version;
                  });

        foundFile = fflist[0];
        return true;
    }
    else
    {
        return false;
    }
}
