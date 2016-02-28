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

#include <algorithm>
#include <QPen>

#include "plotsnapshotoverlay.h"

#define LINE_WIDTH (10)
#define ANIM_LENGTH (500) // milliseconds
#define UPDATE_PERIOD (20) // milliseconds

PlotSnapshotOverlay::PlotSnapshotOverlay(QWidget* widget, QColor color) :
    QwtWidgetOverlay(widget)
{
    _color = color;
    animTimer.setSingleShot(true);
    animTimer.setInterval(ANIM_LENGTH);
    updateTimer.setSingleShot(false);
    updateTimer.setInterval(UPDATE_PERIOD);

    connect(&updateTimer, &QTimer::timeout, [this](){this->updateOverlay();});
    connect(&animTimer, &QTimer::timeout, &updateTimer, &QTimer::stop);
    connect(&animTimer, &QTimer::timeout, this, &PlotSnapshotOverlay::done);

    animTimer.start();
    updateTimer.start();
}

void PlotSnapshotOverlay::drawOverlay(QPainter* painter) const
{
        if (!animTimer.isActive()) return;
    QColor lineColor = _color;

    double fadingRatio = ((double) animTimer.remainingTime()) / ANIM_LENGTH;
    lineColor.setAlpha(std::min(255, (int) (255*fadingRatio)));

    QPen pen(lineColor);
    pen.setWidth(LINE_WIDTH);
    pen.setJoinStyle(Qt::MiterJoin);

    int width = painter->device()->width();
    int height = painter->device()->height();

    painter->save();
    painter->setPen(pen);
    painter->drawRect(LINE_WIDTH/2, LINE_WIDTH/2, width-LINE_WIDTH, height-LINE_WIDTH);
    painter->restore();
}
