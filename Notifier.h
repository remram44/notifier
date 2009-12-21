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

#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QSound>
#include <QSet>

#ifndef PREFIX
#define PREFIX "/usr/local"
#endif

/**
 * A server we can query for players information.
 */
class Server : public QObject {

    Q_OBJECT

public slots:
    /**
     * Update infos by querying the server.
     */
    virtual void refresh() = 0;

    /**
     * Forced update: emit infosChanged() no matter what.
     */
    virtual void forceRefresh() = 0;

signals:
    /**
     * Signal emitted when new informations are available.
     */
    void infosChanged(QString game, int players, int max, QString map,
        QString mode);

    /**
     * Signal emitted on errors.
     */
    void errorEncountered(QString text);

};

/**
 * The notifier.
 */
class Notifier : public QWidget {

    Q_OBJECT

private:
    QSystemTrayIcon *m_pTrayIcon;
    QSound *m_pBeep;

    QSet<Server*> m_Servers;

private slots:
    void displayError(QString error);
    void infosChanged(QString game, int players, int max, QString map,
        QString mode);

public:
    Notifier(QWidget *pParent = NULL);

signals:
    void refreshAll();
    void forceRefreshAll();

};

#endif
