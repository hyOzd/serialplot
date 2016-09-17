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

#include "binarystreamreadersettings.h"
#include "ui_binarystreamreadersettings.h"

#include "utils.h"
#include "setting_defines.h"

BinaryStreamReaderSettings::BinaryStreamReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BinaryStreamReaderSettings)
{
    ui->setupUi(this);

    // Note: if directly connected we get a runtime warning on incompatible signal arguments
    connect(ui->spNumOfChannels, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                emit numOfChannelsChanged(value);
            });

    connect(ui->nfBox, SIGNAL(selectionChanged(NumberFormat)),
            this, SIGNAL(numberFormatChanged(NumberFormat)));

    connect(ui->pbSkipByte, SIGNAL(clicked()), this, SIGNAL(skipByteRequested()));
    connect(ui->pbSkipSample, SIGNAL(clicked()), this, SIGNAL(skipSampleRequested()));
}

BinaryStreamReaderSettings::~BinaryStreamReaderSettings()
{
    delete ui;
}

unsigned BinaryStreamReaderSettings::numOfChannels()
{
    return ui->spNumOfChannels->value();
}

NumberFormat BinaryStreamReaderSettings::numberFormat()
{
    return ui->nfBox->currentSelection();
}

Endianness BinaryStreamReaderSettings::endianness()
{
    return ui->endiBox->currentSelection();
}

void BinaryStreamReaderSettings::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Binary);
    settings->setValue(SG_Binary_NumOfChannels, numOfChannels());
    settings->setValue(SG_Binary_NumberFormat, numberFormatToStr(numberFormat()));
    settings->setValue(SG_Binary_Endianness,
                       endianness() == LittleEndian ? "little" : "big");
    settings->endGroup();
}

void BinaryStreamReaderSettings::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Binary);

    // load number of channels
    ui->spNumOfChannels->setValue(
        settings->value(SG_Binary_NumOfChannels, numOfChannels()).toInt());

    // load number format
    NumberFormat nfSetting =
        strToNumberFormat(settings->value(SG_Binary_NumberFormat,
                                          QString()).toString());
    if (nfSetting == NumberFormat_INVALID) nfSetting = numberFormat();
    ui->nfBox->setSelection(nfSetting);

    // load endianness
    QString endiannessSetting =
        settings->value(SG_Binary_Endianness, QString()).toString();
    if (endiannessSetting == "little")
    {
        ui->endiBox->setSelection(LittleEndian);
    }
    else if (endiannessSetting == "big")
    {
        ui->endiBox->setSelection(BigEndian);
    } // else don't change

    settings->endGroup();
}
