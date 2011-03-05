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

#ifdef _WIN32
#    include <QMessageBox>
#    include <windows.h>
#endif

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
#ifdef _WIN32
    trayMenu->addSeparator();
    {
        m_pAutoStart = new QAction(tr("Start automatically"), this);
        m_pAutoStart->setCheckable(true);
        m_pAutoStart->setChecked(false);
        loadAutoStartState();
        connect(m_pAutoStart, SIGNAL(triggered()),
            this, SLOT(setAutoStart()));
        trayMenu->addAction(m_pAutoStart);
    }
#endif
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
    m_IconEmpty = QIcon(":/icon.png");
    m_IconPlayers = QIcon(":/icon2.png");
    m_pBeep = new QSound(":/beep.wav");
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
        conf.setFileName(":/default.conf");
        if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
            has_conf = true;
    }
#else
    QFile conf(QDir::homePath() + "/.notifierrc");
    if(conf.open(QIODevice::ReadOnly | QIODevice::Text))
        has_conf = true;
    else
    {
        conf.setFileName(":/default.conf");
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
                    if(type == "teeworlds")
                        serv = new TeeworldsServer(param);
                    else if(type == "gamespy")
                        serv = new GameSpyServer(param);
                    else if(type == "urbanterror")
                        serv = new UrbanterrorServer(param);
                    else if(type == "mumble")
                        serv = new MumbleServer(param);
                    else
                        displayError(tr("Unknown server type in configuration "
                            "file: \"%3\", %1, line %2").arg(conf.fileName())
                            .arg(lineNumber).arg(type));
                }
                catch(ServerError &e)
                {
                    displayError(e.what());
                }

                if(serv != NULL)
                    addServer(serv,
                        ServerConf(reg.cap(2), // name
                            reg.cap(3) == "s", // sound
                            reg.cap(4) == "c", // color
                            reg.cap(5) == "p"  // popup
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

#ifdef _WIN32
void Notifier::setExecutablePath(const char *path)
{
    m_sPath = path;
}
#endif

void Notifier::addServer(Server *serv, const ServerConf &conf)
{
    connect(this, SIGNAL(refreshAll()), serv, SLOT(refresh()));
    connect(serv, SIGNAL(infosChanged(int, int, QString, QString, bool)),
        this, SLOT(infosChanged(int, int, QString, QString, bool)));
    connect(serv, SIGNAL(errorEncountered(QString)),
        this, SLOT(displayError(QString)));
    m_aServers.insert(serv, conf);
}

void Notifier::displayError(QString error)
{
    qDebug() << error;
    Server *serv = qobject_cast<Server*>(sender());
    QString name = serv?m_aServers[serv].name:tr("Error");
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
    if(m_aServers[serv].display_popup)
    {
        appendNotification(m_aServers[serv].name, players, max, map, mode);
        if(!m_pMessageTimer->isActive())
            updateMessage();
    }

    // Plays a sound
    if(gamestarted && m_aServers[serv].play_sound)
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
    QMap<Server*, ServerConf>::const_iterator i = m_aServers.constBegin();
    while(i != m_aServers.constEnd())
    {
        if(i.key()->numPlayers() > 0 && i.value().change_color)
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
        QMap<Server*, ServerConf>::const_iterator i = m_aServers.constBegin();
        for(; i != m_aServers.constEnd(); ++i)
        {
            if( (p == 0 && i.key()->numPlayers() == 0)
             || (p == 1 && i.key()->numPlayers() > 0) )
                continue;
            else
                appendNotification(i.value().name, i.key()->numPlayers(),
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

#ifdef _WIN32
#    define REG_WINDOWS_RUN "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#    define REG_VALUE_NAME "Notifier"

void Notifier::loadAutoStartState()
{
    HKEY hKey;
    if(RegOpenKeyEx(
        HKEY_CURRENT_USER,
        TEXT(REG_WINDOWS_RUN),
        0,
        KEY_QUERY_VALUE,
        &hKey
    ) != ERROR_SUCCESS)
        return ; // This is weird ; restricted permissions?

    if(RegQueryValueEx(
        hKey,
        TEXT(REG_VALUE_NAME),
        NULL,
        NULL,
        NULL, // ignore value
        NULL // size
    ) == ERROR_SUCCESS)
        m_pAutoStart->setChecked(true);

    RegCloseKey(hKey);
}

void Notifier::setAutoStart()
{
    bool start = m_pAutoStart->isChecked();

    HKEY hKey;
    if(RegOpenKeyEx(
        HKEY_CURRENT_USER,
        TEXT(REG_WINDOWS_RUN),
        0,
        KEY_ALL_ACCESS,
        &hKey
    ) != ERROR_SUCCESS)
    {
        QMessageBox::warning(this, tr("Error"),
                tr("Can't access the registry"));
        return ;
    }

    if(start)
    {
        // Get executable filename
        wchar_t cwd[1024];
        GetCurrentDirectory(1024, cwd);
        QString fullname = QString("\"") + QString::fromStdWString(cwd) + "\\notifier.exe\"";
        const char *path = fullname.toAscii();

        // Convert to UTF-16
        int size;
#ifdef UNICODE
        size = MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED,
            path,
            -1,
            NULL,
            0
        );
        wchar_t *buffer = new wchar_t[size];
        MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED,
            path,
            -1,
            buffer,
            size
        );
        size *= 2;
#else
        char *buffer = path;
        size = strlen(path)+1;
#endif

        if(RegSetValueEx(
            hKey,
            TEXT(REG_VALUE_NAME),
            0,
            REG_SZ,
            (const unsigned char *)buffer,
            size
        ) != ERROR_SUCCESS)
            QMessageBox::warning(this, tr("Error"),
                    tr("Can't write to the registry"));
    }
    else
    {
        if(RegDeleteValue(
            hKey,
            TEXT(REG_VALUE_NAME)
        ) != ERROR_SUCCESS)
            QMessageBox::warning(this, tr("Error"),
                    tr("Can't write to the registry"));
    }

    RegCloseKey(hKey);
}
#endif
