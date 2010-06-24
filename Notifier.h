/*            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QSound>
#include <QMap>
#include <QTimer>

#include "Server.h"
#include "ConfigDialog.h"
#include "MonitoredServer.h"

#include "config.h"

/**
 * The notifier.
 */
class Notifier : public QWidget {

    Q_OBJECT

private:
    QSystemTrayIcon *m_pTrayIcon;
    QSound *m_pBeep;
    ConfigDialog *m_pConfigDialog;

    QList<MonitoredServer*> m_lServers;

    struct Notification {
        QString title;
        QString message;
    };
    QList<Notification> m_lNotifications;
    QList<Notification> m_lErrors;
    QTimer *m_pMessageTimer;
    QIcon m_IconEmpty;
    QIcon m_IconPlayers;

private:
    void addServer(MonitoredServer *serv);
    void appendNotification(QString name, unsigned int players,
        unsigned int max, QString map, QString mode);

private slots:
    void displayError(QString error);
    void infosChanged(int players, int max, QString map, QString mode,
        bool gamestarted);
    void updateMessage();
    void flushNotifications();
    void updateIcon();
    void tellAgain();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

public:
    Notifier(QWidget *pParent = NULL);

signals:
    void refreshAll();

};

#endif
