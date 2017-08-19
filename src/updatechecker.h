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

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>

#include "versionnumber.h"

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);

    bool isChecking() const;

signals:
    void checkFinished(bool found, QString newVersion, QString downloadUrl);
    void checkFailed(QString errorMessage);

public slots:
    void checkUpdate();
    void cancelCheck();

private:
    enum class FileArch
    {
        unknown,
        i386,
        amd64,
        arm
    };

    struct FileInfo
    {
        QString name;
        QString link;
        bool hasVersion;
        VersionNumber version;
        FileArch arch;
    };

    QNetworkAccessManager nam;
    QNetworkReply* activeReply;

    /// Parses json and creates a list of files
    bool parseData(const QJsonDocument& data, QList<FileInfo>& files) const;
    /// Finds the update file in the file list. Returns `-1` if no new version
    /// is found.
    bool findUpdate(const QList<FileInfo>& files, FileInfo& foundFile) const;

private slots:
    void onReqFinished(QNetworkReply* reply);
};

#endif // UPDATECHECKER_H
