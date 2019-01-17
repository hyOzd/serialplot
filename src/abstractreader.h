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

#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include <QObject>
#include <QIODevice>
#include <QWidget>
#include <QTimer>

#include "source.h"

/**
 * All reader classes must inherit this class.
 */
class AbstractReader : public QObject, public Source
{
    Q_OBJECT
public:
    explicit AbstractReader(QIODevice* device, QObject* parent = 0);

    /**
     * Returns a widget to be shown in data format panel when reader
     * is selected.
     */
    virtual QWidget* settingsWidget() = 0;

    /// Reader should only read when enabled. Default state should be
    /// 'disabled'.
    virtual void enable(bool enabled = true);

    /// None of the current readers support X channel at the moment
    bool hasX() const final { return false; };

signals:
    // TODO: should we keep this?
    void numOfChannelsChanged(unsigned);

public slots:
    /**
     * Pauses the reading.
     *
     * Reader should actually continue reading to keep the
     * synchronization but shouldn't commit data.
     */
    void pause(bool enabled);

protected:
    /// Reader should read from this device in `readData()` function.
    QIODevice* _device;

    /// Reader should check this variable to determine if reading is
    /// paused in `readData()`
    bool paused;

    /**
     * Called when `readyRead` is signaled by the device. This is
     * where the implementors should read the data and return the
     * exact number of bytes read from the device.
     */
    virtual unsigned readData() = 0;

private:
    unsigned numBytesRead;

private slots:
    void onDataReady();
};

#endif // ABSTRACTREADER_H
