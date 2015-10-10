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

#ifndef COMMANDEDIT_H
#define COMMANDEDIT_H

#include <QLineEdit>
#include <QValidator>

class CommandEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CommandEdit(QWidget *parent = 0);
    ~CommandEdit();
    void setMode(bool ascii); // true = ascii, false = hex

private:
    bool ascii_mode;
    QValidator* hexValidator;
    QValidator* asciiValidator;

protected:
    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
};

#endif // COMMANDEDIT_H
