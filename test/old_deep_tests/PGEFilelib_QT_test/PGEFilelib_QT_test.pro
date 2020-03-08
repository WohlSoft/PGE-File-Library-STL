QT += core
QT -= gui

TARGET = PGEFilelib_QT_test
CONFIG += console c++11
CONFIG -= app_bundle
INCLUDEPATH += ../../..

TEMPLATE = app
DEFINES += PGE_FILES_QT

SOURCES += $$files($$PWD/../../../*.cpp)
SOURCES += $$files($$PWD/../../../*.c)
HEADERS  += $$files($$PWD/../../../*.h)

DESTDIR += $$PWD

SOURCES += main.cpp
