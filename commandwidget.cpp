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

#include "commandwidget.h"
#include "ui_commandwidget.h"

#include <QRegExp>
#include <QRegExpValidator>

#include <QtDebug>

CommandWidget::CommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandWidget)
{
    ui->setupUi(this);

    connect(ui->pbDelete, &QPushButton::clicked, this, &CommandWidget::onDeleteClicked);
    connect(ui->pbSend, &QPushButton::clicked, this, &CommandWidget::onSendClicked);
    connect(ui->pbASCII, &QPushButton::toggled, this, &CommandWidget::onASCIIToggled);
}

CommandWidget::~CommandWidget()
{
    delete ui;
}

void CommandWidget::onDeleteClicked()
{
    this->deleteLater();
}

void CommandWidget::onSendClicked()
{
    auto command = ui->leCommand->text();

    if (command.isEmpty())
    {
        qWarning() << "Enter a command to send!";
        ui->leCommand->setFocus(Qt::OtherFocusReason);
        return;
    }

    if (!isASCIIMode()) // hex mode
    {
        command = command.remove(' ');
        // check if nibbles are missing
        if (command.size() % 2 == 1)
        {
            qWarning() << "HEX command is missing a nibble at the end!";
            ui->leCommand->setFocus(Qt::OtherFocusReason);
            int textSize = ui->leCommand->text().size();
            ui->leCommand->setSelection(textSize-1, textSize);
            return;
        }
    }

    emit sendCommand(command, ui->pbASCII->isChecked());
}

void CommandWidget::onASCIIToggled(bool checked)
{
    ui->leCommand->setMode(checked);
}

bool CommandWidget::isASCIIMode()
{
    return ui->pbASCII->isChecked();
}
