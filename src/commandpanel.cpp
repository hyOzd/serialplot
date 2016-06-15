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

#include <QByteArray>
#include <QtDebug>

CommandPanel::CommandPanel(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel),
    _menu(trUtf8("Commands")), _newCommandAction(trUtf8("New Command"), this)
{
    serialPort = port;

    ui->setupUi(this);
    auto layout = new QVBoxLayout();
    layout->setSpacing(0);
    ui->scrollAreaWidgetContents->setLayout(layout);

#ifdef Q_OS_WIN
    ui->pbNew->setIcon(QIcon(":/icons/list-add"));
#endif // Q_OS_WIN

    connect(ui->pbNew, &QPushButton::clicked, this, &CommandPanel::newCommand);
    connect(&_newCommandAction, &QAction::triggered, this, &CommandPanel::newCommand);

    _menu.addAction(&_newCommandAction);
    _menu.addSeparator();

    command_name_counter = 0;
    newCommand(); // add an empty slot by default
}

CommandPanel::~CommandPanel()
{
    delete ui;
}

void CommandPanel::newCommand()
{
    auto command = new CommandWidget();
    command_name_counter++;
    command->setName(trUtf8("Command ") + QString::number(command_name_counter));
    ui->scrollAreaWidgetContents->layout()->addWidget(command);
    command->setFocusToEdit();
    connect(command, &CommandWidget::sendCommand, this, &CommandPanel::sendCommand);
    _menu.addAction(command->sendAction());
}

void CommandPanel::sendCommand(QByteArray command)
{
    if (!serialPort->isOpen())
    {
        qCritical() << "Port is not open!";
        return;
    }

    if (serialPort->write(command) < 0)
    {
        qCritical() << "Send command failed!";
    }
}

QMenu* CommandPanel::menu()
{
    return &_menu;
}

QAction* CommandPanel::newCommandAction()
{
    return &_newCommandAction;
}
