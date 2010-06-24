#ifndef SERVERLISTWIDGET_H
#define SERVERLISTWIDGET_H

#include <QGroupBox>
#include <QTreeView>
#include "Server.h"
#include "MonitoredServer.h"
#include "ConfigDialog/ServerListModel.h"

class ServerListWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ServerListWidget(QList<MonitoredServer*> *monitoredServerList, QWidget *parent = 0);

signals:

public slots:


private:
    QTreeView *treeView;
    ServerConfWidget *currentServerConfWidget;
    ServerListModel *model;
};

#endif // SERVERLISTWIDGET_H
