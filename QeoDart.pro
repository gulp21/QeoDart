######################################################################
# Automatically generated by qmake (2.01a) Mi. Jul 20 07:40:21 2011
######################################################################

TEMPLATE = app
DEPENDPATH += . cpp ui
INCLUDEPATH += . cpp

# Input
HEADERS += cpp/dart.hpp cpp/io.hpp cpp/myLabels.hpp cpp/qtwin.h \
    cpp/preferences.hpp \
    cpp/results.hpp \
    cpp/highscores.hpp \
    cpp/about.hpp \
    cpp/network.hpp
FORMS += ui/mainWindow.ui \
    ui/resultWindow.ui \
    ui/preferences.ui \
    ui/highscore.ui \
    ui/about.ui
SOURCES += cpp/dart.cpp cpp/io.cpp cpp/main.cpp cpp/qtwin.cpp \
    cpp/preferences.cpp \
    cpp/results.cpp \
    cpp/highscores.cpp \
    cpp/about.cpp \
    cpp/network.cpp
QT += xml network

TRANSLATIONS = lang/de.ts \
    lang/la.ts \
    lang/en.ts

CODECFORTR = UTF-8

QMAKE_CXXFLAGS_WARN_OFF += -Wno-reorder

RESOURCES += \
    icons.qrc

TARGET = qeodart
target.path = /usr/bin
INSTALLS += target
langfiles.path += /usr/share/QeoDart/lang
langfiles.files += lang/*.qm
qcf.path += /usr/share/QeoDart/qcf
qcf.files += qcf/*
icon.path +=  /usr/share/pixmaps
icon.files += icons/qeodart.svg
desktop.path +=  /usr/share/applications
desktop.files += QeoDart.desktop
INSTALLS += langfiles
INSTALLS += qcf
INSTALLS += icon
INSTALLS += desktop

