all: openpst

openpst:	
	qmake -makefile -o ./build/Makefile OpenPST.pro 
	$(MAKE) -C build

travis:
	/opt/qt54/bin/qt54-env.sh 
	/opt/qt54/bin/qmake -makefile -o ./build/Makefile OpenPST.pro 

libopenpst:
	$(CXX) \
		-I"./lib/serial/include" \
		-I"./src" \
	    src/qc/hdlc_encoder.cpp \
	    src/qc/dm_efs_manager.cpp \
	    src/qc/dm_efs_node.cpp \
	    src/qc/hdlc_serial.cpp \
	    src/qc/qcdm_serial.cpp \
	    src/qc/sahara_serial.cpp \
	    src/qc/streaming_dload_serial.cpp \
	    src/samsung/akseed_calculator.cpp \
	    src/util/convert.cpp \
	    src/util/endian.cpp \
	    src/util/hexdump.cpp \
	    src/util/sleep.cpp 
		-o ./build/libopenpst \
		-O0 -g3 -std=c++11 -Wall

clean:
	rm -rf build