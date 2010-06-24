#include "ServerListWidget.h"

#include <QHBoxLayout>
#include "ConfigDialog/ServerListModel.h"

ServerListWidget::ServerListWidget(QList<MonitoredServer*> *monitoredServerList, QWidget *parent) :
    QGroupBox(parent)
{
    setTitle(tr("Monitor"));
    QHBoxLayout *layout = new QHBoxLayout(this);
    treeView = new QTreeView(this);
    treeView->rootIsDecorated();
    layout->addWidget(treeView);
    currentServerConfWidget = new ServerConfWidget;
    currentServerConfWidget->setParent(this);
    layout->addWidget(currentServerConfWidget);

    setLayout(layout);

    model = new ServerListModel(monitoredServerList, this);
    treeView->setModel(model);
}
