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

#include "TeeworldsHtml.h"
#include <QtDebug>
#include <QTimer>

TeeworldsHtml::TeeworldsHtml(const char *host, int port, const char *uri)
  : m_sURI(uri), m_iNumPlayers(0), m_iMaxPlayers(0)
{
    m_pHTTP = new QHttp(host, port, this);
    connect(m_pHTTP, SIGNAL(requestFinished(int, bool)),
        this, SLOT(requestFinished(int, bool)));

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(query()));
    timer->start(30000);
}

unsigned int TeeworldsHtml::numPlayers() const
{
    return m_iNumPlayers;
}

unsigned int TeeworldsHtml::maxPlayers() const
{
    return m_iMaxPlayers;
}

QString TeeworldsHtml::map() const
{
    return m_sMap;
}

QString TeeworldsHtml::mode() const
{
    return m_sMode;
}

void TeeworldsHtml::query()
{
    if(!m_pHTTP->hasPendingRequests())
        m_pHTTP->get(m_sURI);
}

void TeeworldsHtml::refresh()
{
    query();
}

void TeeworldsHtml::requestFinished(int /*id*/, bool error)
{
    if(error)
    {
        emit errorEncountered(m_pHTTP->errorString());
    }
    else
    {
        QByteArray page = m_pHTTP->readAll();
        QRegExp regexp("<td>(DM|TDM|CTF)</td>\\s+"
            "<td>(\\d+)/(\\d+)</td>\\s+"
            "<td>([^<]+)</td>");
        int pos = 0;
        QStringList msgs;
        while((pos = regexp.indexIn(page, pos)) != -1)
        {
            bool ok;
            int max, players = regexp.cap(2).toInt(&ok, 10);
            if(ok)
                max = regexp.cap(3).toInt(&ok, 10);
            if(ok && players > 0)
            {
                m_iNumPlayers = players;
                m_iMaxPlayers = max;
                m_sMap = regexp.cap(4);
                m_sMode = regexp.cap(1);
                emit infosChanged(m_iNumPlayers, m_iMaxPlayers, m_sMap,
                    m_sMode);
            }
            pos += regexp.matchedLength();
        }
        m_iNumPlayers = 0;
        m_iMaxPlayers = 0;
        m_sMap = QString();
        m_sMode = QString();
    }
}
