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

#include "utils.h"
#include "setting_defines.h"

#include "asciireadersettings.h"
#include "ui_asciireadersettings.h"

#include <QtDebug>

AsciiReaderSettings::AsciiReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AsciiReaderSettings)
{
    ui->setupUi(this);

    // Note: if directly connected we get a runtime warning on incompatible signal arguments
    connect(ui->spNumOfChannels, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                emit numOfChannelsChanged(value);
            });
}

AsciiReaderSettings::~AsciiReaderSettings()
{
    delete ui;
}

unsigned AsciiReaderSettings::numOfChannels()
{
    return ui->spNumOfChannels->value();
}

void AsciiReaderSettings::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_ASCII);

    // save number of channels setting
    QString numOfChannelsSetting = QString::number(numOfChannels());
    if (numOfChannelsSetting == "0") numOfChannelsSetting = "auto";
    settings->setValue(SG_ASCII_NumOfChannels, numOfChannelsSetting);

    settings->endGroup();
}

void AsciiReaderSettings::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_ASCII);

    // load number of channels
    QString numOfChannelsSetting =
        settings->value(SG_ASCII_NumOfChannels, numOfChannels()).toString();

    if (numOfChannelsSetting == "auto")
    {
        ui->spNumOfChannels->setValue(0);
    }
    else
    {
        bool ok;
        int nc = numOfChannelsSetting.toInt(&ok);
        if (ok)
        {
            ui->spNumOfChannels->setValue(nc);
        }
    }

    settings->endGroup();
}
