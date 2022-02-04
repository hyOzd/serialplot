/*
  Copyright © 2022 Hasan Yavuz Özderya

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
#include <QTextStream>
#include <QVector>
#include <QPointF>
#include <QIcon>
#include <QtDebug>

#include "mainwindow.h"
#include "snapshotmanager.h"

SnapshotManager::SnapshotManager(MainWindow* mainWindow,
                                 Stream* stream) :
    _menu("&Snapshots"),
    _takeSnapshotAction("&Take Snapshot", this),
    loadSnapshotAction("&Load Snapshots", this),
    clearAction("&Clear Snapshots", this)
{
    _mainWindow = mainWindow;
    _stream = stream;

    _takeSnapshotAction.setToolTip("Take a snapshot of current plot");
    _takeSnapshotAction.setShortcut(QKeySequence("Ctrl+P"));
    _takeSnapshotAction.setIcon(QIcon::fromTheme("camera"));
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

Snapshot* SnapshotManager::makeSnapshot() const
{
    QString name = QTime::currentTime().toString("'Snapshot ['HH:mm:ss']'");
    auto snapshot = new Snapshot(_mainWindow, name, *(_stream->infoModel()));

    for (unsigned ci = 0; ci < _stream->numChannels(); ci++)
    {
        snapshot->xData.append(new IndexBuffer(_stream->numSamples()));
        snapshot->yData.append(new ReadOnlyBuffer(_stream->channel(ci)->yData()));
    }

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
    QVector<QVector<double>> data(numOfChannels);
    QTextStream ts(&file);
    QString line;
    unsigned lineNum = 1;

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    while (ts.readLineInto(&line))
    {
#else
    while (true)
    {
        line = ts.readLine();
        if (line.isNull()) break;
#endif
        // parse line
        auto split = line.split(',');

        if (split.size() != (int) numOfChannels)
        {
            qCritical() << "Parsing error at line " << lineNum
                        << ": number of columns is not consistent.";
            qCritical() << "Line " << lineNum << ": " << line;
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
            data[ci].append(y);
        }
        lineNum++;
    }

    // create snapshot
    auto snapshot = new Snapshot(
        _mainWindow, QFileInfo(fileName).baseName(),
        ChannelInfoModel(channelNames), true);

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        snapshot->xData.append(new IndexBuffer(data[ci].size()));
        snapshot->yData.append(new ReadOnlyBuffer(data[ci].data(), data[ci].size()));
    }

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
