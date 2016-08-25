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

#include <QVariant>

#include <math.h>

#include "plotcontrolpanel.h"
#include "ui_plotcontrolpanel.h"


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

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    // connect signals
    connect(ui->spNumOfSamples, SIGNAL(valueChanged(int)),
            this, SIGNAL(numOfSamplesChanged(int)));

    connect(ui->cbAutoScale, &QCheckBox::toggled,
            this, &PlotControlPanel::onAutoScaleChecked);

    connect(ui->spYmax, SIGNAL(valueChanged(double)),
            this, SLOT(onYScaleChanged()));

    connect(ui->spYmin, SIGNAL(valueChanged(double)),
            this, SLOT(onYScaleChanged()));

    connect(ui->cbMultiPlot, &QCheckBox::toggled,
            this, &PlotControlPanel::multiPlotChanged);

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

void PlotControlPanel::onAutoScaleChecked(bool checked)
{
    if (checked)
    {
        // ui->plot->setAxis(true);
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

        // ui->plot->setAxis(false,  ui->spYmin->value(), ui->spYmax->value());
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

void PlotControlPanel::setChannelNamesModel(QAbstractItemModel * model)
{
    ui->lvChannelNames->setModel(model);
}

bool PlotControlPanel::multiPlot()
{
    return ui->cbMultiPlot->isChecked();
}
