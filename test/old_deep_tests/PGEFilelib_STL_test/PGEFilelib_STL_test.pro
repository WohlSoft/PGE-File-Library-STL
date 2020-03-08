TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../..
DESTDIR += $$PWD

DEFINES -= PGE_FILES_QT

SOURCES += $$files($$PWD/../../../*.cpp)
SOURCES += $$files($$PWD/../../../*.c)
HEADERS += $$files($$PWD/../../../*.h)

SOURCES += main.cpp

HEADERS += \
    dirent/dirent.h
