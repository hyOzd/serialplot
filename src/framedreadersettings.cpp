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

#include <QButtonGroup>

#include "utils.h"
#include "defines.h"
#include "setting_defines.h"
#include "framedreadersettings.h"
#include "ui_framedreadersettings.h"

FramedReaderSettings::FramedReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FramedReaderSettings)
{
    ui->setupUi(this);

    ui->leSyncWord->setMode(false); // hex mode
    ui->leSyncWord->setText("AA BB");
    ui->spNumOfChannels->setMaximum(MAX_NUM_CHANNELS);

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

    // add frame size selection buttons to same group
    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(ui->rbFixedSize);
    group->addButton(ui->rbSizeByte);
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

void FramedReaderSettings::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_CustomFrame);
    settings->setValue(SG_CustomFrame_NumOfChannels, numOfChannels());
    settings->setValue(SG_CustomFrame_NumberFormat, numberFormatToStr(numberFormat()));
    settings->setValue(SG_CustomFrame_Endianness,
                       endianness() == LittleEndian ? "little" : "big");
    settings->setValue(SG_CustomFrame_FrameStart, ui->leSyncWord->text());
    settings->setValue(SG_CustomFrame_FixedSize, ui->rbFixedSize->isChecked());
    settings->setValue(SG_CustomFrame_FrameSize, ui->spSize->value());
    settings->setValue(SG_CustomFrame_Checksum, ui->cbChecksum->isChecked());
    settings->setValue(SG_CustomFrame_DebugMode, ui->cbDebugMode->isChecked());
    settings->endGroup();
}

void FramedReaderSettings::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_CustomFrame);

    // load number of channels
    ui->spNumOfChannels->setValue(
        settings->value(SG_CustomFrame_NumOfChannels, numOfChannels()).toInt());

    // load number format
    NumberFormat nfSetting =
        strToNumberFormat(settings->value(SG_CustomFrame_NumberFormat,
                                          QString()).toString());
    if (nfSetting == NumberFormat_INVALID) nfSetting = numberFormat();
    ui->nfBox->setSelection(nfSetting);

    // load endianness
    QString endiannessSetting =
        settings->value(SG_CustomFrame_Endianness, QString()).toString();
    if (endiannessSetting == "little")
    {
        ui->endiBox->setSelection(LittleEndian);
    }
    else if (endiannessSetting == "big")
    {
        ui->endiBox->setSelection(BigEndian);
    } // else don't change

    // load frame start
    QString frameStartSetting =
        settings->value(SG_CustomFrame_FrameStart, ui->leSyncWord->text()).toString();
    auto validator = ui->leSyncWord->validator();
    validator->fixup(frameStartSetting);
    int pos = 0;
    if (validator->validate(frameStartSetting, pos) != QValidator::Invalid)
    {
        ui->leSyncWord->setText(frameStartSetting);
    }

    // load frame size
    ui->spSize->setValue(
        settings->value(SG_CustomFrame_FrameSize, ui->spSize->value()).toInt());
    ui->rbFixedSize->setChecked(
        settings->value(SG_CustomFrame_FixedSize, ui->rbFixedSize->isChecked()).toBool());
    // Note: need to emit here, no signal will be emitted since 'rbFixedSize' is already unchecked
    emit frameSizeChanged(frameSize());

    // load checksum
    ui->cbChecksum->setChecked(
        settings->value(SG_CustomFrame_Checksum, ui->cbChecksum->isChecked()).toBool());

    // load debug mode
    ui->cbDebugMode->setChecked(
        settings->value(SG_CustomFrame_DebugMode, ui->cbDebugMode->isChecked()).toBool());

    settings->endGroup();
}
