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

class ChannelInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ChannelInfoModel(unsigned numberOfChannels, QObject *parent = 0);

    // implemented from QAbstractItemModel
    int           rowCount(const QModelIndex &parent = QModelIndex()) const;
    int           columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool          setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void setNumOfChannels(unsigned number);

private:
    struct ChannelInfo
    {
        QString name;
        bool visibility;
        QColor color;
    };

    unsigned _numOfChannels;     ///< @note this is not necessarily the length of `infos`

    /**
     * Channel info is added here but never removed so that we can
     * remember user entered names.
     */
    QList<ChannelInfo> infos;
};

#endif // CHANNELINFOMODEL_H
