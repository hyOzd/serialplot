#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);

signals:
    void updateFound();
    void checkFinished(bool found, QString newVersion, QString downloadUrl);
    void checkFailed(QString errorMessage);

public slots:
    void checkUpdate();

private:
    struct FileInfo
    {
        QString name;
        QString link;
    };

    QNetworkAccessManager nam;
    bool parseData(const QJsonDocument& data, QList<FileInfo>& files);

private slots:
    void onReqFinished(QNetworkReply* reply);
};

#endif // UPDATECHECKER_H
