/*
  Copyright © 2018 Hasan Yavuz Özderya

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

#ifndef DEMOREADER_H
#define DEMOREADER_H

#include <QTimer>

#include "abstractreader.h"
#include "demoreadersettings.h"

/**
 * This is a special case of reader implementation and should be used
 * with care.
 *
 * There is no settings widget. Number of channels should be set from
 * currently selected actual readers settings widget.
 *
 * This reader should not be enabled when port is open!
 */
class DemoReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit DemoReader(QIODevice* device, QObject* parent = 0);

    QWidget* settingsWidget();
    unsigned numChannels() const;
    void enable(bool enabled = true);

public slots:
    void pause(bool);
    void setNumChannels(unsigned value);

private:
    DemoReaderSettings _settingsWidget;

    bool paused;
    unsigned _numChannels;
    QTimer timer;
    int count;

private slots:
    void demoTimerTimeout();
    void onNumChannelsChanged(unsigned value);
};

#endif // DEMOREADER_H
