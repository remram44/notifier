#include "Server.h"

#include <QtDebug>
QMap<QLatin1String, ServerFactory*> *ServerFactoryList::m_aFactories;

void ServerFactoryList::addServerFactory(ServerFactory *sf)
    throw(NonUniqueServerTypeNameError)
{
    if (m_aFactories == 0)
        m_aFactories = new QMap<QLatin1String, ServerFactory*>;
    ServerFactory *factory = m_aFactories->value(sf->name(), NULL);
    if(factory)
        throw NonUniqueServerTypeNameError();
    else
        (*m_aFactories)[sf->name()] = sf;
}

Server *ServerFactoryList::createFromConfig(const QLatin1String &type,
    const QString &param)
{
    if (m_aFactories == 0)
        m_aFactories = new QMap<QLatin1String, ServerFactory*>;
    ServerFactory *factory = m_aFactories->value(type, NULL);
    if(factory)
        return factory->createFromConfig(param);
    else
        return NULL;
}

ServerFactory::ServerFactory()
{
}
