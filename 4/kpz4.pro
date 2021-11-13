SOURCES = \ 
    src/main.cpp \
    src/MainWindow.cpp

HEADERS = \
    src/MainWindow.h \
    src/marshalling.h \
    src/util.h

FORMS = \
    src/MainWindow.ui

TARGET = main

QMAKE_CXXFLAGS += -w

QT += widgets

INCLUDEPATH += /home/dazzlemon/mxe/usr/i686-w64-mingw32.static/include