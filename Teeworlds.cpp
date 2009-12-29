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

#include "Teeworlds.h"
#include <QHostInfo>
#include <QtDebug>

TeeworldsServer::TeeworldsServer(const char *host, int port)
  : m_bForceDisplay(false), m_sHost(host), m_iPort(port),
    m_iNumPlayers(0), m_iMaxPlayers(0)
{
    m_pUdpSocket = new QUdpSocket(this);
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

unsigned int TeeworldsServer::numPlayers() const
{
    return m_iNumPlayers;
}

unsigned int TeeworldsServer::maxPlayers() const
{
    return m_iMaxPlayers;
}

QString TeeworldsServer::map() const
{
    return m_sMap;
}

QString TeeworldsServer::mode() const
{
    return m_sMode;
}

void TeeworldsServer::refresh()
{
    qDebug() << "TeeworldsServer::refresh()\n";
    QHostInfo ns = QHostInfo::fromName(m_sHost);
    if(!ns.addresses().isEmpty())
    {
        if(m_pUdpSocket->writeDatagram(
            "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xffgief", 10,
            ns.addresses().first(), m_iPort) == -1)
        {
            emit errorEncountered(m_pUdpSocket->errorString());
        }
    }
    else
        emit errorEncountered(ns.errorString());
}

void TeeworldsServer::forceRefresh()
{
    qDebug() << "TeeworldsServer::forceRefresh()\n";
    m_bForceDisplay = true;
    refresh();
}

void TeeworldsServer::receiveData()
{
    while(m_pUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_pUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUdpSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);

        QRegExp regexp("info([0-9]+\\.[0-9]+\\.[0-9]+)" // server version
            "\\0000(.*)" // server name
            "\\0000(\\w*)" // map name
            "\\0000(\\w*)" // game type
            "\\0000([0-9]*)" // flags
            "\\0000([0-9]*)" // progression
            "\\0000([0-9]*)" // player count
            "\\0000([0-9]*)" // max players
            "\\0000");
        if(regexp.indexIn(datagram) == -1)
        {
            emit errorEncountered("R�ponse invalide");
        }

        bool ok, changed = false;
        changed = confirm_assign(&m_iNumPlayers,
            (unsigned)regexp.cap(7).toInt(&ok, 10)) || changed;
        changed = confirm_assign(&m_iMaxPlayers,
            (unsigned)regexp.cap(8).toInt(&ok, 10)) || changed;
        changed = confirm_assign(&m_sMap, regexp.cap(3)) || changed;
        changed = confirm_assign(&m_sMode, regexp.cap(4)) || changed;
        if(changed || m_bForceDisplay)
            emit infosChanged(m_iNumPlayers, m_iMaxPlayers, m_sMap, m_sMode);

        m_bForceDisplay = false;
    }
}
