#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += C++11

TARGET = qcsamunlock

TEMPLATE = app

INCLUDEPATH += $$PWD/../lib/serial/include $$PWD/../src

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    src/util/hexdump.cpp \
    src/gui/application.cpp \
    src/gui/qcsamunlock_window.cpp \
    src/gui/about_dialog.cpp \
    src/qcsamunlock.cpp

HEADERS  += \
    src/util/hexdump.h \
    src/gui/qcsamunlock_window.h \
    src/gui/about_dialog.h \
    src/gui/application.h 


FORMS  += resources/ui/qcsamunlock_window.ui
FORMS  += resources/ui/about_dialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/  -llibopenpst -lserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -llibopenpst -lserial
else:unix: LIBS += -L$$OUT_PWD/ -llibopenpst -lserial
