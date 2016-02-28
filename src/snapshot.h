/*
  Copyright © 2015 Hasan Yavuz Özderya

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
#include <QMainWindow>
#include <QAction>
#include <QVector>
#include <QString>

class SnapshotView;

class Snapshot : public QObject
{
    Q_OBJECT

public:
    Snapshot(QMainWindow* parent, QString name);
    ~Snapshot();

    QVector<QVector<QPointF>> data;
    QAction* showAction();
    QAction* deleteAction();

    QString name();
    void setName(QString name);

    void save(QString fileName); /// save snapshot data as CSV

signals:
    void deleteRequested(Snapshot*);
    void nameChanged(Snapshot*);

private:
    QString _name;
    QAction _showAction;
    QAction _deleteAction;
    QMainWindow* mainWindow;
    SnapshotView* view;

private slots:
    void show();
    void viewClosed();

    void onDeleteTriggered();
};

#endif /* SNAPSHOT_H */
