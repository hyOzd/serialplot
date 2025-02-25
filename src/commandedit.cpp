/*
  Copyright © 2025 Hasan Yavuz Özderya

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
#include <QRegularExpressionValidator>

#include <QtDebug>

#include "commandedit.h"

class HexCommandValidator : public QRegularExpressionValidator
{
public:
    explicit HexCommandValidator(QObject* parent = 0);
    QValidator::State validate(QString & input, int & pos) const;
};

HexCommandValidator::HexCommandValidator(QObject* parent) :
    QRegularExpressionValidator(parent)
{
    QRegularExpression regExp("^(?:(?:[0-9A-F]{2}[ ])+(?:[0-9A-F]{2}))|(?:[0-9A-F]{2})$");
    setRegularExpression(regExp);
}

QValidator::State HexCommandValidator::validate(QString & input, int & pos) const
{
    input = input.toUpper();

    // don't let pos to be altered at this stage
    int orgPos = pos;
    auto r = QRegularExpressionValidator::validate(input, pos);
    pos = orgPos;

    // try fixing up spaces
    if (r != QValidator::Acceptable)
    {
        input = input.replace(" ", "");
        input.replace(QRegularExpression("([0-9A-F]{2}(?!$))"), "\\1 ");
        if (pos == input.size()-1) pos = input.size();
        r = QRegularExpressionValidator::validate(input, pos);
    }

    return r;
}

CommandEdit::CommandEdit(QWidget *parent) :
    QLineEdit(parent)
{
    hexValidator = new HexCommandValidator(this);
    asciiValidator = new QRegularExpressionValidator(
        QRegularExpression("[\\x00-\\x7F]+"), this);
    ascii_mode = true;
    setValidator(asciiValidator);
}

CommandEdit::~CommandEdit()
{
    delete hexValidator;
}

static QString unEscape(QString str);
static QString escape(QString str);

void CommandEdit::setMode(bool ascii)
{
    ascii_mode = ascii;
    if (ascii)
    {
        setValidator(asciiValidator);

        auto hexText = text().remove(" ");
        // try patching HEX string in case of missing nibble so that
        // input doesn't turn into gibberish
        if (hexText.size() % 2 == 1)
        {
            hexText.replace(hexText.size()-1, 1, "3F"); // 0x3F = '?'
            qWarning() << "Broken byte in hex command is replaced. Check your command!";
        }

        setText(escape(QByteArray::fromHex(hexText.toLatin1())));
    }
    else
    {
        setValidator(hexValidator);
        setText(unEscape(text()).toLatin1().toHex());
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

QString CommandEdit::unEscapedText()
{
    return unEscape(text());
}

static QString unEscape(QString str)
{
    const QMap<QString, QString> replacements({
            {"\\\\", "\\"},
            {"\\n", "\n"},
            {"\\r", "\r"},
            {"\\t", "\t"}
        });

    QString result;

    int i = 0;
    while (i < str.size())
    {
        bool found = false;

        for (auto k : replacements.keys())
        {
            // has enough text left?
            if (str.size() - i < 1) continue;

            // try matching the key at current position
            if (k == QStringView(str).mid(i, k.size()))
            {
                // append replacement
                result += replacements[k];
                i += k.size();
                found = true;
                break; // skip other keys
            }
        }

        if (!found)
        {
            // append unmatched character
            result += str[i];
            i++;
        }
    }

    return result;
}

static QString escape(QString str)
{
    str.replace("\\", "\\\\");
    str.replace("\n", "\\n");
    str.replace("\r", "\\r");
    str.replace("\t", "\\t");
    return str;
}
