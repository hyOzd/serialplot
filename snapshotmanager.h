
#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>

#include "framebuffer.h"
#include "snapshot.h"

// class MainWindow;

class SnapshotManager : public QObject
{
    Q_OBJECT

public:
    SnapshotManager(QMainWindow* mainWindow, QList<FrameBuffer*>* channelBuffers);
    ~SnapshotManager();

    QMenu* menu();
    QAction* takeSnapshotAction();

private:
    QMainWindow* _mainWindow;
    QList<FrameBuffer*>* _channelBuffers;

    QList<SnapShot*> snapshots;

    QMenu _menu;
    QAction _takeSnapshotAction;
    QAction clearAction;

    void updateMenu();

private slots:
    void takeSnapshot();
    void clearSnapshots();
    void deleteSnapshot(SnapShot* snapshot);

};

#endif /* SNAPSHOTMANAGER_H */
