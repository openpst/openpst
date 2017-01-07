####
## Makefile for openpst/openpst
####

all: openpst

openpst:	
	if [ ! -d "./libopenpst/include" ] || [ ! -d "./gui-common/include" ]; then git submodule init && git submodule update;  fi
	if [ ! -d "./libopenpst/lib/serial/include" ]; then cd ./libopenpst/ && git submodule init && git submodule update;  fi
	qmake -makefile -o ./build/Makefile openpst.pro 
	$(MAKE) -C build

clean:
	rm -rf build