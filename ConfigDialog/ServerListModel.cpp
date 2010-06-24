#include "ServerListModel.h"

ServerListModel::ServerListModel(QList<MonitoredServer*> *monitoredServerList, QObject *parent) :
    QAbstractItemModel(parent), monitoredServerList(monitoredServerList)
{
}

QModelIndex ServerListModel::index ( int row, int column, const QModelIndex & parent ) const
{
    if (!parent.isValid())
        return createIndex(row, column, monitoredServerList->value(column));
    else
        return QModelIndex();
}

QModelIndex ServerListModel::parent ( const QModelIndex & /*index*/ ) const
{
    return QModelIndex();
}

int ServerListModel::rowCount ( const QModelIndex & /*parent*/ ) const
{
    return monitoredServerList->count();
}

int ServerListModel::columnCount ( const QModelIndex & parent ) const
{
    if (parent.isValid()) // Not root
        return 0;
    return 5;
}

QVariant ServerListModel::data ( const QModelIndex & index, int role ) const
{
    if (role != Qt::DisplayRole && role != Qt::CheckStateRole)
        return QVariant();

    MonitoredServer *server = monitoredServerList->value(index.row());

    if (server == 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0: //Name
            return server->name();
        case 1: //Type
            return QString();
        }
    }
    else if (role == Qt::CheckStateRole)
    {
        switch (index.column())
        {
        case 2: //Play sound
            return server->play_sound() ? Qt::Checked : Qt::Unchecked;
        case 3: //Change icon
            return server->change_icon() ? Qt::Checked : Qt::Unchecked;
        case 4: //Display popup
            return server->display_popup() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant ServerListModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0:
        return tr("Name");
    case 1:
        return tr("Type");
    case 2:
        return tr("Play sound");
    case 3:
        return tr("Change icon");
    case 4:
        return tr("Display popup");
    }
    return QVariant();
}

Qt::ItemFlags ServerListModel::flags ( const QModelIndex & index ) const
{
    switch (index.column())
    {
    case 2:
    case 3:
    case 4:
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    default:
        return QAbstractItemModel::flags(index);
    }
}
