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

#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>

#include "framebuffer.h"
#include "channelmanager.h"
#include "snapshot.h"

class MainWindow;

class SnapshotManager : public QObject
{
    Q_OBJECT

public:
    SnapshotManager(MainWindow* mainWindow, ChannelManager* channelMan);
    ~SnapshotManager();

    QMenu* menu();
    QAction* takeSnapshotAction();

    /// Creates a dynamically allocated snapshot object but doesn't record it in snapshots list.
    /// @note Caller is responsible for deletion of the returned `Snapshot` object.
    Snapshot* makeSnapshot();

    bool isAllSaved(); ///< returns `true` if all snapshots are saved to a file

private:
    MainWindow* _mainWindow;
    ChannelManager* _channelMan;

    QList<Snapshot*> snapshots;

    QMenu _menu;
    QAction _takeSnapshotAction;
    QAction loadSnapshotAction;
    QAction clearAction;

    void addSnapshot(Snapshot* snapshot, bool update_menu=true);
    void updateMenu();

private slots:
    void takeSnapshot();
    void clearSnapshots();
    void deleteSnapshot(Snapshot* snapshot);
    void loadSnapshots();
    void loadSnapshotFromFile(QString fileName);
};

#endif /* SNAPSHOTMANAGER_H */
