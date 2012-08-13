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
    cpp/about.hpp
FORMS += ui/mainWindow.ui \
    ui/resultWindow.ui \
    ui/preferences.ui \
    ui/highscore.ui \
    ui/about.ui
SOURCES += cpp/dart.cpp cpp/io.cpp cpp/main.cpp cpp/qtwin.cpp \
    cpp/preferences.cpp \
    cpp/results.cpp \
    cpp/highscores.cpp \
    cpp/about.cpp
QT += xml

TRANSLATIONS = lang/de.ts \
    lang/la.ts \
    lang/en.ts

CODECFORTR = UTF-8

QMAKE_CXXFLAGS_WARN_OFF += -Wno-reorder

RESOURCES += \
    icons.qrc

RC_FILE = QeoDart.rc

TARGET = qeodart
target.path = /usr/bin
INSTALLS += target
langfiles.path += /usr/share/QeoDart/lang
langfiles.files += lang/*.qm
qcf.path += /usr/share/QeoDart/qcf
qcf.files += qcf/*
icon.path +=  /usr/share/icons/hicolor/scalable/apps
icon.files += icons/qeodart.svg
icon256.path +=  /usr/share/icons/hicolor/256x256/apps
icon256.files += icons/256x256/qeodart.png
icon48.path +=  /usr/share/icons/hicolor/48x48/apps
icon48.files += icons/48x48/qeodart.png
icon16.path +=  /usr/share/icons/hicolor/16x16/apps
icon16.files += icons/16x16/qeodart.png
desktop.path +=  /usr/share/applications
desktop.files += QeoDart.desktop
INSTALLS += langfiles
INSTALLS += qcf
INSTALLS += icon
INSTALLS += icon256
INSTALLS += icon48
INSTALLS += icon16
INSTALLS += desktop

