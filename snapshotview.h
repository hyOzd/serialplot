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
    QInputDialog renameDialog;

    void closeEvent(QCloseEvent *event);

private slots:
    void showRenameDialog();
    void renameSnapshot(QString name);
    void save();
};

#endif // SNAPSHOTVIEW_H
