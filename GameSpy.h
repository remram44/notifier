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

#ifndef GAMESPY_H
#define GAMESPY_H

#include "Notifier.h"
#include <QUdpSocket>
#include <QTimer>

/**
 * Generic GameSpy server update through the GameSpy protocol.
 */
class GameSpyServer : public Server {

    Q_OBJECT

private:
    QUdpSocket *m_pUdpSocket;
    QString m_sHost;
    int m_iPort;
    QTimer *m_pTimer;

    unsigned int m_iNumPlayers;
    unsigned int m_iMaxPlayers;
    QString m_sMap;
    QString m_sMode;

public:
    GameSpyServer(const char *host, int port);
    GameSpyServer(const QString &param);

    unsigned int numPlayers() const;
    unsigned int maxPlayers() const;
    QString map() const;
    QString mode() const;

private:
    void setup(const char *host, int port);

private slots:
    void query();
    void receiveData();

public slots:
    void refresh();

signals:
    void infosChanged(int players, int max, QString map, QString mode,
        bool gamestarted);
    void errorEncountered(QString text);

};

#endif
