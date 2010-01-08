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

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QStringList>
#include <QtDebug>

#include "Teeworlds.h"
#include "GameSpy.h"

ServerError::ServerError(const QString error)
  : w(error)
{
}

const char *ServerError::what() const throw()
{
    return w.toLocal8Bit();
}

unsigned int Server::maxPlayers() const
{
    return 0;
}

QString Server::map() const
{
    return QString();
}

QString Server::mode() const
{
    return QString();
}

Notifier::Notifier(QWidget *pParent)
  : QWidget::QWidget(pParent)
{
    setWindowTitle("Notifier");

    // System tray icon creation
    QMenu *trayMenu = new QMenu(this);
    {
        QAction *refreshAction = new QAction("V�rifier", this);
        connect(refreshAction, SIGNAL(triggered()),
            this, SIGNAL(refreshAll()));
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
    m_pTrayIcon->setIcon(QIcon(PREFIX "/share/notifier/icon.png"));
    m_pBeep = new QSound(PREFIX "/share/notifier/beep.wab");
#endif
    m_pTrayIcon->show();

    m_pBeep->setLoops(1);

    try {
        Server *tw = new TeeworldsServer("yoshi.rez-gif.supelec.fr", 8303);
        connect(this, SIGNAL(refreshAll()), tw, SLOT(refresh()));
        connect(tw, SIGNAL(infosChanged(int, int, QString, QString)),
            this, SLOT(infosChanged(int, int, QString, QString)));
        connect(tw, SIGNAL(errorEncountered(QString)),
            this, SLOT(displayError(QString)));
        m_aServers.insert(tw, "Teeworlds (DM)");
    }
    catch(ServerError &e)
    {
        displayError(e.what());
    }
    try {
        Server *ut = new GameSpyServer("mario.rez-gif.supelec.fr", 7787);
        connect(this, SIGNAL(refreshAll()), ut, SLOT(refresh()));
        connect(ut, SIGNAL(infosChanged(int, int, QString, QString)),
            this, SLOT(infosChanged(int, int, QString, QString)));
        connect(ut, SIGNAL(errorEncountered(QString)),
            this, SLOT(displayError(QString)));
        m_aServers.insert(ut, "UT2004 (mario)");
    }
    catch(ServerError &e)
    {
        displayError(e.what());
    }
}

// TODO : queue error messages to display all of them
// (flush on refreshAll())
void Notifier::displayError(QString error)
{
    Server *serv = qobject_cast<Server*>(sender());
    QString name = serv?m_aServers[serv]:"(origine inconnue)";
    m_pTrayIcon->showMessage(name, tr("Erreur : ") + error,
        QSystemTrayIcon::Warning);
}

// TODO : queue notifications to display all of them
// (flush on refreshAll())
void Notifier::infosChanged(int players, int max, QString map,
    QString mode)
{
    Server *serv = qobject_cast<Server*>(sender());
    if(!serv)
        return ;
    QString name = m_aServers[serv];
    if(max > 0)
        m_pTrayIcon->showMessage(name, QString("%1/%2 joueurs sur %3 en %4")
            .arg(players).arg(max).arg(map).arg(mode),
            QSystemTrayIcon::Information);
    else
        m_pTrayIcon->showMessage(name, QString("%1 joueurs sur %3 en %4")
            .arg(players).arg(map).arg(mode),
            QSystemTrayIcon::Information);
    m_pBeep->play();
}
