/*
  Copyright © 2018 Hasan Yavuz Özderya

  This file is part of serialplot.

  ledwidget is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ledwidget is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ledwidget.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ledwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>

LedWidget::LedWidget(QWidget* parent) : QWidget(parent),
    m_color(107,223,51),
    m_on(true)
{
    resize(20,20);
}

QSize LedWidget::sizeHint() const
{
    return QSize(20,20);
}

QSize LedWidget::minimumSizeHint() const
{
    return QSize(10, 10);
}

void LedWidget::setColor(QColor ledColor)
{
    if (m_color == ledColor) return;
    m_color = ledColor;
    update();
    emit colorChanged(m_color);
}

bool LedWidget::isOn()
{
    return m_on;
}

void LedWidget::setOn(bool on)
{
    if (on == m_on) return;
    m_on = on;
    update();
    emit onChanged(on);
}

void LedWidget::turnOn()
{
    setOn(true);
}

void LedWidget::turnOff()
{
    setOn(false);
}

void LedWidget::toggle()
{
    setOn(!m_on);
}

void LedWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    const qreal r = std::min(width(), height()) / 2; // maximum radius including glow
    const qreal glowOffset = std::max(2., r/5.);
    const qreal borderOffset = std::max(1., r/10.);
    const qreal shineOffset = std::max(1., r/20.);
    const QPointF center(width()/2, height()/2);

    const qreal gr = r;
    const qreal br = gr - glowOffset;   // border shape radius
    const qreal ir = br - borderOffset; // inner fill radius
    const qreal sr = ir - shineOffset;

    QColor borderColor(130,130,130);
    QColor shineColor(255, 255, 255, m_on ? 200 : 50);
    QColor fillColor(m_color);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // draw border
    painter.setPen(Qt::NoPen);
    painter.setBrush(borderColor);
    painter.drawEllipse(center, br, br);

    // draw infill
    if (!m_on) fillColor = fillColor.darker();
    painter.setBrush(fillColor);
    painter.drawEllipse(center, ir, ir);

    // draw glow
    if (m_on)
    {
        QColor glowColor(m_color);
        glowColor.setAlphaF(0.5);
        QRadialGradient glowGradient(center, gr, center);
        glowGradient.setColorAt(0, glowColor);
        glowGradient.setColorAt((r-glowOffset)/r, glowColor);
        glowGradient.setColorAt(1, Qt::transparent);
        painter.setBrush(glowGradient);
        painter.drawEllipse(center, gr, gr);
    }

    // draw shine
    QRadialGradient shineGradient(center, sr, center-QPoint(sr/2,sr/2));
    shineGradient.setColorAt(0, shineColor);
    shineGradient.setColorAt(1, Qt::transparent);
    painter.setBrush(shineGradient);
    painter.drawEllipse(center, sr, sr);
}
