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

#ifndef SNAPSHOTVIEW_H
#define SNAPSHOTVIEW_H

#include <QMainWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QVector>
#include <QPointF>
#include <QPen>
#include <QCloseEvent>
#include <qwt_plot_curve.h>

#include "plot.h"
#include "snapshot.h"

namespace Ui {
class SnapshotView;
}

class SnapshotView : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapshotView(QWidget *parent, Snapshot* snapshot);
    ~SnapshotView();

signals:
    void closed();

private:
    Ui::SnapshotView *ui;
    QList<QwtPlotCurve*> curves;
    Snapshot* _snapshot;
    QInputDialog renameDialog;

    void closeEvent(QCloseEvent *event);

private slots:
    void showRenameDialog();
    void renameSnapshot(QString name);
    void save();
};

#endif // SNAPSHOTVIEW_H
