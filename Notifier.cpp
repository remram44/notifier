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
#include <QFile>
#include <QDir>
#include <QRegExp>

MonitoredServer::MonitoredServer(QString p_Name, bool p_Sound, bool p_Color,
    bool p_Popup, Server *p_Server)
  : name(p_Name), play_sound(p_Sound), change_color(p_Color),
    display_popup(p_Popup), server(p_Server)
{
    connect(server,
        SIGNAL(infosChanged(unsigned int, unsigned int, QString, QString,
            bool)),
        this,
        SIGNAL(infosChanged(unsigned int, unsigned int, QString, QString,
            bool)));
    connect(server, SIGNAL(errorEncountered(QString)),
        this, SIGNAL(errorEncountered(QString)));
    connect(this, SIGNAL(refresh()), server, SLOT(refresh()));
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

    // Configuration dialog
    m_pConfigDialog = new ConfigDialog(this);

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
    {
        QAction *config = new QAction(tr("Configure servers"), this);
        connect(config, SIGNAL(triggered()),
            m_pConfigDialog, SLOT(show()));
        trayMenu->addAction(config);
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
    m_IconEmpty = QIcon(SHARE_DIR "/icon.png");
    m_IconPlayers = QIcon(SHARE_DIR "/icon2.png");
    m_pBeep = new QSound(SHARE_DIR "/beep.wab");
#endif
    m_pTrayIcon->setIcon(m_IconEmpty);
    m_pTrayIcon->show();

    m_pBeep->setLoops(1);

    connect(this, SIGNAL(refreshAll()), this, SLOT(flushNotifications()));

    m_pMessageTimer = new QTimer(this);
    connect(m_pMessageTimer, SIGNAL(timeout()), this, SLOT(updateMessage()));
    m_pMessageTimer->setSingleShot(false);
    m_pMessageTimer->setInterval(10000);

    // Try to open the configuration file
    bool has_conf = false;
#ifdef __WIN32__
    QFile conf(QDir::toNativeSeparators(QDir::homePath() + "/notifier.conf"));
    if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
        has_conf = true;
    else
    {
        conf.setFileName("default.conf");
        if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
            has_conf = true;
    }
#else
    QFile conf(QDir::homePath() + "/.notifierrc");
    if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
        has_conf = true;
    else
    {
        conf.setFileName(PREFIX "/share/notifier/default.conf");
        if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
            has_conf = true;
    }
#endif
    if(has_conf)
    {
        unsigned int lineNumber = 1;
        QByteArray line = conf.readLine(2048);
        while(!line.isEmpty())
        {
            line = line.left(line.size() - 1);
            QRegExp reg("^([a-z]+) \"(.+)\" "
                "(s|n) (c|n) (p|n)");
            if(reg.indexIn(line) != -1)
            {
                Server *serv = NULL;
                QString type = reg.cap(1);
                QString param;
                try {
                    if(reg.matchedLength()+2 <= line.size())
                        param = line.mid(reg.matchedLength()+1);
                    serv = ServerFactoryList::createFromConfig(
                        QLatin1String(type.toLatin1().constData()), // FIXME
                        param);
                    if(!serv)
                        displayError(tr("Unknown server type in configuration "
                            "file: \"%3\", %1, line %2").arg(conf.fileName())
                            .arg(lineNumber).arg(type));
                }
                catch(ServerError &e)
                {
                    displayError(e.what());
                }

                if(serv != NULL)
                    addServer(new MonitoredServer(
                            reg.cap(2), // name
                            reg.cap(3) == "s", // sound
                            reg.cap(4) == "c", // color
                            reg.cap(5) == "p", // popup
                            serv
                            ));
            }
            else
                displayError(tr("Error reading configuration file: %1, line %2")
                    .arg(conf.fileName()).arg(lineNumber));

            line = conf.readLine(2048);
            lineNumber++;
        }
    }
}

void Notifier::addServer(MonitoredServer *mserv)
{
    Server *const serv = mserv->server;
    connect(this, SIGNAL(refreshAll()), serv, SLOT(refresh()));
    connect(serv, SIGNAL(infosChanged(int, int, QString, QString, bool)),
        this, SLOT(infosChanged(int, int, QString, QString, bool)));
    connect(serv, SIGNAL(errorEncountered(QString)),
        this, SLOT(displayError(QString)));
    m_lServers.push_back(mserv);
}

void Notifier::displayError(QString error)
{
    qDebug() << error;
    MonitoredServer *mserv = qobject_cast<MonitoredServer*>(sender());
    QString name = mserv?mserv->name:tr("Error");
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
    MonitoredServer *mserv = qobject_cast<MonitoredServer*>(sender());
    if(!mserv)
        return ;
    // Queues the notification to be displayed later
    if(mserv->display_popup)
    {
        appendNotification(mserv->name, players, max, map, mode);
        if(!m_pMessageTimer->isActive())
            updateMessage();
    }

    // Plays a sound
    if(gamestarted && mserv->play_sound)
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
    QList<MonitoredServer*>::const_iterator i = m_lServers.constBegin();
    while(i != m_lServers.constEnd())
    {
        if((*i)->numPlayers() > 0 && (*i)->change_color)
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
        QList<MonitoredServer*>::const_iterator i = m_lServers.constBegin();
        for(; i != m_lServers.constEnd(); ++i)
        {
            if( (p == 0 && (*i)->numPlayers() == 0)
             || (p == 1 && (*i)->numPlayers() > 0) )
                continue;
            else
                appendNotification((*i)->name, (*i)->numPlayers(),
                    (*i)->maxPlayers(), (*i)->map(), (*i)->mode());
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
