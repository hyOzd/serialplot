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

#include "channelinfomodel.h"
#include "setting_defines.h"

#define NUMOF_COLORS  (32)

const QColor colors[NUMOF_COLORS] =
{
    QColor("#ff0056"),
    QColor("#7e2dd2"),
    QColor("#00ae7e"),
    QColor("#fe8900"),
    QColor("#ff937e"),
    QColor("#6a826c"),
    QColor("#ff029d"),
    QColor("#00b917"),
    QColor("#7a4782"),
    QColor("#85a900"),
    QColor("#a42400"),
    QColor("#683d3b"),
    QColor("#bdc6ff"),
    QColor("#263400"),
    QColor("#bdd393"),
    QColor("#d5ff00"),
    QColor("#9e008e"),
    QColor("#001544"),
    QColor("#c28c9f"),
    QColor("#ff74a3"),
    QColor("#01d0ff"),
    QColor("#004754"),
    QColor("#e56ffe"),
    QColor("#788231"),
    QColor("#0e4ca1"),
    QColor("#91d0cb"),
    QColor("#be9970"),
    QColor("#968ae8"),
    QColor("#bb8800"),
    QColor("#43002c"),
    QColor("#deff74"),
    QColor("#00ffc6")
};

ChannelInfoModel::ChannelInfoModel(unsigned numberOfChannels, QObject* parent) :
    QAbstractTableModel(parent)
{
    _numOfChannels = 0;
    setNumOfChannels(numberOfChannels);
}

ChannelInfoModel::ChannelInfoModel(const ChannelInfoModel& other) :
    ChannelInfoModel(other.rowCount(), other.parent())
{
    for (int i = 0; i < other.rowCount(); i++)
    {
        setData(index(i, COLUMN_NAME),
                other.data(other.index(i, COLUMN_NAME), Qt::EditRole),
                Qt::EditRole);
        setData(index(i, COLUMN_NAME),
                other.data(other.index(i, COLUMN_NAME), Qt::ForegroundRole),
                Qt::ForegroundRole);
        setData(index(i, COLUMN_VISIBILITY),
                other.data(other.index(i, COLUMN_VISIBILITY), Qt::CheckStateRole),
                Qt::CheckStateRole);
    }
}

ChannelInfoModel::ChannelInfoModel(const QStringList& channelNames) :
    ChannelInfoModel(channelNames.length(), NULL)
{
    for (int i = 0; i < channelNames.length(); i++)
    {
        setData(index(i, COLUMN_NAME), channelNames[i], Qt::EditRole);
    }
}

ChannelInfoModel::ChannelInfo::ChannelInfo(unsigned index)
{
    name = tr("Channel %1").arg(index + 1);
    visibility = true;
    color = colors[index % NUMOF_COLORS];
}

QString ChannelInfoModel::name(unsigned i) const
{
    return infos[i].name;
}

QColor ChannelInfoModel::color(unsigned i) const
{
    return infos[i].color;
}

bool ChannelInfoModel::isVisible(unsigned i) const
{
    return infos[i].visibility;
}

QStringList ChannelInfoModel::channelNames() const
{
    QStringList r;
    for (unsigned ci = 0; ci < _numOfChannels; ci++)
    {
        r << name(ci);
    }
    return r;
}

int ChannelInfoModel::rowCount(const QModelIndex &parent) const
{
    return _numOfChannels;
}

int ChannelInfoModel::columnCount(const QModelIndex & parent) const
{
    return COLUMN_COUNT;
}

Qt::ItemFlags ChannelInfoModel::flags(const QModelIndex &index) const
{
    if (index.column() == COLUMN_NAME)
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    }
    else if (index.column() == COLUMN_VISIBILITY)
    {
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    }

    return Qt::NoItemFlags;
}

QVariant ChannelInfoModel::data(const QModelIndex &index, int role) const
{
    // check index
    if (index.row() >= (int) _numOfChannels || index.row() < 0)
    {
        return QVariant();
    }

    // get color
    if (role == Qt::ForegroundRole)
    {
        return infos[index.row()].color;
    }

    // get name
    if (index.column() == COLUMN_NAME)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            return QVariant(infos[index.row()].name);
        }
    } // get visibility
    else if (index.column() == COLUMN_VISIBILITY)
    {
        if (role == Qt::CheckStateRole)
        {
            bool visible = infos[index.row()].visibility;
            return visible ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

QVariant ChannelInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == COLUMN_NAME)
            {
                return tr("Channel");
            }
            else if (section == COLUMN_VISIBILITY)
            {
                return tr("Visible");
            }
        }
    }
    else                        // vertical
    {
        if (section < (int) _numOfChannels && role == Qt::DisplayRole)
        {
            return QString::number(section + 1);
        }
    }

    return QVariant();
}

