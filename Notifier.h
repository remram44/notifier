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
#include <QMap>
#include <QTimer>

#ifndef PREFIX
#define PREFIX "/usr/local"
#endif

/**
 * Utility function.
 *
 * Indicate whether the assignment changes the destination's value.
 */
template<typename T>
bool confirm_assign(T *dst, const T &src)
{
    if(*dst != src)
    {
        *dst = src;
        return true;
    }
    else
        return false;
}

class ServerError : public std::exception {
private:
    QString w;

public:
    ServerError(const QString error);
    virtual const char *what() const throw();
    virtual ~ServerError() throw() {}
};

/**
 * A server we can query for players information.
 */
class Server : public QObject {

    Q_OBJECT

public:
    /**
     * The number of players currently in-game.
     */
    virtual unsigned int numPlayers() const = 0;

    /**
     * The maximum number of players that can join the game, or 0.
     */
    virtual unsigned int maxPlayers() const;

    /**
     * The map's name, if relevant, or QString().
     */
    virtual QString map() const;

    /**
     * The game mode, if relevant, or QString().
     */
    virtual QString mode() const;

public slots:
    /**
     * Forcibly update the information by querying the server.
     */
    virtual void refresh() = 0;

signals:
    /**
     * Signal emitted when the informations have changed.
     *
     * @param gamestarted Boolean indicating whether the game just started, ie
     * there was no player before and there are players now. It should therefore
     * never be set two consecutive times.
     */
    void infosChanged(unsigned int players, unsigned int max, QString map,
        QString mode, bool gamestarted);

    /**
     * Signal emitted on errors.
     */
    void errorEncountered(QString text);

};

/**
 * A server's notification configuration.
 */
struct ServerConf {

    QString name;
    bool play_sound;
    bool change_color;
    bool display_popup;

    inline ServerConf() {}

    inline ServerConf(QString p_Name, bool p_Sound, bool p_Color, bool p_Popup)
      : name(p_Name), play_sound(p_Sound), change_color(p_Color),
        display_popup(p_Popup)
    {
    }

};

/**
 * The notifier.
 */
class Notifier : public QWidget {

    Q_OBJECT

private:
    QSystemTrayIcon *m_pTrayIcon;
    QSound *m_pBeep;

    QMap<Server*, ServerConf> m_aServers;

    struct Notification {
        QString title;
        QString message;
    };
    QList<Notification> m_lNotifications;
    QList<Notification> m_lErrors;
    QTimer *m_pMessageTimer;
    QIcon m_IconEmpty;
    QIcon m_IconPlayers;

private:
    void addServer(Server *serv, const ServerConf &conf);
    void appendNotification(QString name, unsigned int players,
        unsigned int max, QString map, QString mode);

private slots:
    void displayError(QString error);
    void infosChanged(int players, int max, QString map, QString mode,
        bool gamestarted);
    void updateMessage();
    void flushNotifications();
    void updateIcon();
    void tellAgain();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

public:
    Notifier(QWidget *pParent = NULL);

signals:
    void refreshAll();

};

#endif
