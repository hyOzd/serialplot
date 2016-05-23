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

#ifndef DATAFORMATPANEL_H
#define DATAFORMATPANEL_H

#include <QWidget>
#include <QButtonGroup>
#include <QTimer>
#include <QSerialPort>
#include <QList>
#include <QtGlobal>

#include "framebuffer.h"
#include "channelmanager.h"
#include "binarystreamreader.h"
#include "asciireader.h"

namespace Ui {
class DataFormatPanel;
}

class DataFormatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DataFormatPanel(QSerialPort* port,
                             ChannelManager* channelMan,
                             QWidget *parent = 0);
    ~DataFormatPanel();

    unsigned numOfChannels();
    unsigned samplesPerSecond();
    bool skipByteEnabled(void); // true for binary formats

public slots:
    // during next read operation reader will skip 1 byte,
    // requests are not accumulated
    void requestSkipByte();
    void pause(bool);
    void enableDemo(bool); // demo shouldn't be enabled when port is open

signals:
    void numOfChannelsChanged(unsigned);
    void samplesPerSecondChanged(unsigned);
    void skipByteEnabledChanged(bool); // remove
    void dataAdded();

private:
    Ui::DataFormatPanel *ui;

    QSerialPort* serialPort;
    ChannelManager* _channelMan;

    BinaryStreamReader bsReader;
    AsciiReader asciiReader;
    /// Currently selected reader
    AbstractReader* currentReader;
    /// Disable current reader and enable a another one
    void selectReader(AbstractReader* reader);

    bool skipByteRequested; // remove
    bool paused; // remove

    const int SPS_UPDATE_TIMEOUT = 1;  // second
    unsigned _samplesPerSecond;
    unsigned int sampleCount;
    QTimer spsTimer;

    // demo
    QTimer demoTimer;
    int demoCount;

    // `data` contains i th channels data
    void addChannelData(unsigned int channel, double* data, unsigned size);

private slots:
    void spsTimerTimeout();
    void demoTimerTimeout();
};

#endif // DATAFORMATPANEL_H
