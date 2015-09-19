#include "snapshotview.h"
#include "ui_snapshotview.h"

SnapShotView::SnapShotView(QWidget *parent, SnapShot* snapShot) :
    QMainWindow(parent),
    ui(new Ui::SnapShotView)
{
    ui->setupUi(this);

    unsigned numOfChannels = snapShot->data.size();

    for (unsigned ci = 0; ci < numOfChannels; ci++)
    {
        QwtPlotCurve* curve = new QwtPlotCurve();
        curves.append(curve);
        curve->setSamples(snapShot->data[ci]);
        curve->setPen(Plot::makeColor(ci));
        curve->attach(ui->plot);
    }

    _snapShot = snapShot;
}

SnapShotView::~SnapShotView()
{
    for (auto curve : curves)
    {
        delete curve;
    }
    delete ui;
}
