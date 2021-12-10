/*
  Copyright © 2020 Hasan Yavuz Özderya

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

#include <QByteArray>
#include <QtDebug>

#include "commandpanel.h"
#include "ui_commandpanel.h"
#include "setting_defines.h"

CommandPanel::CommandPanel(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel),
    _menu("&Commands"), _newCommandAction("&New Command", this)
{
    serialPort = port;

    ui->setupUi(this);
    auto layout = new QVBoxLayout();
    layout->setSpacing(0);
    ui->scrollAreaWidgetContents->setLayout(layout);

    connect(ui->pbNew, &QPushButton::clicked, this, &CommandPanel::newCommand);
    connect(&_newCommandAction, &QAction::triggered, this, &CommandPanel::newCommand);

    _menu.addAction(&_newCommandAction);
    _menu.addSeparator();

    command_name_counter = 0;
}

CommandPanel::~CommandPanel()
{
    commands.clear(); // UI will 'delete' actual objects
    delete ui;
}

CommandWidget* CommandPanel::newCommand()
{
    auto command = new CommandWidget();
    command_name_counter++;
    command->setName("Command " + QString::number(command_name_counter));
    ui->scrollAreaWidgetContents->layout()->addWidget(command);
    command->setFocusToEdit();
    connect(command, &CommandWidget::sendCommand, this, &CommandPanel::sendCommand);
    connect(command, &CommandWidget::focusRequested, this, &CommandPanel::focusRequested);
    _menu.addAction(command->sendAction());

    // add to command list and remove on destroy
    commands << command;
    connect(command, &QObject::destroyed, [this](QObject* obj)
            {
                commands.removeOne(static_cast<CommandWidget*>(obj));
            });
    return command;
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

unsigned CommandPanel::numOfCommands()
{
    return commands.size();
}

void CommandPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Commands);
    settings->beginWriteArray(SG_Commands_Command);
    for (int i = 0; i < commands.size(); i ++)
    {
        settings->setArrayIndex(i);
        auto command = commands[i];
        settings->setValue(SG_Commands_Name, command->name());
        settings->setValue(SG_Commands_Type, command->isASCIIMode() ? "ascii" : "hex");
        settings->setValue(SG_Commands_Data, command->commandText());
    }
    settings->endArray();
    settings->endGroup();
}

void CommandPanel::loadSettings(QSettings* settings)
{
    // clear all commands
    while (commands.size())
    {
        auto command = commands.takeLast();
        command->disconnect();
        delete command;
    }

    // load commands
    settings->beginGroup(SettingGroup_Commands);
    unsigned size = settings->beginReadArray(SG_Commands_Command);

    for (unsigned i = 0; i < size; i ++)
    {
        settings->setArrayIndex(i);
        auto command = newCommand();

        // load command name
        QString name = settings->value(SG_Commands_Name, "").toString();
        if (!name.isEmpty()) command->setName(name);

        // Important: type should be set before command data for correct validation
        QString type = settings->value(SG_Commands_Type, "").toString();
        if (type == "ascii")
        {
            command->setASCIIMode(true);
        }
        else if (type == "hex")
        {
            command->setASCIIMode(false);
        } // else unchanged

        // load command data
        command->setCommandText(settings->value(SG_Commands_Data, "").toString());
    }

    settings->endArray();
    settings->endGroup();
}
