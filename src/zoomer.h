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

#ifndef ZOOMER_H
#define ZOOMER_H

#include <scrollzoomer.h>

class Zoomer : public ScrollZoomer
{
    Q_OBJECT

public:
    Zoomer(QWidget *, bool doReplot=true);
    void zoom(int up);
    void zoom( const QRectF & );

signals:
    void unzoomed();

protected:
    /// Re-implemented to display selection size in the tracker text.
    QwtText trackerTextF(const QPointF &pos) const;
    /// Re-implemented for alpha background
    void drawRubberBand(QPainter* painter) const;
    /// Re-implemented for alpha background (masking is basically disabled)
    QRegion rubberBandMask() const;
    /// Overloaded for panning
    void widgetMousePressEvent(QMouseEvent* mouseEvent);
    /// Overloaded for panning
    void widgetMouseReleaseEvent(QMouseEvent* mouseEvent);
    /// Overloaded for panning
    void widgetMouseMoveEvent(QMouseEvent* mouseEvent);

private:
    bool is_panning;
    QPointF pan_point;
};

#endif // ZOOMER_H
