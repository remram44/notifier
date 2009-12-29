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

#ifndef TEEWORLDSHTML_H
#define TEEWORLDSHTML_H

#include "Notifier.h"
#include <QHttp>

/**
 * Teeworlds server update through http://yoshi/tw/.
 *
 * The actual gameservers are not queried, the informations are gathered on the
 * web page.
 */
class TeeworldsHtml : public Server {

    Q_OBJECT

private:
    QHttp *m_pHTTP;
    QString m_sURI;
    bool forceDisplay;

    unsigned int m_iNumPlayers;
    unsigned int m_iMaxPlayers;
    QString m_sMap;
    QString m_sMode;

public:
    TeeworldsHtml(const char *host, int port, const char *uri);

    unsigned int numPlayers() const;
    unsigned int maxPlayers() const;
    QString map() const;
    QString mode() const;

private slots:
    void requestFinished(int id, bool error);

public slots:
    void refresh();
    void forceRefresh();

signals:
    void infosChanged(int players, int max, QString map, QString mode);
    void errorEncountered(QString text);

};

#endif
