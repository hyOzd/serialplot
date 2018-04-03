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
    virtual void enable(bool enabled = true) = 0;

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
    virtual void pause(bool) = 0;

protected:
    QIODevice* _device;
};

#endif // ABSTRACTREADER_H
