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

#include "snapshotview.h"
#include "ui_snapshotview.h"

SnapshotView::SnapshotView(QWidget *parent, Snapshot* snapshot) :
    QMainWindow(parent),
    ui(new Ui::SnapshotView),
    renameDialog(this)
{
    _snapshot = snapshot;

    ui->setupUi(this);
    ui->menuSnapshot->insertAction(ui->actionClose, snapshot->deleteAction());
    this->setWindowTitle(snapshot->name());

    unsigned numOfChannels = snapshot->data.size();

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        QwtPlotCurve* curve = new QwtPlotCurve(snapshot->channelName(ci));
        curves.append(curve);
        curve->setSamples(snapshot->data[ci]);
        curve->setPen(Plot::makeColor(ci));
        curve->attach(ui->plot);
    }

    renameDialog.setWindowTitle("Rename Snapshot");
    renameDialog.setLabelText("Enter new name:");
    connect(ui->actionRename, &QAction::triggered,
            this, &SnapshotView::showRenameDialog);

    connect(ui->actionExport, &QAction::triggered,
            this, &SnapshotView::save);

    for (auto a : ui->plot->menuActions())
    {
        ui->menuView->addAction(a);
    }
}

SnapshotView::~SnapshotView()
{
    for (auto curve : curves)
    {
        delete curve;
    }
    delete ui;
}

void SnapshotView::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    emit closed();
}

void SnapshotView::showRenameDialog()
{
    renameDialog.setTextValue(_snapshot->name());
    renameDialog.open(this, SLOT(renameSnapshot(QString)));
}

void SnapshotView::renameSnapshot(QString name)
{
    _snapshot->setName(name);
    setWindowTitle(name);
}

void SnapshotView::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV File"));

    if (fileName.isNull()) return; // user canceled

    _snapshot->save(fileName);
}
