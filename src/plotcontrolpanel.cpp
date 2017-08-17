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
    ui(new Ui::PlotControlPanel),
    resetAct(tr("Reset"), this),
    resetNamesAct(tr("Reset Names"), this),
    resetColorsAct(tr("Reset Colors"), this),
    showAllAct(tr("Show All"), this),
    resetMenu(tr("Reset Menu"), this)
{
    ui->setupUi(this);

    warnNumOfSamples = true;    // TODO: load from settings
    _numOfSamples = ui->spNumOfSamples->value();

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spXmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spXmax->setRange((-1) * std::numeric_limits<double>::max(),
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

    connect(ui->cbIndex, &QCheckBox::toggled,
            this, &PlotControlPanel::onIndexChecked);

    connect(ui->spXmax, SIGNAL(valueChanged(double)),
            this, SLOT(onXScaleChanged()));

    connect(ui->spXmin, SIGNAL(valueChanged(double)),
            this, SLOT(onXScaleChanged()));

    connect(ui->spPlotWidth, SIGNAL(valueChanged(int)),
            this, SLOT(onPlotWidthChanged()));

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

    // color selector starts disabled until a channel is selected
    ui->colorSelector->setColor(QColor(0,0,0,0));
    ui->colorSelector->setDisplayMode(color_widgets::ColorPreview::AllAlpha);
    ui->colorSelector->setDisabled(true);

    // reset button
    resetMenu.addAction(&resetNamesAct);
    resetMenu.addAction(&resetColorsAct);
    resetMenu.addAction(&showAllAct);
    resetAct.setMenu(&resetMenu);
    ui->tbReset->setDefaultAction(&resetAct);
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

        emit yScaleChanged(true); // autoscale
    }
    else
    {
        ui->lYmin->setEnabled(true);
        ui->lYmax->setEnabled(true);
        ui->spYmin->setEnabled(true);
        ui->spYmax->setEnabled(true);

        emit yScaleChanged(false, ui->spYmin->value(), ui->spYmax->value());
    }
}

void PlotControlPanel::onYScaleChanged()
{
    if (!autoScale())
    {
        emit yScaleChanged(false, ui->spYmin->value(), ui->spYmax->value());
    }
}

bool PlotControlPanel::autoScale() const
{
    return ui->cbAutoScale->isChecked();
}

double PlotControlPanel::yMax() const
{
    return ui->spYmax->value();
}

double PlotControlPanel::yMin() const
{
    return ui->spYmin->value();
}

bool PlotControlPanel::xAxisAsIndex() const
{
    return ui->cbIndex->isChecked();
}

double PlotControlPanel::xMax() const
{
    return ui->spXmax->value();
}

double PlotControlPanel::xMin() const
{
    return ui->spXmin->value();
}

void PlotControlPanel::onRangeSelected()
{
    Range r = ui->cbRangePresets->currentData().value<Range>();
    ui->spYmin->setValue(r.rmin);
    ui->spYmax->setValue(r.rmax);
    ui->cbAutoScale->setChecked(false);
}

void PlotControlPanel::onIndexChecked(bool checked)
{
    if (checked)
    {
        ui->lXmin->setEnabled(false);
        ui->lXmax->setEnabled(false);
        ui->spXmin->setEnabled(false);
        ui->spXmax->setEnabled(false);

        emit xScaleChanged(true); // use index
    }
    else
    {
        ui->lXmin->setEnabled(true);
        ui->lXmax->setEnabled(true);
        ui->spXmin->setEnabled(true);
        ui->spXmax->setEnabled(true);

        emit xScaleChanged(false, ui->spXmin->value(), ui->spXmax->value());
    }
    emit plotWidthChanged(plotWidth());
}

void PlotControlPanel::onXScaleChanged()
{
    if (!xAxisAsIndex())
    {
        emit xScaleChanged(false, ui->spXmin->value(), ui->spXmax->value());
        emit plotWidthChanged(plotWidth());
    }
}

double PlotControlPanel::plotWidth() const
{
    double value = ui->spPlotWidth->value();
    if (!xAxisAsIndex())
    {
        // scale by xmin and xmax
        auto xmax = ui->spXmax->value();
        auto xmin = ui->spXmin->value();
        double scale = (xmax - xmin) / _numOfSamples;
        value *= scale;
    }
    return value;
}

