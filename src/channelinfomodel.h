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

#ifndef CHANNELINFOMODEL_H
#define CHANNELINFOMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QSettings>
#include <QStringList>

class ChannelInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ChannelInfoColumn
    {
        COLUMN_NAME = 0,
        COLUMN_VISIBILITY,
        COLUMN_COUNT
    };

    explicit ChannelInfoModel(unsigned numberOfChannels, QObject *parent = 0);
    ChannelInfoModel(const ChannelInfoModel& other);
    explicit ChannelInfoModel(const QStringList& channelNames);

    // implemented from QAbstractItemModel
    int           rowCount(const QModelIndex &parent = QModelIndex()) const;
    int           columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool          setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setNumOfChannels(unsigned number);
    /// Stores all channel info into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads all channel info from a `QSettings`.
    void loadSettings(QSettings* settings);

public slots:
    /// reset all channel info (names, color etc.)
    void resetInfos();
    /// reset all channel names
    void resetNames();
    /// reset all channel colors
    void resetColors();
    /// reset visibility
    void resetVisibility();

private:
    struct ChannelInfo
    {
        explicit ChannelInfo(unsigned index);

        QString name;
        bool visibility;
        QColor color;
    };

    unsigned _numOfChannels;     ///< @note this is not necessarily the length of `infos`

    /**
     * Channel info is added here but never removed so that we can
     * remember user entered info (names, colors etc.).
     */
    QList<ChannelInfo> infos;
};

#endif // CHANNELINFOMODEL_H
