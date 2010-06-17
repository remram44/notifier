#include "Server.h"

QMap<QLatin1String, ServerFactory*> ServerFactoryList::m_aFactories;

void ServerFactoryList::addServerFactory(ServerFactory *sf)
    throw(NonUniqueServerTypeNameError)
{
    ServerFactory *factory = m_aFactories.value(sf->name(), NULL);
    if(factory)
        throw NonUniqueServerTypeNameError();
    else
        m_aFactories[sf->name()] = sf;
}

Server *ServerFactoryList::createFromConfig(const QLatin1String &type,
    const QString &param)
{
    ServerFactory *factory = m_aFactories.value(type, NULL);
    if(factory)
        return factory->createFromConfig(param);
    else
        return NULL;
}
