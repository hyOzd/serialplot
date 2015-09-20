#include "snapshotview.h"
#include "ui_snapshotview.h"

SnapShotView::SnapShotView(QWidget *parent, SnapShot* snapShot) :
    QMainWindow(parent),
    ui(new Ui::SnapShotView),
    renameDialog(this)
{
    _snapShot = snapShot;

    ui->setupUi(this);
    ui->menuSnapshot->addAction(snapShot->deleteAction());
    this->setWindowTitle(snapShot->name());

    unsigned numOfChannels = snapShot->data.size();

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        QwtPlotCurve* curve = new QwtPlotCurve();
        curves.append(curve);
        curve->setSamples(snapShot->data[ci]);
        curve->setPen(Plot::makeColor(ci));
        curve->attach(ui->plot);
    }

    renameDialog.setWindowTitle("Rename Snapshot");
    renameDialog.setLabelText("Enter new name:");
    connect(ui->actionRename, &QAction::triggered,
            this, &SnapShotView::showRenameDialog);
}

SnapShotView::~SnapShotView()
{
    for (auto curve : curves)
    {
        delete curve;
    }
    delete ui;
}

void SnapShotView::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    emit closed();
}

void SnapShotView::showRenameDialog()
{
    renameDialog.setTextValue(_snapShot->name());
    renameDialog.open(this, SLOT(renameSnapshot(QString)));
}

void SnapShotView::renameSnapshot(QString name)
{
    _snapShot->setName(name);
    setWindowTitle(name);
}
