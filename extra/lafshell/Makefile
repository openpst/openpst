all: default

CXXFLAGS =	-O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP
OBJS = 

START_DIR=$(PWD)
BASE_DIR=$(PWD)/../../
BUILD_DIR=$(BASE_DIR)/build/extra
LIB_DIR=$(BASE_DIR)/lib

clean:
	rm -rf $(BUILD_DIR)/*
#	$(MAKE) -C lib/serial clean

default:
	@mkdir -p $(BUILD_DIR)
	$(CXX) \
		-I"$(LIB_DIR)/serial/include" \
		-I"$(LIB_DIR)/serial/src" \
		-o $(BUILD_DIR)/lafshell \
		$(LIB_DIR)/serial/src/impl/list_ports/list_ports_linux.cc  \
		$(LIB_DIR)/serial/src/impl/unix.cc \
		$(LIB_DIR)/serial/src/serial.cc \
		main.cpp 
