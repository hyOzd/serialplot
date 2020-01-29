/*
  Copyright © 2019 Hasan Yavuz Özderya

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

#ifndef DATATEXTVIEW_H
#define DATATEXTVIEW_H

#include <QWidget>

#include "stream.h"

namespace Ui {
class DataTextView;
}

class DataTextViewSink;

class DataTextView : public QWidget
{
    Q_OBJECT

public:
    explicit DataTextView(Stream* stream, QWidget *parent = 0);
    ~DataTextView();

    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

protected:
    void addData(const SamplePack& data);

    friend DataTextViewSink;

private:
    Ui::DataTextView *ui;
    DataTextViewSink* sink;
    Stream* _stream;
};

#endif // DATATEXTVIEW_H
