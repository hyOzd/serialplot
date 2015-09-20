#ifndef SNAPSHOTVIEW_H
#define SNAPSHOTVIEW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QPen>
#include <QCloseEvent>
#include <qwt_plot_curve.h>
#include "plot.h"
#include "snapshot.h"

namespace Ui {
class SnapShotView;
}

class SnapShotView : public QMainWindow
{
    Q_OBJECT

public:
    explicit SnapShotView(QWidget *parent, SnapShot* snapShot);
    ~SnapShotView();

signals:
    void closed();

private:
    Ui::SnapShotView *ui;
    QList<QwtPlotCurve*> curves;

    SnapShot* _snapShot;
    void closeEvent(QCloseEvent *event);
};

#endif // SNAPSHOTVIEW_H
