#ifndef SNAPSHOTVIEW_H
#define SNAPSHOTVIEW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <qwt_plot_curve.h>
#include "plot.h"

class SnapShotView;
struct SnapShot
{
    QString name;
    QVector<QVector<QPointF>> data;
    SnapShotView* view;
};

namespace Ui {
class SnapShotView;
}

class SnapShotView : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapShotView(QWidget *parent, SnapShot* snapShot);
    ~SnapShotView();

private:
    Ui::SnapShotView *ui;
    QList<QwtPlotCurve*> curves;

    SnapShot* _snapShot;
};

#endif // SNAPSHOTVIEW_H
