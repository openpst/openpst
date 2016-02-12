all: default

#CXXFLAGS =	-O0 -g3 -Wall -fmessage-length=0 -MMD -MP -std=c++11 	
CXXFLAGS =	-O0 -g3 -w -fmessage-length=0 -MMD -MP -std=c++11 	
OBJS = 

START_DIR=$(PWD)
BASE_DIR=$(PWD)/../../
BUILD_DIR=$(BASE_DIR)/build/extra
LIB_DIR=$(BASE_DIR)/lib
OPENPST_BASE_DIR=$(BASE_DIR)/src
clean:
	rm -rf $(BUILD_DIR)/*

default:
	@mkdir -p $(BUILD_DIR)
	$(CXX) \
		-I"$(LIB_DIR)/serial/include" \
		-I"$(LIB_DIR)/serial/src" \
		-I"$(BASE_DIR)/src" \
		$(LIB_DIR)/serial/src/impl/list_ports/list_ports_linux.cc  \
		$(LIB_DIR)/serial/src/impl/unix.cc \
		$(LIB_DIR)/serial/src/serial.cc \
		$(OPENPST_BASE_DIR)/qc/dm_efs_manager.cpp \
		$(OPENPST_BASE_DIR)/qc/dm_efs_node.cpp \
		$(OPENPST_BASE_DIR)/serial/hdlc_serial.cpp \
		$(OPENPST_BASE_DIR)/serial/qcdm_serial.cpp \
		$(OPENPST_BASE_DIR)/qc/hdlc.cpp \
		$(OPENPST_BASE_DIR)/util/sleep.cpp \
		$(OPENPST_BASE_DIR)/util/hexdump.cpp \
		main.cpp \
		-o $(BUILD_DIR)/qcsamunlock \
		$(CXXFLAGS)
		

#include <iostream>
#include <string>
#include <stdio.h>
#include "include/definitions.h"
#include "util/sleep.h"
#include "serial/qcdm_serial.h"
#include "qc/dm.h"
#include "qc/dm_efs_manager.h"