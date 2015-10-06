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

#include "commandpanel.h"
#include "ui_commandpanel.h"

#include <QtDebug>

CommandPanel::CommandPanel(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel)
{
    serialPort = port;

    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setLayout(new QVBoxLayout);

    connect(ui->pbNew, &QPushButton::clicked, this, &CommandPanel::newCommand);
}

CommandPanel::~CommandPanel()
{
    delete ui;
}

void CommandPanel::newCommand()
{
    auto command = new CommandWidget();
    ui->scrollAreaWidgetContents->layout()->addWidget(command);
    connect(command, &CommandWidget::sendCommand, this, &CommandPanel::sendCommand);
}

void CommandPanel::sendCommand(QString command, bool ascii)
{
    if (!serialPort->isOpen())
    {
        qCritical() << "Port is not open!";
        return;
    }

    if (ascii)
    {
        qDebug() << "Sending" << command;
        if (serialPort->write(command.toLatin1()) < 0)
        {
            qCritical() << "Send command failed!";
        }
    }
    else
    {
        qCritical("NYI");
    }
}
