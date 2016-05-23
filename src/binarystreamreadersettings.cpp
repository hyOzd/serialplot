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
