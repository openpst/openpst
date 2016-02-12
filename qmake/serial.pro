#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

CONFIG += C++11 staticlib

TARGET = serial

TEMPLATE = lib

INCLUDEPATH += $$PWD/../src $$PWD/../lib/serial/include

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    lib/serial/src/serial.cc

HEADERS  += \
    lib/serial/include/serial/serial.h \
    lib/serial/include/serial/v8stdint.h

linux:unix:!macx {
    SOURCES += \
        lib/serial/src/impl/unix.cc \
        lib/serial/src/impl/list_ports/list_ports_linux.cc

    HEADERS += \
        lib/serial/include/serial/impl/unix.h
}

win32 {
    SOURCES += \
        lib/serial/src/impl/win.cc \
        lib/serial/src/impl/list_ports/list_ports_win.cc

    HEADERS += \
        lib/serial/include/serial/impl/win.h \
        src/include/win_inttypes.h \
        src/include/win_stdint.h
}

macx {
    SOURCES += \
        lib/serial/src/impl/unix.cc \
        lib/serial/src/impl/list_ports/list_ports_osx.cc

    HEADERS += \
        lib/serial/include/serial/impl/unix.h
}
