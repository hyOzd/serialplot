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

#include "demoreadersettings.h"
#include "ui_demoreadersettings.h"

#include "utils.h"
#include "defines.h"

DemoReaderSettings::DemoReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DemoReaderSettings)
{
    ui->setupUi(this);

    ui->spNumChannels->setMaximum(MAX_NUM_CHANNELS);

    connect(ui->spNumChannels, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                emit numChannelsChanged(value);
            });
}

DemoReaderSettings::~DemoReaderSettings()
{
    delete ui;
}

unsigned DemoReaderSettings::numChannels() const
{
    return ui->spNumChannels->value();
}

void DemoReaderSettings::setNumChannels(unsigned value)
{
    ui->spNumChannels->setValue(value);
}
