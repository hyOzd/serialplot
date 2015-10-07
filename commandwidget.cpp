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

class HexCommandValidator : public QRegExpValidator
{
public:
    explicit HexCommandValidator(QObject* parent = 0);
    QValidator::State validate(QString & input, int & pos) const;
};

HexCommandValidator::HexCommandValidator(QObject* parent) :
    QRegExpValidator(parent)
{
    QRegExp regExp("^(?:(?:[0-9A-F]{2}[ ])+(?:[0-9A-F]{2}))|(?:[0-9A-F]{2})$");
    setRegExp(regExp);
}

QValidator::State HexCommandValidator::validate(QString & input, int & pos) const
{
    input = input.toUpper();

    // don't let pos to be altered at this stage
    int orgPos = pos;
    auto r = QRegExpValidator::validate(input, pos);
    pos = orgPos;

    // try fixing up spaces
    if (r != QValidator::Acceptable)
    {
        input = input.replace(" ", "");
        input.replace(QRegExp("([0-9A-F]{2}(?!$))"), "\\1 ");
        if (pos == input.size()-1) pos = input.size();
        r = QRegExpValidator::validate(input, pos);
    }

    return r;
}

CommandWidget::CommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandWidget)
{
    ui->setupUi(this);

    connect(ui->pbDelete, &QPushButton::clicked, this, &CommandWidget::onDeleteClicked);
    connect(ui->pbSend, &QPushButton::clicked, this, &CommandWidget::onSendClicked);
    connect(ui->pbASCII, &QPushButton::toggled, this, &CommandWidget::onASCIIToggled);

    hexValidator = new HexCommandValidator(this);
}

CommandWidget::~CommandWidget()
{
    delete ui;
    delete hexValidator;
}

void CommandWidget::onDeleteClicked()
{
    this->deleteLater();
}

void CommandWidget::onSendClicked()
{
    emit sendCommand(ui->leCommand->text(), ui->pbASCII->isChecked());
}

void CommandWidget::onASCIIToggled(bool checked)
{
    if (checked)
    {
        ui->leCommand->setValidator(0);
    }
    else
    {
        ui->leCommand->setValidator(hexValidator);
    }
}
