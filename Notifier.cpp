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
#include "Urbanterror.h"
#include "Mumble.h"

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
    setWindowTitle(tr("Notifier"));

    // System tray icon creation
    QMenu *trayMenu = new QMenu(this);
    {
        QAction *refreshAction = new QAction(tr("Check"), this);
        connect(refreshAction, SIGNAL(triggered()),
            this, SIGNAL(refreshAll()));
        trayMenu->addAction(refreshAction);
    }
    {
        QAction *tellMe = new QAction(tr("Tell me again"), this);
        connect(tellMe, SIGNAL(triggered()),
            this, SLOT(tellAgain()));
        trayMenu->addAction(tellMe);
    }
    trayMenu->addSeparator();
    {
        QAction *quitAction = new QAction(tr("Quit"), this);
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        trayMenu->addAction(quitAction);
    }

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setContextMenu(trayMenu);
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
#ifdef __WIN32__
    m_IconEmpty = QIcon("icon.png");
    m_IconPlayers = QIcon("icon2.png");
    m_pBeep = new QSound("beep.wav");
#else
    m_IconEmpty = QIcon(PREFIX "/share/notifier/icon.png");
    m_IconPlayers = QIcon(PREFIX "/share/notifier/icon2.png");
    m_pBeep = new QSound(PREFIX "/share/notifier/beep.wab");
#endif
    m_pTrayIcon->setIcon(m_IconEmpty);
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
        addServer(new GameSpyServer("mario.rez-gif.supelec.fr", 7787),
            "UT2004 (mario)");
        addServer(new UrbanterrorServer("mario.rez-gif.supelec.fr", 27960),
            "Urbanterror (mario)");
        addServer(new MumbleServer("mario.rez-gif.supelec.fr", 64738),
            "Mumble (mario)");
    }
    catch(ServerError &e)
    {
        displayError(e.what());
    }
}

void Notifier::addServer(Server *serv, const QString &name)
{
    connect(this, SIGNAL(refreshAll()), serv, SLOT(refresh()));
    connect(serv, SIGNAL(infosChanged(int, int, QString, QString, bool)),
        this, SLOT(infosChanged(int, int, QString, QString, bool)));
    connect(serv, SIGNAL(errorEncountered(QString)),
        this, SLOT(displayError(QString)));
    m_aServers.insert(serv, name);
}

void Notifier::displayError(QString error)
{
    Server *serv = qobject_cast<Server*>(sender());
    QString name = serv?m_aServers[serv]:tr("(unknown origin)");
    Notification n = {name, tr("Error: ") + error};
    m_lErrors.append(n);
    if(!m_pMessageTimer->isActive())
        updateMessage();
}

void Notifier::appendNotification(QString name, unsigned int players,
    unsigned int max, QString map, QString mode)
{
    QString text;
    if(map.isEmpty())
    {
        if(mode.isEmpty())
        {
            if(max > 0)
                text = tr("%1/%2 players").arg(players).arg(max);
            else
                text = tr("%1 players").arg(players);
        }
        else
        {
            if(max > 0)
                text = tr("%1/%2 players in %4")
                    .arg(players).arg(max).arg(mode);
            else
                text = tr("%1 players in %4").arg(players).arg(mode);
        }
    }
    else
    {
        if(mode.isEmpty())
        {
            if(max > 0)
                text = tr("%1/%2 players on %3").arg(players).arg(max).arg(map);
            else
                text = tr("%1 players on %3").arg(players).arg(map);
        }
        else
        {
            if(max > 0)
                text = tr("%1/%2 players on %3 in %4")
                    .arg(players).arg(max).arg(map).arg(mode);
            else
                text = tr("%1 players on %3 in %4")
                    .arg(players).arg(map).arg(mode);
        }
    }
    Notification n = {name, text};
    m_lNotifications.append(n);
}

void Notifier::infosChanged(int players, int max, QString map, QString mode,
    bool gamestarted)
{
    Server *serv = qobject_cast<Server*>(sender());
    if(!serv)
        return ;
    // Queues the notification to be displayed later
    QString name = m_aServers[serv];
    appendNotification(name, players, max, map, mode);

    if(!m_pMessageTimer->isActive())
        updateMessage();

    // Plays a sound
    if(gamestarted)
        m_pBeep->play();

    // Updates the icon
    updateIcon();
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
    {
        m_pMessageTimer->stop();
        return ;
    }
    m_pMessageTimer->start();
}

void Notifier::flushNotifications()
{
    m_lNotifications.clear();
    m_lErrors.clear();
    m_pMessageTimer->stop();
}

void Notifier::updateIcon()
{
    bool players = false;
    QMap<Server*, QString>::const_iterator i = m_aServers.constBegin();
    while(i != m_aServers.constEnd())
    {
        if(i.key()->numPlayers() > 0)
        {
            players = true;
            break;
        }
        ++i;
    }
    if(players)
        m_pTrayIcon->setIcon(m_IconPlayers);
    else
        m_pTrayIcon->setIcon(m_IconEmpty);
}

void Notifier::tellAgain()
{
    flushNotifications();

    int p;
    // Servers with people playing, then the others
    for(p = 0; p <= 1; p++)
    {
        QMap<Server*, QString>::const_iterator i = m_aServers.constBegin();
        for(; i != m_aServers.constEnd(); ++i)
        {
            if( (p == 0 && i.key()->numPlayers() == 0)
             || (p == 1 && i.key()->numPlayers() > 0) )
                continue;
            else
                appendNotification(i.value(), i.key()->numPlayers(),
                    i.key()->maxPlayers(), i.key()->map(), i.key()->mode());
        }
    }

    if(!m_pMessageTimer->isActive())
        updateMessage();
}

void Notifier::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        tellAgain();
}
