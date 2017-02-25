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

#ifndef SCALEPICKER_H
#define SCALEPICKER_H

#include <QObject>
#include <QMouseEvent>
#include <QPen>
#include <QWidget>
#include <QList>
#include <QMap>
#include <qwt_scale_widget.h>
#include <qwt_widget_overlay.h>

class ScalePicker : public QObject
{
    Q_OBJECT

public:
    ScalePicker(QwtScaleWidget* scaleWidget, QWidget* canvas);
    virtual bool eventFilter(QObject*, QEvent*);

    void drawPlotOverlay(QPainter*); // called from ScalePickerOverlay
    void drawScaleOverlay(QPainter*); // called from ScaleOverlay
    void setPen(QPen pen);

signals:
    void pickStarted(double pos);
    void picking(double firstPos, double lastPos);
    void picked(double firstPos, double lastPos);

private:
    QwtScaleWidget* _scaleWidget;
    QWidget* _canvas;
    QwtWidgetOverlay* pickerOverlay; // will be PlotOverlay
    QwtWidgetOverlay* scaleOverlay;  // will be ScaleOverlay
    QPen _pen;

    bool pressed;
    bool started;
    double firstPos; // converted to plot coordinates
    double firstPosPx; // pixel coordinates
    double currentPosPx; // current position in pixel coordinates
    QList<int> snapPoints;
    /// used to restore precision of snappoints that is lost due to rounding
    QMap<int, double> snapPointMap;

    double position(double) const; // returns the axis mouse position relative to plot coordinates
    int positionPx(QMouseEvent*); // returns the axis mouse position in pixels
    double posCanvasPx(double pos) const; // returns the given position in canvas coordinates
    void drawTriangle(QPainter* painter, int position);
    QwtText trackerText() const;
     /// Returns tracker text position
    QRectF trackerTextRect(QPainter* painter, int posPx, QSizeF textSize) const;
    /// Returns the text position for tracker text shown during picking
    QRectF pickTrackerTextRect(QPainter* painter, QRect pickRect, QSizeF textSize) const;

private slots:
    void updateSnapPoints();
};

#endif // SCALEPICKER_H
