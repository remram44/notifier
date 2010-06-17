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

#ifndef SERVER_H
#define SERVER_H

#include <exception>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QMap>

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

class ServerConfWidget : public QWidget {

    Q_OBJECT

public slots:
    virtual void applyChanges() = 0;

};

/**
 * Fatal error during a Server setup.
 */
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

    /**
     * Returns the configuration widget for this Server.
     */
    virtual ServerConfWidget *confWidget() = 0;

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

class ServerFactory;

/**
 * Singleton registering all the ServerFactories.
 */
class ServerFactoryList {

private:
    static QMap<QLatin1String, ServerFactory*> *m_aFactories;

public:
    class NonUniqueServerTypeNameError {};

    static void addServerFactory(ServerFactory *sf)
        throw(NonUniqueServerTypeNameError);

    static Server *createFromConfig(const QLatin1String &type,
        const QString &param);

};

/**
 * ServerFactory is subclassed for every Server subclass, and a static instance
 * is created and registers with ServerFactoryList.
 *
 * These objects are responsible for the creation of new Servers of the
 * associated type.
 */
class ServerFactory {

public:
    /**
     * Constructor, registers this ServerFactory to the ServerFactoryList.
     */
    ServerFactory();

    /**
     * Create an empty configuration widget with no associated Server.
     */
    virtual ServerConfWidget *newConfWidget() const = 0;

    /**
     * Displayed name of this Server (i.e. localized).
     */
    virtual QString displayName() const = 0;

    /**
     * Internal name of this Server type.
     *
     * @warning Mustn't contain spaces.
     */
    virtual QLatin1String name() const = 0;

    /**
     * Create a Server from the config file, at startup.
     */
    virtual Server *createFromConfig(const QString &line) const = 0;

};

template <class T>
class ServerFactoryRegistrar {
    T serverFactory;
public:
    ServerFactoryRegistrar<T>() {
        ServerFactoryList::addServerFactory(&serverFactory);
    }
};

#endif
