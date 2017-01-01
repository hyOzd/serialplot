/*
  Copyright © 2017 Hasan Yavuz Özderya

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

#include <QVariant>
#include <QMessageBox>
#include <QCheckBox>

#include <math.h>

#include "color_selector.hpp"
#include "plotcontrolpanel.h"
#include "ui_plotcontrolpanel.h"
#include "setting_defines.h"

/// Confirm if #samples is being set to a value greater than this
const int NUMSAMPLES_CONFIRM_AT = 10000;

/// Used for scale range selection combobox
struct Range
{
    double rmin;
    double rmax;
};

Q_DECLARE_METATYPE(Range);

PlotControlPanel::PlotControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotControlPanel)
{
    ui->setupUi(this);

    warnNumOfSamples = true;    // TODO: load from settings
    _numOfSamples = ui->spNumOfSamples->value();

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    // connect signals
    connect(ui->spNumOfSamples, SIGNAL(valueChanged(int)),
            this, SLOT(onNumOfSamples(int)));

    connect(ui->cbAutoScale, &QCheckBox::toggled,
            this, &PlotControlPanel::onAutoScaleChecked);

    connect(ui->spYmax, SIGNAL(valueChanged(double)),
            this, SLOT(onYScaleChanged()));

    connect(ui->spYmin, SIGNAL(valueChanged(double)),
            this, SLOT(onYScaleChanged()));

    // init scale range preset list
    for (int nbits = 8; nbits <= 24; nbits++) // signed binary formats
    {
        int rmax = pow(2, nbits-1)-1;
        int rmin = -rmax-1;
        Range r = {double(rmin),  double(rmax)};
        ui->cbRangePresets->addItem(
            QString().sprintf("Signed %d bits %d to +%d", nbits, rmin, rmax),
            QVariant::fromValue(r));
    }
    for (int nbits = 8; nbits <= 24; nbits++) // unsigned binary formats
    {
        int rmax = pow(2, nbits)-1;
        ui->cbRangePresets->addItem(
            QString().sprintf("Unsigned %d bits %d to +%d", nbits, 0, rmax),
            QVariant::fromValue(Range{0, double(rmax)}));
    }
    ui->cbRangePresets->addItem("-1 to +1", QVariant::fromValue(Range{-1, +1}));
    ui->cbRangePresets->addItem("0 to +1", QVariant::fromValue(Range{0, +1}));
    ui->cbRangePresets->addItem("-100 to +100", QVariant::fromValue(Range{-100, +100}));
    ui->cbRangePresets->addItem("0 to +100", QVariant::fromValue(Range{0, +100}));

    QObject::connect(ui->cbRangePresets, SIGNAL(activated(int)),
                     this, SLOT(onRangeSelected()));
}

PlotControlPanel::~PlotControlPanel()
{
    delete ui;
}

unsigned PlotControlPanel::numOfSamples()
{
    return ui->spNumOfSamples->value();
}

void PlotControlPanel::onNumOfSamples(int value)
{
    if (warnNumOfSamples && value > NUMSAMPLES_CONFIRM_AT)
    {
        // ask confirmation
        if (!askNSConfirmation(value))
        {
            // revert to old value
            disconnect(ui->spNumOfSamples, SIGNAL(valueChanged(int)),
                       this, SLOT(onNumOfSamples(int)));

            ui->spNumOfSamples->setValue(_numOfSamples);

            connect(ui->spNumOfSamples, SIGNAL(valueChanged(int)),
                    this, SLOT(onNumOfSamples(int)));

            return;
        }
    }

    _numOfSamples = value;
    emit numOfSamplesChanged(value);
}

bool PlotControlPanel::askNSConfirmation(int value)
{
    auto text = tr("Setting number of samples to a too big value "
                   "(>%1) can seriously impact the performance of "
                   "the application and cause freezes. Are you sure you "
                   "want to change the number of samples to %2?")
        .arg(QString::number(NUMSAMPLES_CONFIRM_AT), QString::number(value));

    // TODO: parent the mainwindow
    QMessageBox mb(QMessageBox::Warning,
                   tr("Confirm Number of Samples"),
                   text,
                   QMessageBox::Apply | QMessageBox::Cancel,
                   this);

    auto cb = new QCheckBox("Don't show this again.");
    connect(cb, &QCheckBox::stateChanged, [this](int state)
            {
                warnNumOfSamples = (state == Qt::Unchecked);
            });

    mb.setCheckBox(cb);

    return mb.exec() == QMessageBox::Apply;
}

void PlotControlPanel::onAutoScaleChecked(bool checked)
{
    if (checked)
    {
        ui->lYmin->setEnabled(false);
        ui->lYmax->setEnabled(false);
        ui->spYmin->setEnabled(false);
        ui->spYmax->setEnabled(false);

        emit scaleChanged(true); // autoscale
    }
    else
    {
        ui->lYmin->setEnabled(true);
        ui->lYmax->setEnabled(true);
        ui->spYmin->setEnabled(true);
        ui->spYmax->setEnabled(true);

        emit scaleChanged(false, ui->spYmin->value(), ui->spYmax->value());
    }
}

void PlotControlPanel::onYScaleChanged()
{
    emit scaleChanged(false, ui->spYmin->value(), ui->spYmax->value());
}

bool PlotControlPanel::autoScale()
{
    return ui->cbAutoScale->isChecked();
}

double PlotControlPanel::yMax()
{
    return ui->spYmax->value();
}

double PlotControlPanel::yMin()
{
    return ui->spYmin->value();
}

void PlotControlPanel::onRangeSelected()
{
    Range r = ui->cbRangePresets->currentData().value<Range>();
    ui->spYmin->setValue(r.rmin);
    ui->spYmax->setValue(r.rmax);
    ui->cbAutoScale->setChecked(false);
}

void PlotControlPanel::setChannelInfoModel(ChannelInfoModel* model)
{
    ui->tvChannelInfo->setModel(model);

    // channel color selector
    QObject::connect(ui->tvChannelInfo->selectionModel(), &QItemSelectionModel::currentRowChanged,
                     [this](const QModelIndex &current, const QModelIndex &previous)
                     {
                         auto model = ui->tvChannelInfo->model();
                         QColor color = model->data(current, Qt::ForegroundRole).value<QColor>();
                         ui->colorSelector->setColor(color);
                         // cpicker.setColor(cim.data(current, Qt::ForegroundRole).value<QColor>());
                     });

    QObject::connect(ui->colorSelector, &color_widgets::ColorSelector::colorChanged,
                     [this](QColor color)
                     {
                         auto index = ui->tvChannelInfo->selectionModel()->currentIndex();
                         // index = index.sibling(index.row(), 0);
                         ui->tvChannelInfo->model()->setData(index, color, Qt::ForegroundRole);
                     });
}

void PlotControlPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    settings->setValue(SG_Plot_NumOfSamples, numOfSamples());
    settings->setValue(SG_Plot_AutoScale, autoScale());
    settings->setValue(SG_Plot_YMax, yMax());
    settings->setValue(SG_Plot_YMin, yMin());
    settings->endGroup();
}

void PlotControlPanel::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    ui->spNumOfSamples->setValue(
        settings->value(SG_Plot_NumOfSamples, numOfSamples()).toInt());
    ui->cbAutoScale->setChecked(
        settings->value(SG_Plot_AutoScale, autoScale()).toBool());
    ui->spYmax->setValue(settings->value(SG_Plot_YMax, yMax()).toDouble());
    ui->spYmin->setValue(settings->value(SG_Plot_YMin, yMin()).toDouble());
    settings->endGroup();
}
