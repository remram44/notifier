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
#include <QHttp>
#include <QSound>

const QString HOST = "yoshi.rez-gif.supelec.fr";
const int PORT = 80;
const QString URI = "/tw/";

#ifndef PREFIX
#define PREFIX "/usr/local"
#endif

class Notifier : public QWidget {

    Q_OBJECT

private:
    QSystemTrayIcon *m_pTrayIcon;
    QSound *m_pBeep;
    QHttp *m_pHTTP;

    int old_nb;
    bool forceDisplay;

public:
    Notifier(QWidget *pParent = NULL);

public slots:
    void refresh();
    void forceRefresh();
    void requestFinished(int id, bool error);

};

#endif
