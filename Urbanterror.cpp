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

#include "Urbanterror.h"
#include <QHostInfo>
#include <QtDebug>

static QString gametype(const QString &num)
{
    bool ok;
    int type = num.toInt(&ok, 10);
    if(!ok)
        return "";
    switch(type)
    {
    case 0:
        return "FreeForAll";
        break;
    case 3:
        return "TeamDeathMatch";
        break;
    case 4:
        return "Team Survivor";
        break;
    case 5:
        return "Follow the Leader";
        break;
    case 6:
        return "Capture and Hold";
        break;
    case 7:
        return "Capture the Flag";
        break;
    case 8:
        return "Bombmode";
        break;
    default:
        return QString("Mode %1").arg(num);
        break;
    }
}

UrbanterrorServer::UrbanterrorServer(const char *host, int port)
  : m_sHost(host), m_iPort(port), m_iNumPlayers(0), m_iMaxPlayers(0)
{
    m_pUdpSocket = new QUdpSocket(this);
    {
        int port = 5000;
        while(!m_pUdpSocket->bind(QHostAddress::Any, port))
        {
            delete m_pUdpSocket; m_pUdpSocket = new QUdpSocket(this); // FIXME
            port++;
            if(port == 5040)
                throw ServerError(QString("UrbanterrorServer: "
                    "impossible d'ecouter: ") + m_pUdpSocket->errorString());
        }
        qDebug() << "UrbanterrorServer: en ecoute sur le port " << port << "\n";
    }
    connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));

    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(false);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(query()));
    m_pTimer->start(30000);

    query();
}

unsigned int UrbanterrorServer::numPlayers() const
{
    return m_iNumPlayers;
}

unsigned int UrbanterrorServer::maxPlayers() const
{
    return m_iMaxPlayers;
}

QString UrbanterrorServer::map() const
{
    return m_sMap;
}

QString UrbanterrorServer::mode() const
{
    return m_sMode;
}

void UrbanterrorServer::query()
{
    QHostInfo ns = QHostInfo::fromName(m_sHost);
    if(!ns.addresses().isEmpty())
    {
        if(m_pUdpSocket->writeDatagram("\xFF\xFF\xFF\xFFgetstatus", 13,
            ns.addresses().first(), m_iPort) == -1)
        {
            emit errorEncountered(m_pUdpSocket->errorString());
        }
    }
    else
        emit errorEncountered(ns.errorString());
}

void UrbanterrorServer::refresh()
{
    query();
    m_pTimer->start();
}

void UrbanterrorServer::receiveData()
{
    while(m_pUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_pUdpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_pUdpSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);
    
        if(datagram.left(19) != "\xFF\xFF\xFF\xFFstatusResponse\x0A")
        {
            emit errorEncountered("UrbanterrorServer: impossible de comprendre "
                "les données reçues");
            return ;
        }

        // Lit les informations générales
        QMap<QString, QString> infos;
        QRegExp regexp("\\\\([^\\\\]+)\\\\");
        int end = datagram.indexOf('\x0A', 19);
        int pos = 19;
        while((pos = regexp.indexIn(datagram, pos)) != -1 && pos < end)
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

        // Compte les joueurs
        int players = 0;
        pos = end;
        while((pos = datagram.indexOf('\x0A', pos+1)) != -1)
            players++;

        bool ok, changed = false;
        unsigned int old_players = m_iNumPlayers;
        changed = confirm_assign(&m_iNumPlayers, (unsigned)players) || changed;
        if(!infos["sv_maxclients"].isEmpty())
            changed = confirm_assign(&m_iMaxPlayers,
                (unsigned)infos["sv_maxclients"].toInt(&ok, 10)) || changed;
        if(!infos["mapname"].isEmpty())
            changed = confirm_assign(&m_sMap, infos["mapname"]) || changed;
        if(!infos["g_gametype"].isEmpty())
            changed = confirm_assign(&m_sMode, gametype(infos["g_gametype"]))
                || changed;
        if(changed)
            emit infosChanged(m_iNumPlayers, m_iMaxPlayers, m_sMap, m_sMode,
                old_players == 0 && m_iNumPlayers > 0);
    }
}
