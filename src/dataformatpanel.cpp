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

#include <QtEndian>
#include <QtDebug>

#include "utils.h"
#include "floatswap.h"

DataFormatPanel::DataFormatPanel(QSerialPort* port,
                                 ChannelManager* channelMan,
                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataFormatPanel)
{
    ui->setupUi(this);

    serialPort = port;
    _channelMan = channelMan;
    paused = false;

    // setup number format buttons
    numberFormatButtons.addButton(ui->rbUint8,  NumberFormat_uint8);
    numberFormatButtons.addButton(ui->rbUint16, NumberFormat_uint16);
    numberFormatButtons.addButton(ui->rbUint32, NumberFormat_uint32);
    numberFormatButtons.addButton(ui->rbInt8,   NumberFormat_int8);
    numberFormatButtons.addButton(ui->rbInt16,  NumberFormat_int16);
    numberFormatButtons.addButton(ui->rbInt32,  NumberFormat_int32);
    numberFormatButtons.addButton(ui->rbFloat,  NumberFormat_float);
    numberFormatButtons.addButton(ui->rbASCII,  NumberFormat_ASCII);

    QObject::connect(
        &numberFormatButtons, SIGNAL(buttonToggled(int, bool)),
        this, SLOT(onNumberFormatButtonToggled(int, bool)));

    // init number format
    selectNumberFormat((NumberFormat) numberFormatButtons.checkedId());

    // setup number of channels spinbox
    QObject::connect(ui->spNumOfChannels,
                     SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
                     this, &DataFormatPanel::onNumOfChannelsSP);

    _numOfChannels = ui->spNumOfChannels->value();

    // Init sps (sample per second) counter
    sampleCount = 0;
    QObject::connect(&spsTimer, &QTimer::timeout,
                     this, &DataFormatPanel::spsTimerTimeout);
    spsTimer.start(SPS_UPDATE_TIMEOUT * 1000);

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

void DataFormatPanel::onNumberFormatButtonToggled(int numberFormatId,
                                                  bool checked)
{
    if (checked) selectNumberFormat((NumberFormat) numberFormatId);
}

void DataFormatPanel::selectNumberFormat(NumberFormat numberFormatId)
{
    numberFormat = numberFormatId;

    switch(numberFormat)
    {
        case NumberFormat_uint8:
            sampleSize = 1;
            readSample = &DataFormatPanel::readSampleAs<quint8>;
            break;
        case NumberFormat_int8:
            sampleSize = 1;
            readSample = &DataFormatPanel::readSampleAs<qint8>;
            break;
        case NumberFormat_uint16:
            sampleSize = 2;
            readSample = &DataFormatPanel::readSampleAs<quint16>;
            break;
        case NumberFormat_int16:
            sampleSize = 2;
            readSample = &DataFormatPanel::readSampleAs<qint16>;
            break;
        case NumberFormat_uint32:
            sampleSize = 4;
            readSample = &DataFormatPanel::readSampleAs<quint32>;
            break;
        case NumberFormat_int32:
            sampleSize = 4;
            readSample = &DataFormatPanel::readSampleAs<qint32>;
            break;
        case NumberFormat_float:
            sampleSize = 4;
            readSample = &DataFormatPanel::readSampleAs<float>;
            break;
        case NumberFormat_ASCII:
            sampleSize = 0;    // these two members should not be used
            readSample = NULL; // in this mode
            break;
    }

    if (numberFormat == NumberFormat_ASCII)
    {
        QObject::disconnect(serialPort, &QSerialPort::readyRead, 0, 0);
        QObject::connect(this->serialPort, &QSerialPort::readyRead,
                         this, &DataFormatPanel::onDataReadyASCII);
    }
    else
    {
        QObject::disconnect(serialPort, &QSerialPort::readyRead, 0, 0);
        QObject::connect(serialPort, &QSerialPort::readyRead,
                         this, &DataFormatPanel::onDataReady);
    }

    emit skipByteEnabledChanged(skipByteEnabled());
}

bool DataFormatPanel::skipByteEnabled()
{
    return numberFormat != NumberFormat_ASCII;
}

unsigned DataFormatPanel::numOfChannels()
{
    return _numOfChannels;
}

void DataFormatPanel::onNumOfChannelsSP(int value)
{
    _numOfChannels = value;
    emit numOfChannelsChanged(value);
}

void DataFormatPanel::requestSkipByte()
{
    skipByteRequested = true;
}

void DataFormatPanel::pause(bool enabled)
{
    paused = enabled;
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
    unsigned currentSps = _samplesPerSecond;
    _samplesPerSecond = sampleCount/SPS_UPDATE_TIMEOUT;
    if (currentSps != _samplesPerSecond)
    {
        emit samplesPerSecondChanged(_samplesPerSecond);
    }
    sampleCount = 0;
}


void DataFormatPanel::demoTimerTimeout()
{
    const double period = 100;
    demoCount++;
    if (demoCount >= 100) demoCount = 0;

    if (!paused)
    {
        for (unsigned ci = 0; ci < _numOfChannels; ci++)
        {
            // we are calculating the fourier components of square wave
            double value = 4*sin(2*M_PI*double((ci+1)*demoCount)/period)/((2*(ci+1))*M_PI);
            addChannelData(ci, &value, 1);
        }
        emit dataAdded();
    }
}

void DataFormatPanel::onDataReady()
{
    // a package is a set of channel data like {CHAN0_SAMPLE, CHAN1_SAMPLE...}
    int packageSize = sampleSize * _numOfChannels;
    int bytesAvailable = serialPort->bytesAvailable();
    int numOfPackagesToRead =
        (bytesAvailable - (bytesAvailable % packageSize)) / packageSize;

    if (paused)
    {
        // read and discard data
        serialPort->read(numOfPackagesToRead*packageSize);
        return;
    }

    if (bytesAvailable > 0 && skipByteRequested)
    {
        serialPort->read(1);
        skipByteRequested = false;
        bytesAvailable--;
    }

    if (bytesAvailable < packageSize) return;

    double* channelSamples = new double[numOfPackagesToRead*_numOfChannels];

    for (int i = 0; i < numOfPackagesToRead; i++)
    {
        for (unsigned int ci = 0; ci < _numOfChannels; ci++)
        {
            // channelSamples[ci].replace(i, (this->*readSample)());
            channelSamples[ci*numOfPackagesToRead+i] = (this->*readSample)();
        }
    }

    for (unsigned int ci = 0; ci < _numOfChannels; ci++)
    {
        addChannelData(ci,
                       channelSamples + ci*numOfPackagesToRead,
                       numOfPackagesToRead);
    }
    emit dataAdded();

    delete channelSamples;
}

void DataFormatPanel::onDataReadyASCII()
{
    while(serialPort->canReadLine())
    {
        QByteArray line = serialPort->readLine();

        // discard data if paused
        if (paused)
        {
            return;
        }

        line = line.trimmed();
        auto separatedValues = line.split(',');

        int numReadChannels; // effective number of channels to read
        if (separatedValues.length() >= int(_numOfChannels))
        {
            numReadChannels = _numOfChannels;
        }
        else // there is missing channel data
        {
            numReadChannels = separatedValues.length();
            qWarning() << "Incoming data is missing data for some channels!";
        }

        // parse read line
        for (int ci = 0; ci < numReadChannels; ci++)
        {
            bool ok;
            double channelSample = separatedValues[ci].toDouble(&ok);
            if (ok)
            {
                addChannelData(ci, &channelSample, 1);
            }
            else
            {
                qWarning() << "Data parsing error for channel: " << ci;
            }
        }
        emit dataAdded();
    }
}

template<typename T> double DataFormatPanel::readSampleAs()
{
    T data;
    serialPort->read((char*) &data, sizeof(data));

    if (ui->rbLittleE->isChecked())
    {
        data = qFromLittleEndian(data);
    }
    else
    {
        data = qFromBigEndian(data);
    }

    return double(data);
}

void DataFormatPanel::addChannelData(unsigned int channel,
                                     double* data, unsigned size)
{
    _channelMan->addChannelData(channel, data, size);
    sampleCount += size;
}
