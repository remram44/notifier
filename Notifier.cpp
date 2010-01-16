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
        QAction *refreshAction = new QAction("Vérifier", this);
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
#ifdef __WIN32__
    m_pTrayIcon->setIcon(QIcon("icon.png"));
    m_pBeep = new QSound("beep.wav");
#else
    m_pTrayIcon->setIcon(QIcon(PREFIX "/share/notifier/icon.png"));
    m_pBeep = new QSound(PREFIX "/share/notifier/beep.wab");
#endif
    m_pTrayIcon->show();

    m_pBeep->setLoops(1);

    connect(this, SIGNAL(refreshAll()), this, SLOT(flushNotifications()));

    m_pMessageTimer = new QTimer(this);
    connect(m_pMessageTimer, SIGNAL(timeout()), this, SLOT(updateMessage()));
    m_pMessageTimer->setSingleShot(false);
    m_pMessageTimer->setInterval(10000);

    try {
        addServer(new TeeworldsServer("yoshi.rez-gif.supelec.fr", 8303),
            "Teeworlds (DM)");
        addServer(new TeeworldsServer("yoshi.rez-gif.supelec.fr", 8304),
            "Teeworlds (TDM)");
        addServer(new TeeworldsServer("yoshi.rez-gif.supelec.fr", 8305),
            "Teeworlds (CTF)");
    }
    catch(ServerError &e)
    {
        displayError(e.what());
    }
    try {
        addServer(new GameSpyServer("mario.rez-gif.supelec.fr", 7787),
            "UT2004 (mario)");
    }
    catch(ServerError &e)
    {
        displayError(e.what());
    }
}

void Notifier::addServer(Server *serv, const QString &name)
{
    connect(this, SIGNAL(refreshAll()), serv, SLOT(refresh()));
    connect(serv, SIGNAL(infosChanged(int, int, QString, QString)),
        this, SLOT(infosChanged(int, int, QString, QString)));
    connect(serv, SIGNAL(errorEncountered(QString)),
        this, SLOT(displayError(QString)));
    m_aServers.insert(serv, name);
}

void Notifier::displayError(QString error)
{
    Server *serv = qobject_cast<Server*>(sender());
    QString name = serv?m_aServers[serv]:"(origine inconnue)";
    Notification n = {name, QString("Erreur : ") + error};
    m_lErrors.append(n);
    if(!m_pMessageTimer->isActive())
    {
        updateMessage();
        m_pMessageTimer->start();
    }
}

void Notifier::infosChanged(int players, int max, QString map,
    QString mode)
{
    Server *serv = qobject_cast<Server*>(sender());
    if(!serv)
        return ;
    QString name = m_aServers[serv];
    if(max > 0)
    {
        Notification n = {name, QString("%1/%2 joueurs sur %3 en %4")
            .arg(players).arg(max).arg(map).arg(mode)};
        m_lNotifications.append(n);
    }
    else
    {
        Notification n = {name, QString("%1 joueurs sur %3 en %4")
            .arg(players).arg(map).arg(mode)};
        m_lNotifications.append(n);
    }
    if(!m_pMessageTimer->isActive())
    {
        updateMessage();
        m_pMessageTimer->start();
    }
}

void Notifier::updateMessage()
{
    if(!m_lErrors.isEmpty())
    {
        Notification n = m_lErrors.takeFirst();
        m_pTrayIcon->showMessage(n.title, n.message, QSystemTrayIcon::Warning);
    }
    else if(!m_lNotifications.isEmpty())
    {
        Notification n = m_lNotifications.takeFirst();
        m_pTrayIcon->showMessage(n.title, n.message,
            QSystemTrayIcon::Information);
    }
    else
        m_pMessageTimer->stop();
}

void Notifier::flushNotifications()
{

}
