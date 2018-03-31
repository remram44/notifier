# Notifier -- Desktop tool providing alerting of player presence on game servers

## Description

Notifier is a small program appearing in the notification area of your system, and notifying you (via a sound, the change of color of the icon and tooltips) when players join one of the monitored game servers.

At the moment, Notifier is configured (through the included configuration file) to monitor the game servers of Supélec Rézo. A future version will allow you to freely configure the servers to query through a graphical interface.

## TODO-LIST

* Display all the information in one tooltip, instead of showing one for each event.
* Add the possibility to configure which servers to monitor, with the associated icons and sounds.

## Compilation

This program uses Qt's build system, qmake. Use "qmake" to generate a Makefile, and you can then build the binaries with "make".

## Installation (Windows)

Place all the files of the archive somewhere and open notifier.exe. If you want this program to be run automatically each boot, toggle the corresponding checkbox in the context menu.