void PlotControlPanel::onPlotWidthChanged()
{
    emit plotWidthChanged(plotWidth());
}

void PlotControlPanel::setChannelInfoModel(ChannelInfoModel* model)
{
    ui->tvChannelInfo->setModel(model);

    // channel color selector
    connect(ui->tvChannelInfo->selectionModel(), &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &current, const QModelIndex &previous)
            {
                // TODO: duplicate with below lambda
                QColor color(0,0,0,0); // transparent

                if (current.isValid())
                {
                    ui->colorSelector->setEnabled(true);
                    auto model = ui->tvChannelInfo->model();
                    color = model->data(current, Qt::ForegroundRole).value<QColor>();
                }
                else
                {
                    ui->colorSelector->setDisabled(true);
                }

                // temporarily block signals because `setColor` emits `colorChanged`
                bool wasBlocked = ui->colorSelector->blockSignals(true);
                ui->colorSelector->setColor(color);
                ui->colorSelector->blockSignals(wasBlocked);
            });

    connect(ui->tvChannelInfo->selectionModel(), &QItemSelectionModel::selectionChanged,
            [this](const QItemSelection & selected, const QItemSelection & deselected)
            {
                if (!selected.length())
                {
                    ui->colorSelector->setDisabled(true);

                    // temporarily block signals because `setColor` emits `colorChanged`
                    bool wasBlocked = ui->colorSelector->blockSignals(true);
                    ui->colorSelector->setColor(QColor(0,0,0,0));
                    ui->colorSelector->blockSignals(wasBlocked);
                }
            });

    connect(ui->colorSelector, &color_widgets::ColorSelector::colorChanged,
            [this](QColor color)
            {
                auto index = ui->tvChannelInfo->selectionModel()->currentIndex();
                ui->tvChannelInfo->model()->setData(index, color, Qt::ForegroundRole);
            });

    connect(model, &QAbstractItemModel::dataChanged,
            [this](const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles = QVector<int> ())
            {
                auto current = ui->tvChannelInfo->selectionModel()->currentIndex();

                // no current selection
                if (!current.isValid()) return;

                auto mod = ui->tvChannelInfo->model();
                QColor color = mod->data(current, Qt::ForegroundRole).value<QColor>();

                // temporarily block signals because `setColor` emits `colorChanged`
                bool wasBlocked = ui->colorSelector->blockSignals(true);
                ui->colorSelector->setColor(color);
                ui->colorSelector->blockSignals(wasBlocked);
            });

    // reset actions
    connect(&resetAct, &QAction::triggered, model, &ChannelInfoModel::resetInfos);
    connect(&resetNamesAct, &QAction::triggered, model, &ChannelInfoModel::resetNames);
    connect(&resetColorsAct, &QAction::triggered, model, &ChannelInfoModel::resetColors);
    connect(&showAllAct, &QAction::triggered, model, &ChannelInfoModel::resetVisibility);
}

void PlotControlPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    settings->setValue(SG_Plot_NumOfSamples, numOfSamples());
    settings->setValue(SG_Plot_PlotWidth, ui->spPlotWidth->value());
    settings->setValue(SG_Plot_IndexAsX, xAxisAsIndex());
    settings->setValue(SG_Plot_XMax, xMax());
    settings->setValue(SG_Plot_XMin, xMin());
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
    ui->spPlotWidth->setValue(
        settings->value(SG_Plot_PlotWidth, ui->spPlotWidth->value()).toInt());
    ui->cbIndex->setChecked(
        settings->value(SG_Plot_IndexAsX, xAxisAsIndex()).toBool());
    ui->spXmax->setValue(settings->value(SG_Plot_XMax, xMax()).toDouble());
    ui->spXmin->setValue(settings->value(SG_Plot_XMin, xMin()).toDouble());
    ui->cbAutoScale->setChecked(
        settings->value(SG_Plot_AutoScale, autoScale()).toBool());
    ui->spYmax->setValue(settings->value(SG_Plot_YMax, yMax()).toDouble());
    ui->spYmin->setValue(settings->value(SG_Plot_YMin, yMin()).toDouble());
    settings->endGroup();
}
