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

#ifndef PLOTSNAPSHOTOVERLAY_H
#define PLOTSNAPSHOTOVERLAY_H

#include <QColor>
#include <QPainter>
#include <QTimer>
#include <qwt_widget_overlay.h>

// Draws a sort of flashing effect on plot widget when snapshot taken
class PlotSnapshotOverlay : public QwtWidgetOverlay
{
    Q_OBJECT

public:
    PlotSnapshotOverlay(QWidget* widget, QColor color);

protected:
    void drawOverlay(QPainter*) const;

signals:
    void done(); // indicates that animation completed

private:
    QColor _color;
    QTimer animTimer; // controls fading
    QTimer updateTimer; // need to force repaint the canvas
};

#endif // PLOTSNAPSHOTOVERLAY_H
