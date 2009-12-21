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
#include <QtDebug>

TeeworldsHtml::TeeworldsHtml(const char *host, int port, const char *uri)
  : m_sURI(uri), forceDisplay(false)
{
    m_pHTTP = new QHttp(host, port, this);
    connect(m_pHTTP, SIGNAL(requestFinished(int, bool)),
        this, SLOT(requestFinished(int, bool)));
}

void TeeworldsHtml::refresh()
{
    qDebug() << "TeeworldsHtml::refresh()\n";
    if(!m_pHTTP->hasPendingRequests())
        m_pHTTP->get(m_sURI);
}

void TeeworldsHtml::forceRefresh()
{
    qDebug() << "TeeworldsHtml::forceRefresh()\n";
    forceDisplay = true;
    refresh();
}

void TeeworldsHtml::requestFinished(int /*id*/, bool error)
{
    qDebug() << "TeeworldsHtml::requestFinished()";
    if(error)
    {
        emit errorEncountered("Erreur : " + m_pHTTP->errorString());
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
            if(ok && (players > 0 || forceDisplay) )
            {
                emit infosChanged("Teeworlds", players, max, regexp.cap(4),
                    regexp.cap(1));
            }
            pos += regexp.matchedLength();
        }
        forceDisplay = false;
    }
}
