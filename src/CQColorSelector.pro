TEMPLATE = lib

TARGET = CQColorSelector

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++14

CONFIG += staticlib

# Input
HEADERS += \
../include/CQColorSelector.h \

SOURCES += \
CQColorSelector.cpp \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
. \
../include \
