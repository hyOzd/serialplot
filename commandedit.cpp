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

#include <QKeyEvent>

#include "commandedit.h"

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

CommandEdit::CommandEdit(QWidget *parent) :
    QLineEdit(parent)
{
    hexValidator = new HexCommandValidator(this);
    asciiValidator = new QRegExpValidator(QRegExp("[\\x0000-\\x007F]+"));
    ascii_mode = true;
    setValidator(asciiValidator);
}

CommandEdit::~CommandEdit()
{
    delete hexValidator;
}

void CommandEdit::setMode(bool ascii)
{
    ascii_mode = ascii;
    if (ascii)
    {
        setValidator(asciiValidator);
    }
    else
    {
        setValidator(hexValidator);
    }
}

void CommandEdit::keyPressEvent(QKeyEvent * event)
{
    if (ascii_mode)
    {
        QLineEdit::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Backspace && !hasSelectedText())
    {
        int cursor = cursorPosition();
        if (cursor != 0 && text()[cursor-1] == ' ')
        {
            setCursorPosition(cursor-1);
        }
    }

    QLineEdit::keyPressEvent(event);
}
