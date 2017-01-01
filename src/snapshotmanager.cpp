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

#include <QTime>
#include <QMenuBar>
#include <QKeySequence>
#include <QFileDialog>
#include <QFile>
#include <QVector>
#include <QPointF>
#include <QtDebug>

#include "snapshotmanager.h"

SnapshotManager::SnapshotManager(QMainWindow* mainWindow,
                                 ChannelManager* channelMan) :
    _menu("&Snapshots"),
    _takeSnapshotAction("&Take Snapshot", this),
    loadSnapshotAction("&Load Snapshots", this),
    clearAction("&Clear Snapshots", this)
{
    _mainWindow = mainWindow;
    _channelMan = channelMan;

    _takeSnapshotAction.setToolTip("Take a snapshot of current plot");
    _takeSnapshotAction.setShortcut(QKeySequence("F5"));
    loadSnapshotAction.setToolTip("Load snapshots from CSV files");
    clearAction.setToolTip("Delete all snapshots");
    connect(&_takeSnapshotAction, SIGNAL(triggered(bool)),
            this, SLOT(takeSnapshot()));
    connect(&clearAction, SIGNAL(triggered(bool)),
            this, SLOT(clearSnapshots()));
    connect(&loadSnapshotAction, SIGNAL(triggered(bool)),
            this, SLOT(loadSnapshots()));

    updateMenu();
}

SnapshotManager::~SnapshotManager()
{
    for (auto snapshot : snapshots)
    {
        delete snapshot;
    }
}

Snapshot* SnapshotManager::makeSnapshot()
{
    QString name = QTime::currentTime().toString("'Snapshot ['HH:mm:ss']'");
    auto snapshot = new Snapshot(_mainWindow, name);

    unsigned numOfChannels = _channelMan->numOfChannels();
    unsigned numOfSamples = _channelMan->numOfSamples();

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        snapshot->data.append(QVector<QPointF>(numOfSamples));
        for (unsigned i = 0; i < numOfSamples; i++)
        {
            snapshot->data[ci][i] = QPointF(i, _channelMan->channelBuffer(ci)->sample(i));
        }
    }
    snapshot->setChannelNames(_channelMan->channelNames());

    return snapshot;
}

void SnapshotManager::takeSnapshot()
{
    addSnapshot(makeSnapshot());
}

void SnapshotManager::addSnapshot(Snapshot* snapshot, bool update_menu)
{
    snapshots.append(snapshot);
    QObject::connect(snapshot, &Snapshot::deleteRequested,
                     this, &SnapshotManager::deleteSnapshot);
    if (update_menu) updateMenu();
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

void SnapshotManager::deleteSnapshot(Snapshot* snapshot)
{
    snapshots.removeOne(snapshot);
    snapshot->deleteLater(); // regular delete causes a crash when triggered from menu
    updateMenu();
}

void SnapshotManager::loadSnapshots()
{
    auto files = QFileDialog::getOpenFileNames(_mainWindow, tr("Load CSV File"));

    for (auto f : files)
    {
        if (!f.isNull()) loadSnapshotFromFile(f);
    }

    updateMenu();
}

void SnapshotManager::loadSnapshotFromFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Couldn't open file: " << fileName;
        qCritical() << file.errorString();
        return;
    }

    // read first row as headlines and determine number of channels
    auto headLine = QString(file.readLine());
    QStringList channelNames = headLine.split(',');
    unsigned numOfChannels = channelNames.size();

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

    auto snapshot = new Snapshot(_mainWindow, QFileInfo(fileName).baseName());
    snapshot->data = data;
    snapshot->setChannelNames(channelNames);

    addSnapshot(snapshot, false);
}

QMenu* SnapshotManager::menu()
{
    return &_menu;
}

QAction* SnapshotManager::takeSnapshotAction()
{
    return &_takeSnapshotAction;
}

bool SnapshotManager::isAllSaved()
{
    for (auto snapshot : snapshots)
    {
        if (!snapshot->isSaved()) return false;
    }
    return true;
}
