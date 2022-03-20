SOURCES = \ 
    src/main.cpp \
    src/mainwindow.cpp

HEADERS = \
    src/mainwindow.hpp \
    src/postfix_evaluator.hpp

FORMS = \
    src/mainwindow.ui

TARGET = main

QMAKE_CXXFLAGS += -w

QT += widgets

INCLUDEPATH += /home/dazzlemon/mxe/usr/i686-w64-mingw32.static/include
