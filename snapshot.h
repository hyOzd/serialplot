#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QObject>
#include <QMainWindow>
#include <QAction>
#include <QVector>
#include <QString>

class SnapShotView;

class SnapShot : public QObject
{
    Q_OBJECT

public:
    SnapShot(QMainWindow* parent, QString name);
    ~SnapShot();

    QVector<QVector<QPointF>> data;
    QAction* showAction();
    QAction* deleteAction();

    QString name();
    void setName(QString name);

signals:
    void deleteRequested(SnapShot*);
    void nameChanged(SnapShot*);

private:
    QString _name;
    QAction _showAction;
    QAction _deleteAction;
    QMainWindow* mainWindow;
    SnapShotView* view;

private slots:
    void show();
    void viewClosed();

    void onDeleteTriggered();
};

#endif /* SNAPSHOT_H */
