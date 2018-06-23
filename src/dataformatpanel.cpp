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

#include "dataformatpanel.h"
#include "ui_dataformatpanel.h"

#include <QRadioButton>
#include <QtDebug>

#include "utils.h"
#include "setting_defines.h"

DataFormatPanel::DataFormatPanel(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataFormatPanel),
    bsReader(port, this),
    asciiReader(port, this),
    framedReader(port, this),
    demoReader(port, this)
{
    ui->setupUi(this);

    serialPort = port;
    paused = false;
    readerBeforeDemo = nullptr;

    // initalize default reader
    currentReader = &bsReader;
    bsReader.enable();
    ui->rbBinary->setChecked(true);
    ui->horizontalLayout->addWidget(bsReader.settingsWidget(), 1);

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
}

DataFormatPanel::~DataFormatPanel()
{
    delete ui;
}

unsigned DataFormatPanel::numChannels() const
{
    return currentReader->numChannels();
}

Source* DataFormatPanel::activeSource()
{
    return currentReader;
}

void DataFormatPanel::pause(bool enabled)
{
    paused = enabled;
    currentReader->pause(enabled);
    demoReader.pause(enabled);
}

void DataFormatPanel::enableDemo(bool demoEnabled)
{
    if (demoEnabled)
    {
        readerBeforeDemo = currentReader;
        demoReader.setNumChannels(readerBeforeDemo->numChannels());
        selectReader(&demoReader);
    }
    else
    {
        Q_ASSERT(readerBeforeDemo != nullptr);
        selectReader(readerBeforeDemo);
    }

    // disable/enable reader selection buttons during/after demo
    ui->rbAscii->setDisabled(demoEnabled);
    ui->rbBinary->setDisabled(demoEnabled);
    ui->rbFramed->setDisabled(demoEnabled);
}

bool DataFormatPanel::isDemoEnabled() const
{
    return currentReader == &demoReader;
}

void DataFormatPanel::selectReader(AbstractReader* reader)
{
    currentReader->enable(false);
    reader->enable();

    // re-connect signals
    disconnect(currentReader, 0, this, 0);

    // switch the settings widget
    ui->horizontalLayout->removeWidget(currentReader->settingsWidget());
    currentReader->settingsWidget()->hide();
    ui->horizontalLayout->addWidget(reader->settingsWidget(), 1);
    reader->settingsWidget()->show();

    reader->pause(paused);

    currentReader = reader;
    emit sourceChanged(currentReader);
}

void DataFormatPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_DataFormat);

    // save selected data format (current reader)
    QString format;
    AbstractReader* selectedReader = isDemoEnabled() ? readerBeforeDemo : currentReader;
    if (selectedReader == &bsReader)
    {
        format = "binary";
    }
    else if (selectedReader == &asciiReader)
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
    asciiReader.saveSettings(settings);
    framedReader.saveSettings(settings);
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
    asciiReader.loadSettings(settings);
    framedReader.loadSettings(settings);
}
