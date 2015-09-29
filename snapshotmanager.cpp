
#include <QTime>
#include <QMenuBar>
#include <QKeySequence>
#include <QFileDialog>
#include <QFile>
#include <QVector>
#include <QPointF>

#include "snapshotmanager.h"

SnapshotManager::SnapshotManager(QMainWindow* mainWindow,
                                 QList<FrameBuffer*>* channelBuffers) :
    _menu("Snapshots"),
    _takeSnapshotAction("Take Snapshot", this),
    loadSnapshotAction("Load Snapshot", this),
    clearAction("Clear Snapshots", this)
{
    _mainWindow = mainWindow;
    _channelBuffers = channelBuffers;

    _takeSnapshotAction.setToolTip("Take a snapshot of current plot (F5)");
    _takeSnapshotAction.setShortcut(QKeySequence("F5"));
    loadSnapshotAction.setToolTip("Load a snapshot from CSV file");
    clearAction.setToolTip("Delete all snapshots");
    connect(&_takeSnapshotAction, SIGNAL(triggered(bool)),
            this, SLOT(takeSnapshot()));
    connect(&clearAction, SIGNAL(triggered(bool)),
            this, SLOT(clearSnapshots()));
    connect(&loadSnapshotAction, SIGNAL(triggered(bool)),
            this, SLOT(loadSnapshot()));

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
    _menu.addAction(&loadSnapshotAction);
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
    snapshots.removeOne(snapshot);
    snapshot->deleteLater(); // regular delete causes a crash when triggered from menu
    updateMenu();
}

void SnapshotManager::loadSnapshot()
{
    QString fileName = QFileDialog::getOpenFileName(_mainWindow, tr("Load CSV File"));

    if (fileName.isNull()) return; // user canceled

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Couldn't open file: " << fileName;
        qCritical() << file.errorString();
        return;
    }

    // read first row as headlines and determine number of channels
    auto headLine = QString(file.readLine());
    unsigned numOfChannels = headLine.split(',').size();

    // read data
    QVector<QVector<QPointF>> data(numOfChannels);
    unsigned lineNum = 1;
    while (file.canReadLine())
    {
        // parse line
        auto line = QString(file.readLine());
        auto split = line.split(',');

        if (split.size() != (int) numOfChannels)
        {
            qCritical() << "Parsing error at line " << lineNum
                        << ": number of columns is not consistent.";
            return;
        }

        for (unsigned ci = 0; ci < numOfChannels; ci++)
        {
            // parse column
            bool ok;
            double y = split[ci].toDouble(&ok);
            if (!ok)
            {
                qCritical() << "Parsing error at line " << lineNum
                            << ", column " << ci
                            << ": can't convert \"" << split[ci]
                            << "\" to double.";
                return;
            }
            data[ci].append(QPointF(lineNum-1, y));
        }
        lineNum++;
    }

    auto snapShot = new SnapShot(_mainWindow, QFileInfo(fileName).baseName());
    snapShot->data = data;

    snapshots.append(snapShot);
    QObject::connect(snapShot, &SnapShot::deleteRequested,
                     this, &SnapshotManager::deleteSnapshot);
    QObject::connect(snapShot, &SnapShot::deleteRequested,
                     this, &SnapshotManager::updateMenu);

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
