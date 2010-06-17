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

#ifndef MUMBLE_H
#define MUMBLE_H

#include "Server.h"
#include <QUdpSocket>
#include <QString>
#include <QLatin1String>
#include <QTimer>
#include <QLineEdit>
#include <QSpinBox>

class MumbleServer;

/**
 * The MumbleServer configuration widget.
 */
class MumbleServerConfWidget : public ServerConfWidget {

    Q_OBJECT

private:
    MumbleServer *m_pServer;
    QLineEdit *m_pHost;
    QSpinBox *m_pPort;

public:
    MumbleServerConfWidget(MumbleServer *serv);

public slots:
    void applyChanges();

};

/**
 * A Mumble serveur.
 */
class MumbleServer : public Server {

    Q_OBJECT

private:
    QUdpSocket *m_pUdpSocket;
    QString m_sHost;
    int m_iPort;
    QTimer *m_pTimer;

    MumbleServerConfWidget *m_pConfWidget;

    unsigned int m_iNumPlayers;
    unsigned int m_iMaxPlayers;

public:
    MumbleServer(const QString &host, int port,
        MumbleServerConfWidget *w = NULL);
    MumbleServer(const QString &param);

    unsigned int numPlayers() const;
    unsigned int maxPlayers() const;
    QString map() const;
    QString mode() const;

    ServerConfWidget *confWidget();

private:
    void setup(const QString &host, int port);

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

class MumbleServerFactory : public ServerFactory {

public:
    MumbleServerFactory();
    virtual QString displayName() const { return "Mumble"; }
    virtual ServerConfWidget *newConfWidget() const
    { return new MumbleServerConfWidget(NULL); }
    Server *createFromConfig(const QString &line) const;

};

#endif
