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

#ifndef COMMANDPANEL_H
#define COMMANDPANEL_H

#include <QWidget>
#include <QSerialPort>
#include <QByteArray>

#include "commandwidget.h"

namespace Ui {
class CommandPanel;
}

class CommandPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CommandPanel(QSerialPort* port, QWidget *parent = 0);
    ~CommandPanel();

private:
    Ui::CommandPanel *ui;
    QSerialPort* serialPort;

private slots:
    void newCommand();
    void sendCommand(QByteArray command);
};

#endif // COMMANDPANEL_H
