#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += C++11

TARGET = qcdm

TEMPLATE = app

INCLUDEPATH += $$PWD/../lib/serial/include $$PWD/../src

DEPENDPATH += $$PWD/../

VPATH += $$PWD/../

SOURCES += \
    src/util/hexdump.cpp \
    src/gui/application.cpp \
    src/gui/qcdm_window.cpp \
    src/gui/about_dialog.cpp \
    src/gui/worker/qcdm_efs_directory_tree_worker.cpp \
    src/gui/worker/qcdm_efs_file_read_worker.cpp \
    src/gui/worker/qcdm_efs_file_write_worker.cpp \
    src/gui/worker/qcdm_memory_read_worker.cpp \
    src/gui/worker/qcdm_prl_read_worker.cpp \
    src/gui/worker/qcdm_prl_write_worker.cpp \
    src/gui/worker/qcdm_nv_item_read_worker.cpp \
    src/qcdm.cpp

HEADERS  += \
    src/util/hexdump.h \
    src/gui/qcdm_window.h \
    src/gui/about_dialog.h \
    src/worker/gui/qcdm_efs_directory_tree_worker.h \
    src/worker/gui/qcdm_efs_file_read_worker.h \
    src/worker/gui/qcdm_efs_file_write_worker.h \
    src/worker/gui/qcdm_memory_read_worker.h \
    src/worker/gui/qcdm_prl_read_worker.h \
    src/worker/gui/qcdm_prl_write_worker.h \
    src/worker/gui/qcdm_nv_item_read_worker.h \
    src/gui/application.h 


FORMS  += resources/ui/qcdm_window.ui
FORMS  += resources/ui/about_dialog.ui

RESOURCES = resources/qcdm.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/  -llibopenpst -lserial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug/ -llibopenpst -lserial
else:unix: LIBS += -L$$OUT_PWD/ -llibopenpst -lserial
