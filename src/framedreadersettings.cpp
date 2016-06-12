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
#include "framedreadersettings.h"
#include "ui_framedreadersettings.h"

FramedReaderSettings::FramedReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FramedReaderSettings)
{
    ui->setupUi(this);

    ui->leSyncWord->setMode(false); // hex mode
    ui->leSyncWord->setText("AA BB");

    connect(ui->cbChecksum, &QCheckBox::toggled,
            [this](bool enabled)
            {
                emit checksumChanged(enabled);
            });

    connect(ui->cbDebugMode, &QCheckBox::toggled,
            this, &FramedReaderSettings::debugModeChanged);

    connect(ui->rbFixedSize, &QRadioButton::toggled,
            ui->spSize, &QWidget::setEnabled);

    connect(ui->rbFixedSize, &QRadioButton::toggled,
            [this](bool checked)
            {
                emit frameSizeChanged(frameSize());
            });

    // Note: if directly connected we get a runtime warning on incompatible signal arguments
    connect(ui->spSize, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                emit frameSizeChanged(value);
            });

    connect(ui->spNumOfChannels, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                emit numOfChannelsChanged(value);
            });

    connect(ui->leSyncWord, &QLineEdit::textChanged,
            this, &FramedReaderSettings::onSyncWordEdited);

    connect(ui->nfBox, SIGNAL(selectionChanged(NumberFormat)),
            this, SIGNAL(numberFormatChanged(NumberFormat)));
}

FramedReaderSettings::~FramedReaderSettings()
{
    delete ui;
}

void FramedReaderSettings::showMessage(QString message, bool error)
{
    ui->lMessage->setText(message);
    if (error)
    {
        ui->lMessage->setStyleSheet("color: red;");
    }
    else
    {
        ui->lMessage->setStyleSheet("");
    }
}

unsigned FramedReaderSettings::numOfChannels()
{
    return ui->spNumOfChannels->value();
}

NumberFormat FramedReaderSettings::numberFormat()
{
    return ui->nfBox->currentSelection();
}

Endianness FramedReaderSettings::endianness()
{
    return ui->endiBox->currentSelection();
}

QByteArray FramedReaderSettings::syncWord()
{
    QString text = ui->leSyncWord->text().remove(' ');

    // check if nibble is missing
    if (text.size() % 2 == 1)
    {
        // TODO: remove this warning
        return QByteArray();
    }
    else
    {
        return QByteArray::fromHex(text.toLatin1());
    }
}

void FramedReaderSettings::onSyncWordEdited()
{
    // TODO: emit with a delay so that error message doesn't flash!
    emit syncWordChanged(syncWord());
}

unsigned FramedReaderSettings::frameSize()
{
    if (ui->rbFixedSize->isChecked())
    {
        return ui->spSize->value();
    }
    else
    {
        return 0; // frame byte is enabled
    }
}

bool FramedReaderSettings::isChecksumEnabled()
{
    return ui->cbChecksum->isChecked();
}

bool FramedReaderSettings::isDebugModeEnabled()
{
    return ui->cbDebugMode->isChecked();
}