bool ChannelInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // check index
    if (index.row() >= (int) _numOfChannels || index.row() < 0)
    {
        return false;
    }

    // set color
    if (role == Qt::ForegroundRole)
    {
        infos[index.row()].color = value.value<QColor>();
        emit dataChanged(index, index, QVector<int>({Qt::ForegroundRole}));
        return true;
    }

    // set name
    if (index.column() == COLUMN_NAME)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            infos[index.row()].name = value.toString();
            emit dataChanged(index, index, QVector<int>({role}));
            return true;
        }
    } // set visibility
    else if (index.column() == COLUMN_VISIBILITY)
    {
        if (role == Qt::CheckStateRole)
        {
            bool checked = value.toInt() == Qt::Checked;
            infos[index.row()].visibility = checked;
            emit dataChanged(index, index, QVector<int>({role}));
            return true;
        }
    }

    // invalid index/role
    return false;
}

void ChannelInfoModel::setNumOfChannels(unsigned number)
{
    if (number == _numOfChannels) return;

    bool isInserting = number > _numOfChannels;
    if (isInserting)
    {
        beginInsertRows(QModelIndex(), _numOfChannels, number-1);
    }
    else
    {
        beginRemoveRows(QModelIndex(), number, _numOfChannels-1);
    }

    // we create channel info but never remove channel info to
    // remember user entered info
    if ((int) number > infos.length())
    {
        for (unsigned ci = infos.length(); ci < number; ci++)
        {
            infos.append(ChannelInfo(ci));
        }
    }

    // make sure newly available channels are visible, we don't
    // remember visibility option intentionally so that user doesn't
    // get confused
    if (number > _numOfChannels)
    {
        for (unsigned ci = _numOfChannels; ci < number; ci++)
        {
            infos[ci].visibility = true;
        }
    }

    _numOfChannels = number;

    if (isInserting)
    {
        endInsertRows();
    }
    else
    {
        endRemoveRows();
    }
}

void ChannelInfoModel::resetInfos()
{
    beginResetModel();
    for (unsigned ci = 0; (int) ci < infos.length(); ci++)
    {
        infos[ci] = ChannelInfo(ci);
    }
    endResetModel();
}

void ChannelInfoModel::resetNames()
{
    beginResetModel();
    for (unsigned ci = 0; (int) ci < infos.length(); ci++)
    {
        infos[ci].name = ChannelInfo(ci).name;
    }
    endResetModel();
}

void ChannelInfoModel::resetColors()
{
    beginResetModel();
    for (unsigned ci = 0; (int) ci < infos.length(); ci++)
    {
        infos[ci].color = ChannelInfo(ci).color;
    }
    endResetModel();
}

void ChannelInfoModel::resetVisibility()
{
    beginResetModel();
    for (unsigned ci = 0; (int) ci < infos.length(); ci++)
    {
        infos[ci].visibility = true;
    }
    endResetModel();
}

void ChannelInfoModel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Channels);
    settings->beginWriteArray(SG_Channels_Channel);

    // save all channel information regardless of current number of channels
    for (unsigned ci = 0; (int) ci < infos.length(); ci++)
    {
        settings->setArrayIndex(ci);
        settings->setValue(SG_Channels_Name, infos[ci].name);
        settings->setValue(SG_Channels_Color, infos[ci].color);
        settings->setValue(SG_Channels_Visible, infos[ci].visibility);
    }

    settings->endArray();
    settings->endGroup();
}

void ChannelInfoModel::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Channels);
    unsigned size = settings->beginReadArray(SG_Channels_Channel);

    for (unsigned ci = 0; ci < size; ci++)
    {
        settings->setArrayIndex(ci);

        ChannelInfo chanInfo(ci);
        chanInfo.name       = settings->value(SG_Channels_Name, chanInfo.name).toString();
        chanInfo.color      = settings->value(SG_Channels_Color, chanInfo.color).value<QColor>();
        chanInfo.visibility = settings->value(SG_Channels_Visible, true).toBool();

        if ((int) ci < infos.size())
        {
            infos[ci] = chanInfo;

            if (ci < _numOfChannels)
            {
                auto roles = QVector<int>({
                    Qt::DisplayRole, Qt::EditRole, Qt::ForegroundRole, Qt::CheckStateRole});
                emit dataChanged(index(ci, 0), index(ci, COLUMN_COUNT-1), roles);
            }
        }
        else
        {
            infos.append(chanInfo);
        }
    }

    settings->endArray();
    settings->endGroup();
}
