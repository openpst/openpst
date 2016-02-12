#-------------------------------------------------
# PLACEHOLDER
#-------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered


SUBDIRS = serial libopenpst sahara streaming_dload qcdm qcsamunlock mbn_tool

serial.file 			= qmake/serial.pro
libopenpst.file 		= qmake/libopenpst.pro
sahara.file 			= qmake/sahara.pro
streaming_dload.file            = qmake/streaming_dload.pro
qcdm.file 			= qmake/qcdm.pro
qcsamunlock.file                = qmake/qcsamunlock.pro
mbn_tool.file 			= qmake/mbn_tool.pro

libopenpst.depends 		= serial
qcdm.depends 			= libopenpst
qcsamunlock.depends             = libopenpst
sahara.depends 			= libopenpst
streaming_dload.depends         = libopenpst
