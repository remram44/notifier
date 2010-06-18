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

#include "Server.h"

// Empty, needed to make it private (DP singleton)
ServerFactoryList::ServerFactoryList()
{
}

ServerFactoryList *ServerFactoryList::getInstance()
{
    static ServerFactoryList *sfl = NULL;
    if(sfl == NULL)
        sfl = new ServerFactoryList;
    return sfl;
}

void ServerFactoryList::addServerFactory(ServerFactory *sf)
    throw(NonUniqueServerTypeNameError)
{
    QMap<QLatin1String, ServerFactory*> &factories =
        getInstance()->m_aFactories;
    ServerFactory *factory = factories.value(sf->name(), NULL);
    if(factory)
        throw NonUniqueServerTypeNameError();
    else
        factories[sf->name()] = sf;
}

Server *ServerFactoryList::createFromConfig(const QLatin1String &type,
    const QString &param)
{
    ServerFactory *factory = getInstance()->m_aFactories.value(type, NULL);
    if(factory)
        return factory->createFromConfig(param);
    else
        return NULL;
}
