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

private slots:
    void requestFinished(int id, bool error);

public:
    TeeworldsHtml(const char *host, int port, const char *uri);

public slots:
    void refresh();
    void forceRefresh();

signals:
    void infosChanged(QString game, int players, int max, QString map,
        QString mode);
    void errorEncountered(QString text);

};
