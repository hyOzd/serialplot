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

#ifndef LEDWIDGET_H
#define LEDWIDGET_H

#include <QWidget>
#include <QSize>
#include <QColor>

class LedWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor color MEMBER m_color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool on READ isOn WRITE setOn NOTIFY onChanged)

public:
    explicit LedWidget(QWidget *parent = 0);

    void setColor(QColor ledColor);
    bool isOn();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

signals:
    void colorChanged(QColor ledColor);
    void onChanged(bool on);

public slots:
    void setOn(bool on);
    void turnOn();
    void turnOff();
    void toggle();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QColor m_color;
    bool m_on;
};

#endif // LEDWIDGET_H
