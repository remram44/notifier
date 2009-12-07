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

#include <iostream>

Notifier::Notifier(QWidget *pParent)
  : QWidget::QWidget(pParent)
{
    setWindowTitle("Teeworlds-Notifier");

    // System tray icon creation
    QMenu *trayMenu = new QMenu(this);
    {
        QAction *refreshAction = new QAction("Vérifier", this);
        connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
        trayMenu->addAction(refreshAction);
    }
    {
        QAction *quitAction = new QAction("Quitter", this);
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        trayMenu->addAction(quitAction);
    }

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(trayMenu);
#ifdef __WIN32
    m_pTrayIcon->setIcon(QIcon("icon.png"));
    m_pBeep = new QSound("beep.wav");
#else
    m_pTrayIcon->setIcon(QIcon(PREFIX "/share/teeworlds-notifier/icon.png"));
    m_pBeep = new QSound(PREFIX "/share/teeworlds-notifier/beep.wab");
#endif
    m_pTrayIcon->show();

    m_pBeep->setLoops(1);

    m_pHTTP = new QHttp(HOST, PORT, this);
    connect(m_pHTTP, SIGNAL(requestFinished(int, bool)),
        this, SLOT(requestFinished(int, bool)));

    refresh();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
    timer->start(30000);
}

void Notifier::refresh()
{
    std::cerr << "refresh()\n";
    if(!m_pHTTP->hasPendingRequests())
        m_pHTTP->get(URI);
}

void Notifier::requestFinished(int id, bool error)
{
    std::cerr << "requestFinished() ";
    if(error)
    {
        std::cerr << "erreur\n";
        m_pTrayIcon->showMessage("Teeworlds-notifier", "Erreur : "
            + m_pHTTP->errorString(), QSystemTrayIcon::Warning);
        m_pTrayIcon->show();
    }
    else
    {
        QByteArray page = m_pHTTP->readAll();
        QRegExp regexp("<td>(DM|TDM|CTF)</td>\\s+"
            "<td>(\\d+)/(\\d+)</td>\\s+"
            "<td>([^<]+)</td>");
        int pos = 0;
        QString msg;
        int nb = 0;
        while((pos = regexp.indexIn(page, pos)) != -1)
        {
            bool ok;
            if(regexp.cap(2).toInt(&ok, 10) > 0 && ok)
            {
                msg += QString("%1 joueurs sur %2 en %3\n").arg(regexp.cap(2))
                    .arg(regexp.cap(4)).arg(regexp.cap(1));
                nb++;
            }
            pos += regexp.matchedLength();
        }
        std::cerr << nb << " resultats\n";
        if(nb > 0)
        {
            static int old_nb = 0;
            if(nb != old_nb)
            {
                m_pBeep->play();
                old_nb = nb;
            }
            m_pTrayIcon->showMessage("Teeworlds-notifier", msg,
                QSystemTrayIcon::Information);
            m_pTrayIcon->show();
        }
    }
}
