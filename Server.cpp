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

ServerFactory::ServerFactory(QLatin1String sName)
  : m_sName(sName)
{
    ServerFactoryList::addServerFactory(this);
}
