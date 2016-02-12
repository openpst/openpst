#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += C++11

TARGET = mbn_tool

TEMPLATE = app

INCLUDEPATH += $$PWD/../src

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    src/util/hexdump.cpp \
    src/util/endian.cpp \
    src/gui/mbn_tool_window.cpp \
    src/gui/application.cpp \
    src/mbn_tool.cpp

HEADERS  += \
    src/include/definitions.h \
    src/qc/mbn.h \
    src/util/hexdump.h \
    src/util/endian.h \
    src/gui/mbn_tool_window.h \
    src/gui/application.h 


FORMS  += resources/ui/mbn_tool_window.ui

RESOURCES = resources/mbn_tool.qrc
