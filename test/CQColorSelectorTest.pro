TEMPLATE = app

TARGET = CQColorSelectorTest

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
CQColorSelectorTest.cpp \

HEADERS += \
CQColorSelectorTest.h \

DESTDIR     = .
OBJECTS_DIR = .

INCLUDEPATH += \
../include \
.

unix:LIBS += \
-L../lib \
-lCQColorSelector
