/*
  Copyright © 2021 Hasan Yavuz Özderya

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

#include "numberformatbox.h"
#include "ui_numberformatbox.h"

NumberFormatBox::NumberFormatBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NumberFormatBox)
{
    ui->setupUi(this);

    // setup buttons
    buttonGroup.addButton(ui->rbUint8,  NumberFormat_uint8);
    buttonGroup.addButton(ui->rbUint16, NumberFormat_uint16);
    buttonGroup.addButton(ui->rbUint32, NumberFormat_uint32);
    buttonGroup.addButton(ui->rbInt8,   NumberFormat_int8);
    buttonGroup.addButton(ui->rbInt16,  NumberFormat_int16);
    buttonGroup.addButton(ui->rbInt32,  NumberFormat_int32);
    buttonGroup.addButton(ui->rbFloat,  NumberFormat_float);
    buttonGroup.addButton(ui->rbDouble,  NumberFormat_double);

    QObject::connect(
        &buttonGroup, SIGNAL(buttonToggled(int, bool)),
        this, SLOT(onButtonToggled(int, bool)));
}

NumberFormatBox::~NumberFormatBox()
{
    delete ui;
}

void NumberFormatBox::onButtonToggled(int numberFormatId, bool checked)
{
    if (checked) emit selectionChanged((NumberFormat) numberFormatId);
}

NumberFormat NumberFormatBox::currentSelection()
{
    return (NumberFormat) buttonGroup.checkedId();
}

void NumberFormatBox::setSelection(NumberFormat nf)
{
    buttonGroup.button(nf)->setChecked(true);
}
