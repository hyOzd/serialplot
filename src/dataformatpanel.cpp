/*
  Copyright © 2016 Hasan Yavuz Özderya

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
#include <QMap>
#include <QtDebug>

#include "utils.h"
#include "setting_defines.h"
#include "floatswap.h"

DataFormatPanel::DataFormatPanel(QSerialPort* port,
                                 ChannelManager* channelMan,
                                 QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataFormatPanel),
    bsReader(port, channelMan, this),
    asciiReader(port, channelMan, this),
    framedReader(port, channelMan, this),
    demoReader(port, channelMan, this)
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
    connect(&bsReader, SIGNAL(samplesPerSecondChanged(unsigned)),
            this, SIGNAL(samplesPerSecondChanged(unsigned)));

    // initalize reader selection buttons
    connect(ui->rbBinary, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) selectReader(&bsReader);
            });

    connect(ui->rbAscii, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) selectReader(&asciiReader);
            });

    connect(ui->rbFramed, &QRadioButton::toggled, [this](bool checked)
            {
                if (checked) selectReader(&framedReader);
            });

    // re-purpose numofchannels settings from actual reader settings to demo reader
    connect(this, &DataFormatPanel::numOfChannelsChanged,
            &demoReader, &DemoReader::setNumOfChannels);
}

DataFormatPanel::~DataFormatPanel()
{
    delete ui;
}

unsigned DataFormatPanel::numOfChannels()
{
    return currentReader->numOfChannels();
}

void DataFormatPanel::pause(bool enabled)
{
    paused = enabled;
    currentReader->pause(enabled);
    demoReader.pause(enabled);
}

void DataFormatPanel::enableDemo(bool enabled)
{
    if (enabled)
    {
        demoReader.enable();
        connect(&demoReader, &DemoReader::dataAdded,
                this, &DataFormatPanel::dataAdded);
        connect(&demoReader, &DemoReader::samplesPerSecondChanged,
                this, &DataFormatPanel::samplesPerSecondChanged);
    }
    else
    {
        demoReader.enable(false);
        disconnect(&demoReader, 0, this, 0);
    }
}

void DataFormatPanel::addChannelData(unsigned int channel,
                                     double* data, unsigned size)
{
    _channelMan->addChannelData(channel, data, size);
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
    connect(reader, SIGNAL(samplesPerSecondChanged(unsigned)),
            this, SIGNAL(samplesPerSecondChanged(unsigned)));

    // switch the settings widget
    ui->horizontalLayout->removeWidget(currentReader->settingsWidget());
    currentReader->settingsWidget()->hide();
    ui->horizontalLayout->addWidget(reader->settingsWidget(), 1);
    reader->settingsWidget()->show();

    // notify if number of channels is different
    if (currentReader->numOfChannels() != reader->numOfChannels())
    {
        emit numOfChannelsChanged(reader->numOfChannels());
    }

    // pause
    reader->pause(paused);

    currentReader = reader;
}

void DataFormatPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_DataFormat);

    // save selected format
    QString format;
    if (currentReader == &bsReader)
    {
        format = "binary";
    }
    else if (currentReader == &asciiReader)
    {
        format = "ascii";
    }
    else // framed reader
    {
        format = "custom";
    }
    settings->setValue(SG_DataFormat_Format, format);

    settings->endGroup();

    // save reader settings
    bsReader.saveSettings(settings);
}

void DataFormatPanel::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_DataFormat);

    // load selected format
    QString format = settings->value(
        SG_DataFormat_Format, QString()).toString();

    if (format == "binary")
    {
        selectReader(&bsReader);
        ui->rbBinary->setChecked(true);
    }
    else if (format == "ascii")
    {
        selectReader(&asciiReader);
        ui->rbAscii->setChecked(true);
    }
    else if (format == "custom")
    {
        selectReader(&framedReader);
        ui->rbFramed->setChecked(true);
    } // else current selection stays

    settings->endGroup();

    // load reader settings
    bsReader.loadSettings(settings);
}
