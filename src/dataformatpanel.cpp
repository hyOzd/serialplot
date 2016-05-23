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

#include "dataformatpanel.h"
#include "ui_dataformatpanel.h"

#include <QRadioButton>
#include <QtEndian>
#include <QtDebug>

#include "utils.h"
#include "floatswap.h"

DataFormatPanel::DataFormatPanel(QSerialPort* port,
                                 ChannelManager* channelMan,
                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataFormatPanel),
    bsReader(port, channelMan),
    asciiReader(port, channelMan)
{
    ui->setupUi(this);

    serialPort = port;
    _channelMan = channelMan;
    paused = false;

    // initalize default reader
    currentReader = &bsReader;
    bsReader.enable();
    ui->rbBinary->setChecked(true);
    ui->horizontalLayout->addWidget(bsReader.settingsWidget(), 1);
    connect(&bsReader, SIGNAL(dataAdded()), this, SIGNAL(dataAdded()));
    connect(&bsReader, SIGNAL(numOfChannelsChanged(unsigned)),
            this, SIGNAL(numOfChannelsChanged(unsigned)));

    // initalize reader selection buttons
    connect(ui->rbBinary, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) selectReader(&bsReader);
            });

    connect(ui->rbAscii, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) selectReader(&asciiReader);
            });

    // Init sps (sample per second) counter
    // sampleCount = 0;
    // QObject::connect(&spsTimer, &QTimer::timeout,
    //                  this, &DataFormatPanel::spsTimerTimeout);
    // spsTimer.start(SPS_UPDATE_TIMEOUT * 1000);

    // Init demo mode
    demoCount = 0;
    demoTimer.setInterval(100);
    QObject::connect(&demoTimer, &QTimer::timeout,
                     this, &DataFormatPanel::demoTimerTimeout);
}

DataFormatPanel::~DataFormatPanel()
{
    delete ui;
}

// TODO: remove
bool DataFormatPanel::skipByteEnabled()
{
    return false;
}

unsigned DataFormatPanel::numOfChannels()
{
    return currentReader->numOfChannels();
}

// TODO: remove
void DataFormatPanel::requestSkipByte()
{
    skipByteRequested = true;
}

void DataFormatPanel::pause(bool enabled)
{
    currentReader->pause(enabled);
}

void DataFormatPanel::enableDemo(bool enabled)
{
    if (enabled)
    {
        demoTimer.start();
    }
    else
    {
        demoTimer.stop();
    }
}

void DataFormatPanel::spsTimerTimeout()
{
    // unsigned currentSps = _samplesPerSecond;
    // _samplesPerSecond = (sampleCount/_numOfChannels)/SPS_UPDATE_TIMEOUT;
    // if (currentSps != _samplesPerSecond)
    // {
    //     emit samplesPerSecondChanged(_samplesPerSecond);
    // }
    // sampleCount = 0;
}

void DataFormatPanel::demoTimerTimeout()
{
    const double period = 100;
    demoCount++;
    if (demoCount >= 100) demoCount = 0;

    if (!paused)
    {
        for (unsigned ci = 0; ci < currentReader->numOfChannels(); ci++)
        {
            // we are calculating the fourier components of square wave
            double value = 4*sin(2*M_PI*double((ci+1)*demoCount)/period)/((2*(ci+1))*M_PI);
            addChannelData(ci, &value, 1);
        }
        emit dataAdded();
    }
}

void DataFormatPanel::addChannelData(unsigned int channel,
                                     double* data, unsigned size)
{
    _channelMan->addChannelData(channel, data, size);
    sampleCount += size;
}

void DataFormatPanel::selectReader(AbstractReader* reader)
{
    currentReader->enable(false);
    reader->enable();

    // re-connect signals
    disconnect(currentReader, 0, this, 0);
    connect(reader, SIGNAL(dataAdded()), this, SIGNAL(dataAdded()));
    connect(reader, SIGNAL(numOfChannelsChanged(unsigned)),
            this, SIGNAL(numOfChannelsChanged(unsigned)));

    // switch the settings widget
    ui->horizontalLayout->removeWidget(currentReader->settingsWidget());
    currentReader->settingsWidget()->hide();
    ui->horizontalLayout->addWidget(reader->settingsWidget(), 1);
    reader->settingsWidget()->show();

    currentReader = reader;
}
