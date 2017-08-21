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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QObject>
#include <QAction>
#include <QVector>
#include <QString>
#include <QStringList>

#include "channelinfomodel.h"

class SnapshotView;
class MainWindow;

class Snapshot : public QObject
{
    Q_OBJECT

public:
    Snapshot(MainWindow* parent, QString name, ChannelInfoModel infoModel, bool saved = false);
    ~Snapshot();

    QVector<QVector<QPointF>> data;
    QAction* showAction();
    QAction* deleteAction();

    QString name();
    QString displayName(); ///< `name()` plus '*' if snapshot is not saved
    ChannelInfoModel* infoModel();
    void setName(QString name);
    QString channelName(unsigned channel);

    void save(QString fileName); ///< save snapshot data as CSV
    bool isSaved(); ///< snapshot has been saved at least once

signals:
    void deleteRequested(Snapshot*);
    void nameChanged(Snapshot*);

private:
    QString _name;
    ChannelInfoModel cInfoModel;
    QAction _showAction;
    QAction _deleteAction;
    MainWindow* mainWindow;
    SnapshotView* view;
    bool _saved;

private slots:
    void show();
    void viewClosed();

    void onDeleteTriggered();
};

#endif /* SNAPSHOT_H */
