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

#include "config.h"

/**
 * A server being monitored, i.e. the Server representation plus the
 * notifiation configuration for this server.
 */
class MonitoredServer : public QObject {

    Q_OBJECT

public:
    QString name;
    bool play_sound;
    bool change_color;
    bool display_popup;
    Server *server;

public:
    inline MonitoredServer() {}
    MonitoredServer(QString p_Name, bool p_Sound, bool p_Color, bool p_Popup,
        Server *p_Server);

public:
    /**
     * The number of players currently in-game.
     */
    inline unsigned int numPlayers() const { return server->numPlayers(); }
    /**
     * The maximum number of players that can join the game, or 0.
     */
    inline unsigned int maxPlayers() const { return server->maxPlayers(); }
    /**
     * The map's name, if relevant, or QString().
     */
    inline QString map() const { return server->map(); }
    /**
     * The game mode, if relevant, or QString().
     */
    inline QString mode() const { return server->mode(); }

signals:
    /**
     * Signal emitted when the informations on the server have changed.
     *
     * @param gamestarted Boolean indicating whether the game just started, ie
     * there was no player before and there are players now. It should therefore
     * never be set two consecutive times.
     */
    void infosChanged(unsigned int players, unsigned int max, QString map,
        QString mode, bool gamestarted);

    /**
     * Signal emitted on errors.
     */
    void errorEncountered(QString text);

};

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
