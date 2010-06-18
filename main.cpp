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

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "Notifier.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    translator.load(QString("notifier_") + QLocale::system().name());
    app.installTranslator(&translator);

    Notifier teeworlds_notifier;

    return app.exec();
}
