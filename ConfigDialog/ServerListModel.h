#ifndef SERVERLISTMODEL_H
#define SERVERLISTMODEL_H

#include <QAbstractItemModel>
#include "MonitoredServer.h"

class ServerListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ServerListModel(QList<MonitoredServer*> *monitoredServerList, QObject *parent = 0);

    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

signals:

public slots:

private:
    QList<MonitoredServer*> *monitoredServerList;
};

#endif // SERVERLISTMODEL_H
