######################################################################
# Automatically generated by qmake (2.01a) dim. mai 6 14:40:11 2007
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += release
DESTDIR = ./

QT += network

# Program
HEADERS = Notifier.h Server.h ConfigDialog.h
SOURCES = Notifier.cpp main.cpp ConfigDialog.cpp
FORMS = ConfigDialog.ui
RC_FILE = notifier.rc

# Server modules
HEADERS += Teeworlds.h GameSpy.h Urbanterror.h Mumble.h
SOURCES += Teeworlds.cpp GameSpy.cpp Urbanterror.cpp Mumble.cpp

TRANSLATIONS = notifier_fr.ts
CODECFORTR = ISO-8859-1
