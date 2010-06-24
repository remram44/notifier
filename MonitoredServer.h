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

#ifndef MONITOREDSERVER_H
#define MONITOREDSERVER_H

#include <QObject>
#include "Server.h"

/**
 * A server being monitored, i.e. the Server representation plus the
 * notifiation configuration for this server.
 */
class MonitoredServer : public QObject {

    Q_OBJECT

private:
    QString m_name;
    bool m_play_sound;
    bool m_change_color;
    bool m_display_popup;
    Server *server;

public:
    MonitoredServer(QString p_Name = QString(), bool p_Sound = true,
        bool p_Color = true, bool p_Popup = true, Server *p_Server = 0);

public:
    //! Name given by the user to the server
    QString name() const {return m_name;}

    //! Whether a sound should be played for this server
    bool play_sound() const {return m_play_sound;}

    //! Whether the icon should be changed for this server
    bool change_icon() const {return m_change_color;}

    //! Whether a popup should be display for this server
    bool display_popup() const {return m_display_popup;}

    /**
     * The number of players currently in-game.
     */
    unsigned int numPlayers() const { return server->numPlayers(); }
    /**
     * The maximum number of players that can join the game, or 0.
     */
    unsigned int maxPlayers() const { return server->maxPlayers(); }
    /**
     * The map's name, if relevant, or QString().
     */
    QString map() const { return server->map(); }
    /**
     * The game mode, if relevant, or QString().
     */
    QString mode() const { return server->mode(); }

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

public slots:
    /**
     * Forcibly update the information by querying the server.
     */
    void refresh() {server->refresh();}

};

#endif // MONITOREDSERVER_H
