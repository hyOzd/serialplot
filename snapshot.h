#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QObject>
#include <QMainWindow>
#include <QAction>
#include <QVector>

class SnapShotView;

class SnapShot : public QObject
{
    Q_OBJECT

public:
    SnapShot(QMainWindow* parent, QString name);

    // QString _name;
    QVector<QVector<QPointF>> data;
    QAction* menuAction();

public slots:
    void show();
    void hide();

private:
    QAction _menuAction;
    QMainWindow* mainWindow;
    SnapShotView* view;
};

#endif /* SNAPSHOT_H */
