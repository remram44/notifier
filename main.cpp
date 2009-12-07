#include <QApplication>
#include "Notifier.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Notifier teeworlds_notifier;

    return app.exec();
}
