/*
  Copyright © 2020 Hasan Yavuz Özderya

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

#include <QRegularExpressionValidator>
#include <QRegularExpression>

#include "utils.h"
#include "defines.h"
#include "setting_defines.h"

#include "asciireadersettings.h"
#include "ui_asciireadersettings.h"

AsciiReaderSettings::AsciiReaderSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AsciiReaderSettings)
{
    ui->setupUi(this);

    auto validator = new QRegularExpressionValidator(QRegularExpression("[^\\d]?"), this);
    ui->leDelimiter->setValidator(validator);

    ui->spNumOfChannels->setMaximum(MAX_NUM_CHANNELS);

    delimiterButtons.addButton(ui->rbComma);
    delimiterButtons.addButton(ui->rbSpace);
    delimiterButtons.addButton(ui->rbTab);
    delimiterButtons.addButton(ui->rbOtherDelimiter);

    filterButtons.addButton(ui->rbFilterDisabled, (int) FilterMode::disabled);
    filterButtons.addButton(ui->rbFilterInclude, (int) FilterMode::include);
    filterButtons.addButton(ui->rbFilterExclude, (int) FilterMode::exclude);

    // delimiter buttons signals
    connect(ui->rbComma, &QAbstractButton::toggled,
            this, &AsciiReaderSettings::delimiterToggled);
    connect(ui->rbSpace, &QAbstractButton::toggled,
            this, &AsciiReaderSettings::delimiterToggled);
    connect(ui->rbTab, &QAbstractButton::toggled,
            this, &AsciiReaderSettings::delimiterToggled);
    connect(ui->rbOtherDelimiter, &QAbstractButton::toggled,
            this, &AsciiReaderSettings::delimiterToggled);
    connect(ui->leDelimiter, &QLineEdit::textChanged,
            this, &AsciiReaderSettings::customDelimiterChanged);

    // filter buttons signals
    connect(ui->rbFilterDisabled, &QAbstractButton::toggled,
            [this] (bool checked)
            {
                ui->leFilterPrefix->setDisabled(checked);
            });

    connect(&filterButtons,
            SELECT<int, bool>::OVERLOAD_OF(&QButtonGroup::idToggled),
            [this](int id, bool checked)
            {
                emit filterChanged(static_cast<FilterMode>(id), ui->leFilterPrefix->text());
            });

    connect(ui->leFilterPrefix, &QLineEdit::textChanged,
            [this] (QString text)
            {
                emit filterChanged(filterMode(), text);
            });

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

unsigned AsciiReaderSettings::numOfChannels() const
{
    return ui->spNumOfChannels->value();
}

AsciiReaderSettings::FilterMode AsciiReaderSettings::filterMode() const
{
    return static_cast<FilterMode>(filterButtons.checkedId());
}

QChar AsciiReaderSettings::delimiter() const
{
    if (ui->rbComma->isChecked())
    {
        return QChar(',');
    }
    else if (ui->rbSpace->isChecked())
    {
        return QChar(' ');
    }
    else if (ui->rbTab->isChecked())
    {
        return QChar('\t');
    }
    else                        // rbOther
    {
        auto t = ui->leDelimiter->text();
        return t.isEmpty() ? QChar() : t.at(0);
    }
}

void AsciiReaderSettings::delimiterToggled(bool checked)
{
    if (!checked) return;

    auto d = delimiter();
    if (!d.isNull())
    {
        emit delimiterChanged(d);
    }
}

void AsciiReaderSettings::customDelimiterChanged(const QString text)
{
    if (ui->rbOtherDelimiter->isChecked())
    {
        if (!text.isEmpty()) emit delimiterChanged(text.at(0));
    }
}

void AsciiReaderSettings::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_ASCII);

    // save number of channels setting
    QString numOfChannelsSetting = QString::number(numOfChannels());
    if (numOfChannelsSetting == "0") numOfChannelsSetting = "auto";
    settings->setValue(SG_ASCII_NumOfChannels, numOfChannelsSetting);

    // save delimiter
    QString delimiterS;
    if (ui->rbOtherDelimiter->isChecked())
    {
        delimiterS = "other";
    }
    else if (ui->rbTab->isChecked())
    {
        // Note: \t is not correctly loaded
        delimiterS = "TAB";
    }
    else
    {
        delimiterS = delimiter();
    }

    settings->setValue(SG_ASCII_Delimiter, delimiterS);
    settings->setValue(SG_ASCII_CustomDelimiter, ui->leDelimiter->text());

    // save filter
    QString filterModeS;
    switch (filterMode())
    {
        case FilterMode::disabled:
            filterModeS = "disabled";
            break;
        case FilterMode::include:
            filterModeS = "include";
            break;
        case FilterMode::exclude:
            filterModeS = "exclude";
            break;
    }
    settings->setValue(SG_ASCII_FilterMode, filterModeS);
    settings->setValue(SG_ASCII_FilterPrefix, ui->leFilterPrefix->text());

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

    // load delimiter
    auto delimiterS = settings->value(SG_ASCII_Delimiter, delimiter()).toString();
    auto customDelimiter = settings->value(SG_ASCII_CustomDelimiter, delimiter()).toString();
    if (!customDelimiter.isEmpty()) ui->leDelimiter->setText(customDelimiter);
    if (delimiterS == ",")
    {
        ui->rbComma->setChecked(true);
    }
    else if (delimiterS == " ")
    {
        ui->rbSpace->setChecked(true);
    }
    else if (delimiterS == "TAB")
    {
        ui->rbTab->setChecked(true);
    }
    else
    {
        ui->rbOtherDelimiter->setChecked(true);
    }

    // load filter
    FilterMode filterModeE = filterMode();
    auto filterModeS = settings->value(SG_ASCII_FilterMode, "");
    if (filterModeS == "disabled")
    {
        filterModeE = FilterMode::disabled;
    }
    else if (filterModeS == "include")
    {
        filterModeE = FilterMode::include;
    }
    else if (filterModeS == "exclude")
    {
        filterModeE = FilterMode::exclude;
    }
    filterButtons.button(static_cast<int>(filterModeE))->setChecked(true);

    auto filterPrefixS = settings->value(SG_ASCII_FilterPrefix, ui->leFilterPrefix->text()).toString();
    ui->leFilterPrefix->setText(filterPrefixS);

    settings->endGroup();
}
