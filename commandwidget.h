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

#ifndef COMMANDWIDGET_H
#define COMMANDWIDGET_H

#include <QWidget>
#include <QString>

namespace Ui {
class CommandWidget;
}

class CommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandWidget(QWidget *parent = 0);
    ~CommandWidget();

signals:
    void deleteRequested(CommandWidget* thisWidget); // emitted when delete button is clicked

    // emitted when send button is clicked
    //
    // in case of hex mode, command text should be a hexadecimal
    // string containing hexadecimal characters only (not even spaces)
    void sendCommand(QString command, bool ascii);

private:
    Ui::CommandWidget *ui;
    QString storedAsciiText;
    QString storedHexText;

    bool isASCIIMode(); // true: ascii mode, false hex mode

private slots:
    void onDeleteClicked();
    void onSendClicked();
    void onASCIIToggled(bool checked);
};

#endif // COMMANDWIDGET_H
