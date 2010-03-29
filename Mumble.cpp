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

#include "Mumble.h"
#include <QHostInfo>
#include <QtDebug>

MumbleServer::MumbleServer(const QString &param)
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

    throw ServerError(tr("MumbleServer: invalid configuration"));
}

MumbleServer::MumbleServer(const char *host, int port)
{
    setup(host, port);
}

void MumbleServer::setup(const char *host, int port)
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
                throw ServerError(tr("MumbleServer: "
                    "can't listen: ") + m_pUdpSocket->errorString());
        }
        qDebug() << tr("MumbleServer: listening on port %1").arg(port);
    }
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));

    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(false);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(query()));
    m_pTimer->start(30000);

    query();
}

unsigned int MumbleServer::numPlayers() const
{
    return m_iNumPlayers;
}

unsigned int MumbleServer::maxPlayers() const
{
    return m_iMaxPlayers;
}

QString MumbleServer::map() const
{
    return "";
}

QString MumbleServer::mode() const
{
    return "";
}

void MumbleServer::query()
{
    QHostInfo ns = QHostInfo::fromName(m_sHost);
    if(!ns.addresses().isEmpty())
    {
        if(m_pUdpSocket->writeDatagram(QByteArray(12, '\0'), 12,
            ns.addresses().first(), m_iPort) == -1)
        {
            emit errorEncountered(m_pUdpSocket->errorString());
        }
    }
    else
        emit errorEncountered(ns.errorString());
}

void MumbleServer::refresh()
{
    query();
    m_pTimer->start();
}

void MumbleServer::receiveData()
{
    while(m_pUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_pUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUdpSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);

        unsigned int players = datagram[12]<<24 | datagram[13]<<16
            | datagram[14]<<8 | datagram[15];
        unsigned int maxplayers = datagram[16]<<24 | datagram[17]<<16
            | datagram[18]<<8 | datagram[19];

        bool changed = false;
        unsigned int old_players = m_iNumPlayers;
        changed = confirm_assign(&m_iNumPlayers, players) || changed;
        changed = confirm_assign(&m_iMaxPlayers, maxplayers) || changed;
        if(changed)
            emit infosChanged(m_iNumPlayers, m_iMaxPlayers, "", "",
                old_players == 0 && m_iNumPlayers > 0);
    }
}
