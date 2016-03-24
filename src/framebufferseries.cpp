
#include "framebufferseries.h"

FrameBufferSeries::FrameBufferSeries(FrameBuffer* buffer)
{
    _buffer = buffer;
}

size_t FrameBufferSeries::size() const
{
    return _buffer->size();
}

QPointF FrameBufferSeries::sample(size_t i) const
{
    return _buffer->sample(i);
}

QRectF FrameBufferSeries::boundingRect() const
{
    return _buffer->boundingRect();
}
