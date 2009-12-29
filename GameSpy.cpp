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

GameSpyServer::GameSpyServer(const char *host, int port)
  : m_bForceDisplay(false), m_sHost(host), m_iPort(port),
    m_iNumPlayers(0), m_iMaxPlayers(0)
{
    m_pUdpSocket = new QUdpSocket(this);
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
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

void GameSpyServer::refresh()
{
    qDebug() << "GameSpyServer::refresh()\n";
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

void GameSpyServer::forceRefresh()
{
    qDebug() << "GameSpyServer::forceRefresh()\n";
    m_bForceDisplay = true;
    refresh();
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
        QRegExp regexp("\\(.+)\\");
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

        bool ok;
        m_iNumPlayers = infos["numplayers"].toInt(&ok, 10);
        m_iMaxPlayers = infos["maxplayers"].toInt(&ok, 10);
        m_sMap = infos["mapname"];
        m_sMode = infos["gametype"];
        emit infosChanged(m_iNumPlayers, m_iMaxPlayers, m_sMap, m_sMode);

        m_bForceDisplay = false;
    }
}
