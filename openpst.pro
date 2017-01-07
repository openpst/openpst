#-------------------------------------------------
# QMake Build Script for: openpst/openpst
#-------------------------------------------------

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libopenpst sahara streaming_dload qcdm mbn_tool

libopenpst.file 		= libopenpst/libopenpst.pro
sahara.file 			= sahara/sahara.pro
streaming_dload.file    = streaming-dload/streaming_dload.pro
qcdm.file 				= qcdm/qcdm.pro
mbn_tool.file 			= mbn-tool/mbn_tool.pro

sahara.depends 			= libopenpst
streaming_dload.depends = libopenpst
qcdm.depends 			= libopenpst
