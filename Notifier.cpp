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
#include <QTimer>
#include <QStringList>
#include <QtDebug>

#include "Teeworlds.h"

Notifier::Notifier(QWidget *pParent)
  : QWidget::QWidget(pParent)
{
    setWindowTitle("Teeworlds-Notifier");

    // System tray icon creation
    QMenu *trayMenu = new QMenu(this);
    {
        QAction *refreshAction = new QAction("Vérifier", this);
        connect(refreshAction, SIGNAL(triggered()),
            this, SIGNAL(forceRefreshAll()));
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

    {
        Server *tw = new TeeworldsHtml("yoshi.rez-gif.supelec.fr", 80, "/tw/");
        connect(this, SIGNAL(refreshAll()), tw, SLOT(refresh()));
        connect(this, SIGNAL(forceRefreshAll()), tw, SLOT(forceRefresh()));
        connect(tw, SIGNAL(infosChanged(QString, int, int, QString, QString)),
            this, SLOT(infosChanged(QString, int, int, QString, QString)));
        connect(tw, SIGNAL(errorEncountered(QString)),
            this, SLOT(displayError(QString)));
        m_Servers.insert(tw);
    }

    refreshAll();

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(refreshAll()));
    timer->start(30000);
}

void Notifier::displayError(QString error)
{
    m_pTrayIcon->showMessage("Teeworlds-notifier", error,
        QSystemTrayIcon::Warning);
}

void Notifier::infosChanged(QString game, int players, int max, QString map,
    QString mode)
{
    if(max > 0)
        m_pTrayIcon->showMessage(game, QString("%1/%2 joueurs sur %3 en %4")
            .arg(players).arg(max).arg(map).arg(mode),
            QSystemTrayIcon::Information);
    else
        m_pTrayIcon->showMessage(game, QString("%1 joueurs sur %3 en %4")
            .arg(players).arg(map).arg(mode),
            QSystemTrayIcon::Information);
}
