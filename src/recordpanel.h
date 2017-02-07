/*
  Copyright © 2017 Hasan Yavuz Özderya

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

#ifndef RECORDPANEL_H
#define RECORDPANEL_H

#include <QWidget>
#include <QToolBar>
#include <QAction>

namespace Ui {
class RecordPanel;
}

class RecordPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RecordPanel(QWidget *parent = 0);
    ~RecordPanel();

    QToolBar* toolbar();

private:
    Ui::RecordPanel *ui;
    QToolBar recordToolBar;
    QAction recordAction;
};

#endif // RECORDPANEL_H
