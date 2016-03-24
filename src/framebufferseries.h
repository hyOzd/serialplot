
#ifndef FRAMEBUFFERSERIES_H
#define FRAMEBUFFERSERIES_H

#include <QPointF>
#include <QRectF>
#include <qwt_series_data.h>

#include "framebuffer.h"

/**
 * This class provides an interface for actual FrameBuffer
 * object. That way we can keep our data structures relatively
 * isolated from Qwt. Otherwise QwtPlotCurve owns FrameBuffer
 * structures.
 */
class FrameBufferSeries : public QwtSeriesData<QPointF>
{
public:
    FrameBufferSeries(FrameBuffer* buffer);

    // QwtSeriesData implementations
    size_t size() const;
    QPointF sample(size_t i) const;
    QRectF boundingRect() const;

private:
    FrameBuffer* _buffer;
};

#endif // FRAMEBUFFERSERIES_H
