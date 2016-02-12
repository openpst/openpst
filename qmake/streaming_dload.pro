#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += C++11

TARGET = streaming_dload

TEMPLATE = app

INCLUDEPATH += $$PWD/../src $$PWD/../lib/serial/include

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    src/util/hexdump.cpp \
    src/gui/streaming_dload_window.cpp \
    src/gui/worker/streaming_dload_read_worker.cpp \
    src/gui/worker/streaming_dload_stream_write_worker.cpp \
    src/gui/application.cpp \
    src/streaming_dload.cpp

HEADERS  += \
    src/include/definitions.h \
    src/util/hexdump.h \
    src/gui/streaming_dload_window.h \
    src/gui/worker/streaming_dload_read_worker.h \
    src/gui/worker/streaming_dload_stream_write_worker.h \
    src/gui/application.h


FORMS  += resources/ui/streaming_dload_window.ui \
    ../resources/ui/progress_group_widget.ui

RESOURCES = resources/streaming_dload.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/  -llibopenpst -lserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -llibopenpst -lserial
else:unix: LIBS += -L$$OUT_PWD/ -llibopenpst -lserial
