/*
  Copyright © 2018 Hasan Yavuz Özderya

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

#ifndef ASCIIREADER_H
#define ASCIIREADER_H

#include <QSettings>
#include <QString>

#include "samplepack.h"
#include "abstractreader.h"
#include "asciireadersettings.h"

class AsciiReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit AsciiReader(QIODevice* device, QObject *parent = 0);
    QWidget* settingsWidget();
    unsigned numChannels() const;
    void enable(bool enabled) override;
    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

private:
    AsciiReaderSettings _settingsWidget;
    unsigned _numChannels;
    /// number of channels will be determined from incoming data
    unsigned autoNumOfChannels;
    QChar delimiter; ///< selected column delimiter

    bool firstReadAfterEnable = false;

private slots:
    void onDataReady() override;
    /**
     * Parses given line and returns sample pack.
     *
     * Returns `nullptr` in case of error.
     */
    SamplePack* parseLine(const QString& line) const;
};

#endif // ASCIIREADER_H
