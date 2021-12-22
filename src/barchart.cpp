/*
  Copyright © 2021 Hasan Yavuz Özderya

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

#include "barchart.h"
#include <qwt_scale_map.h>
#include <qwt_text.h>
#include <QPalette>
#include <math.h>

BarChart::BarChart(const Stream* stream)
{
    _stream = stream;
    setSpacing(0);
}

void BarChart::resample()
{
    setSamples(chartData());
}

QVector<double> BarChart::chartData() const
{
    unsigned numChannels = _stream->numChannels();
    unsigned numSamples = _stream->numSamples();
    QVector<double> data(numChannels);
    for (unsigned i = 0; i < numChannels; i++)
    {
        data[i] = _stream->channel(i)->yData()->sample(numSamples-1);
    }
    return data;
}

QwtColumnSymbol* BarChart::specialSymbol(int sampleIndex, const QPointF& sample) const
{
    unsigned numChannels = _stream->numChannels();
    if (sampleIndex < 0 || sampleIndex > (int) numChannels)
    {
        return NULL;
    }

    auto color = _stream->channel(sampleIndex)->color();

    QwtColumnSymbol* symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setLineWidth(1);
    symbol->setFrameStyle(QwtColumnSymbol::Plain);
    symbol->setPalette(QPalette(color));

    return symbol;
}

void BarChart::drawSample(
        QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, const QwtInterval &boundingInterval,
        int index, const QPointF &sample ) const
{
    QwtPlotBarChart::drawSample(painter, xMap, yMap, canvasRect, boundingInterval, index, sample);

    // calculate bar size
    const double barWidth = sampleWidth(xMap, canvasRect.width(),
                                        boundingInterval.width(), sample.y());
    const double x = xMap.transform( sample.x() );
    const double x1 = x - 0.5 * barWidth;
    // const double x2 = x + 0.5 * barWidth;

    const double y1 = yMap.transform(baseline());
    const double y2 = yMap.transform(sample.y());
    const double barHeight = fabs(y2 - y1);

    // create and calculate text size
    const auto valueStr = QString::number(sample.y());
    auto valueText = QwtText(valueStr, QwtText::PlainText);
    const auto textSize = valueText.textSize();
    auto r = QRectF(x1, y2, barWidth, textSize.height());
    if (y2 > y1) r.moveBottom(y2);

    // display text if there is enough space
    if (barHeight >= textSize.height() && barWidth >= textSize.width())
    {
        valueText.draw(painter, r);
    }
}
