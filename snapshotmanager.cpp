
#include <QTime>
#include <QMenuBar>
#include <QKeySequence>

#include "snapshotmanager.h"

SnapshotManager::SnapshotManager(QMainWindow* mainWindow,
                                 QList<FrameBuffer*>* channelBuffers) :
    _menu("Snapshots"),
    _takeSnapshotAction("Take Snapshot", this),
    clearAction("Clear Snapshots", this)
{
    _mainWindow = mainWindow;
    _channelBuffers = channelBuffers;

    _takeSnapshotAction.setToolTip("Take a snapshot of current plot (F5)");
    _takeSnapshotAction.setShortcut(QKeySequence("F5"));
    clearAction.setToolTip("Delete all snapshots");
    connect(&_takeSnapshotAction, SIGNAL(triggered(bool)),
            this, SLOT(takeSnapshot()));
    connect(&clearAction, SIGNAL(triggered(bool)),
            this, SLOT(clearSnapshots()));

    updateMenu();
}

SnapshotManager::~SnapshotManager()
{
    for (auto snapshot : snapshots)
    {
        delete snapshot;
    }
}

void SnapshotManager::takeSnapshot()
{
    QString name = QTime::currentTime().toString("'Snapshot ['HH:mm:ss']'");
    auto snapShot = new SnapShot(_mainWindow, name);

    unsigned numOfChannels = _channelBuffers->size();
    unsigned numOfSamples = _channelBuffers->at(0)->size();

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        snapShot->data.append(QVector<QPointF>(numOfSamples));
        for (unsigned i = 0; i < numOfSamples; i++)
        {
            snapShot->data[ci][i] = _channelBuffers->at(ci)->sample(i);
        }
    }
    snapshots.append(snapShot);
    QObject::connect(snapShot, &SnapShot::deleteRequested,
                     this, &SnapshotManager::deleteSnapshot);
    QObject::connect(snapShot, &SnapShot::deleteRequested,
                     this, &SnapshotManager::updateMenu);

    updateMenu();
}

void SnapshotManager::updateMenu()
{
    _menu.clear();
    _menu.addAction(&_takeSnapshotAction);
    if (snapshots.size())
    {
        _menu.addSeparator();
        for (auto ss : snapshots)
        {
            _menu.addAction(ss->showAction());
        }
        _menu.addSeparator();
        _menu.addAction(&clearAction);
    }
}

void SnapshotManager::clearSnapshots()
{
    for (auto snapshot : snapshots)
    {
        delete snapshot;
    }
    snapshots.clear();
    updateMenu();
}

void SnapshotManager::deleteSnapshot(SnapShot* snapshot)
{
    delete snapshots.takeAt(snapshots.indexOf(snapshot));
    updateMenu();
}

QMenu* SnapshotManager::menu()
{
    return &_menu;
}

QAction* SnapshotManager::takeSnapshotAction()
{
    return &_takeSnapshotAction;
}
