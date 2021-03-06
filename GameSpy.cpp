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

#include "GameSpy.h"
#include <QHostInfo>
#include <QtDebug>

GameSpyServer::GameSpyServer(const QString &param)
{
    QRegExp reg("^([^ ]+) ([0-9]+)$");
    if(reg.indexIn(param) != -1)
    {
        bool ok;
        int port = reg.cap(2).toInt(&ok, 10);
        if(ok && port >= 1 && port <= 65535)
        {
            setup(reg.cap(1).toLatin1(), port);
            return ;
        }
    }

    throw ServerError(tr("GameSpyServer: invalid configuration"));
}

GameSpyServer::GameSpyServer(const char *host, int port)
{
    setup(host, port);
}

void GameSpyServer::setup(const char *host, int port)
{
    m_sHost = host; m_iPort = port; m_iNumPlayers = 0; m_iMaxPlayers = 0;

    m_pUdpSocket = new QUdpSocket(this);
    {
        int port = 5000;
        while(!m_pUdpSocket->bind(QHostAddress::Any, port))
        {
            delete m_pUdpSocket; m_pUdpSocket = new QUdpSocket(this); // FIXME
            port++;
            if(port == 5040)
                throw ServerError(QString("GameSpyServer: "
                    "can't listen: ") + m_pUdpSocket->errorString());
        }
        qDebug() << tr("GameSpyServer: listening on port %1").arg(port);
    }
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));

    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(false);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(query()));
    m_pTimer->start(30000);

    query();
}

unsigned int GameSpyServer::numPlayers() const
{
    return m_iNumPlayers;
}

unsigned int GameSpyServer::maxPlayers() const
{
    return m_iMaxPlayers;
}

QString GameSpyServer::map() const
{
    return m_sMap;
}

QString GameSpyServer::mode() const
{
    return m_sMode;
}

void GameSpyServer::query()
{
    QHostInfo ns = QHostInfo::fromName(m_sHost);
    if(!ns.addresses().isEmpty())
    {
        if(m_pUdpSocket->writeDatagram("\\basic\\\\info\\", 13,
            ns.addresses().first(), m_iPort) == -1)
        {
            emit errorEncountered(m_pUdpSocket->errorString());
        }
    }
    else
        emit errorEncountered(ns.errorString());
}

void GameSpyServer::refresh()
{
    query();
    m_pTimer->start();
}

void GameSpyServer::receiveData()
{
    while(m_pUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_pUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUdpSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);

        QMap<QString, QString> infos;
        QRegExp regexp("\\\\([^\\\\]+)\\\\");
        int pos = 0;
        while((pos = regexp.indexIn(datagram, pos)) != -1)
        {
            QString key = regexp.cap(1);
            pos += regexp.matchedLength() - 1;
            if( (key != "basic" && key != "info")
             && ((pos = regexp.indexIn(datagram, pos)) != -1) )
            {
                infos[key] = regexp.cap(1);
                pos += regexp.matchedLength() - 1;
            }
        }

        bool ok, changed = false;
        unsigned int old_players = m_iNumPlayers;
        changed = confirm_assign(&m_iNumPlayers,
            (unsigned)infos["numplayers"].toInt(&ok, 10)) || changed;
        if(infos["numplayers"] != "0")
            changed = confirm_assign(&m_iMaxPlayers,
                (unsigned)infos["maxplayers"].toInt(&ok, 10)) || changed;
        if(!infos["mapname"].isEmpty())
            changed = confirm_assign(&m_sMap, infos["mapname"]) || changed;
        if(!infos["gametype"].isEmpty())
            changed = confirm_assign(&m_sMode, infos["gametype"]) || changed;
        if(changed)
            emit infosChanged(m_iNumPlayers, m_iMaxPlayers, m_sMap, m_sMode,
                old_players == 0 && m_iNumPlayers > 0);
    }
}
