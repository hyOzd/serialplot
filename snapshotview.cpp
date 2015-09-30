#include "snapshotview.h"
#include "ui_snapshotview.h"

#include <QSaveFile>

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

    connect(ui->actionExport, &QAction::triggered,
            this, &SnapShotView::save);

    for (auto a : ui->plot->menuActions())
    {
        ui->menuView->addAction(a);
    }
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

void SnapShotView::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV File"));

    if (fileName.isNull()) return; // user canceled

    // TODO: remove code duplication (MainWindow::onExportCsv)
    QSaveFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);

        unsigned numOfChannels = _snapShot->data.size();
        unsigned numOfSamples = _snapShot->data[0].size();

        // print header
        for (unsigned int ci = 0; ci < numOfChannels; ci++)
        {
            fileStream << "Channel " << ci;
            if (ci != numOfChannels-1) fileStream << ",";
        }
        fileStream << '\n';

        // print rows
        for (unsigned int i = 0; i < numOfSamples; i++)
        {
            for (unsigned int ci = 0; ci < numOfChannels; ci++)
            {
                fileStream << _snapShot->data[ci][i].y();
                if (ci != numOfChannels-1) fileStream << ",";
            }
            fileStream << '\n';
        }

        if (!file.commit())
        {
            qCritical() << "File save error during snapshot save: " << file.error();
        }
    }
    else
    {
        qCritical() << "File open error during snapshot save: " << file.error();
    }
}
