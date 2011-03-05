#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "Notifier.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load(QString(":/notifier_") + QLocale::system().name());
    app.installTranslator(&translator);

    Notifier notifier;

#ifdef WIN32
    notifier.setExecutablePath(*argv);
#endif

    return app.exec();
}
