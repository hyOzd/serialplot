#include "updatechecker.h"
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// This returns the list of downloads in JSON format. Note that we only use the first
// page because results are sorted in new to old.
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
    qDebug() << "finished";
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
                qDebug() << "Data parsing error.";
            }
            else
            {
                for (auto f : files)
                {
                    qDebug() << f.name << f.link;
                }
            }
        }
    }
    reply->deleteLater();
}

bool UpdateChecker::parseData(const QJsonDocument& data, QList<FileInfo>& files)
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

        files += {name.toString(), href.toString()};
    }

    return true;
}
